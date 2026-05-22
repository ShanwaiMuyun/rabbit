#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <mysql.h>
#include "httplib.h"
#include <json.hpp>

using json = nlohmann::json;

const char* DB_HOST = "localhost";
const char* DB_USER = "root";
const char* DB_PASS = "Zjq@050910";
const char* DB_NAME = "ecommerce";

struct Db {
    MYSQL* conn = nullptr;

    Db() {
        conn = mysql_init(nullptr);
        if (conn && mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, nullptr, 0)) {
            mysql_set_character_set(conn, "utf8mb4");
        } else {
            std::cerr << "MySQL connection failed: "
                      << (conn ? mysql_error(conn) : "mysql_init failed") << std::endl;
            if (conn) {
                mysql_close(conn);
            }
            conn = nullptr;
        }
    }

    ~Db() {
        if (conn) {
            mysql_close(conn);
        }
    }

    bool ok() const {
        return conn != nullptr;
    }

    std::string escape(const std::string& value) {
        std::string out(value.size() * 2 + 1, '\0');
        unsigned long len = mysql_real_escape_string(conn, &out[0], value.c_str(),
                                                     static_cast<unsigned long>(value.size()));
        out.resize(len);
        return out;
    }

    bool exec(const std::string& sql) {
        if (!ok()) {
            return false;
        }
        if (mysql_query(conn, sql.c_str()) != 0) {
            std::cerr << "SQL error: " << mysql_error(conn) << "\nSQL: " << sql << std::endl;
            return false;
        }
        return true;
    }

    json rows(const std::string& sql) {
        json result = json::array();
        if (!exec(sql)) {
            return result;
        }

        MYSQL_RES* raw = mysql_store_result(conn);
        if (!raw) {
            return result;
        }
        std::unique_ptr<MYSQL_RES, decltype(&mysql_free_result)> res(raw, mysql_free_result);

        int fields_count = mysql_num_fields(raw);
        MYSQL_FIELD* fields = mysql_fetch_fields(raw);
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(raw))) {
            unsigned long* lengths = mysql_fetch_lengths(raw);
            json item = json::object();
            for (int i = 0; i < fields_count; ++i) {
                item[fields[i].name] = row[i] ? std::string(row[i], lengths[i]) : "";
            }
            result.push_back(item);
        }
        return result;
    }

    long long insert_id() const {
        return static_cast<long long>(mysql_insert_id(conn));
    }
};

json success(const std::string& message, const json& data = json::object()) {
    return {{"success", true}, {"message", message}, {"data", data}};
}

json failure(const std::string& message) {
    return {{"success", false}, {"message", message}};
}

void write_json(httplib::Response& res, const json& body, int status = 200) {
    res.status = status;
    res.set_content(body.dump(), "application/json; charset=utf-8");
}

std::string body_value(const json& body, const std::string& key, const std::string& fallback = "") {
    if (!body.contains(key) || body[key].is_null()) {
        return fallback;
    }
    if (body[key].is_number_integer()) {
        return std::to_string(body[key].get<int>());
    }
    if (body[key].is_number_float()) {
        return std::to_string(body[key].get<double>());
    }
    return body[key].get<std::string>();
}

std::string query_value(const httplib::Request& req, const std::string& key,
                        const std::string& fallback = "") {
    return req.has_param(key) ? req.get_param_value(key) : fallback;
}

int to_int(const std::string& value, int fallback = 0) {
    try {
        return std::stoi(value);
    } catch (...) {
        return fallback;
    }
}

double to_double(const std::string& value, double fallback = 0.0) {
    try {
        return std::stod(value);
    } catch (...) {
        return fallback;
    }
}

bool is_admin(const std::string& username) {
    Db db;
    if (!db.ok()) {
        return false;
    }
    std::string sql = "SELECT id FROM users WHERE username='" + db.escape(username) +
                      "' AND role='admin' AND status='approved'";
    return !db.rows(sql).empty();
}

bool has_role(const std::string& username, const std::string& role) {
    Db db;
    if (!db.ok()) {
        return false;
    }
    std::string sql = "SELECT id FROM users WHERE username='" + db.escape(username) +
                      "' AND role='" + db.escape(role) + "' AND status='approved'";
    return !db.rows(sql).empty();
}

bool is_order_owner(Db& db, int order_id, const std::string& username, const std::string& column) {
    std::string sql = "SELECT id FROM orders WHERE id=" + std::to_string(order_id) + " AND " +
                      column + "='" + db.escape(username) + "'";
    return !db.rows(sql).empty();
}

int register_user(const std::string& username, const std::string& password,
                  const std::string& role) {
    Db db;
    if (!db.ok()) {
        return 2;
    }

    std::string safe_role = role == "seller" ? "seller" : "buyer";
    std::string check = "SELECT id FROM users WHERE username='" + db.escape(username) + "'";
    if (!db.rows(check).empty()) {
        return 1;
    }

    std::string sql = "INSERT INTO users(username,password,role,status) VALUES('" +
                      db.escape(username) + "','" + db.escape(password) + "','" +
                      safe_role + "','pending')";
    return db.exec(sql) ? 0 : 2;
}

json login_user(const std::string& username, const std::string& password) {
    Db db;
    if (!db.ok()) {
        return failure("数据库连接失败");
    }

    std::string sql = "SELECT id,username,role,status FROM users WHERE username='" +
                      db.escape(username) + "' AND password='" + db.escape(password) + "'";
    json rows = db.rows(sql);
    if (rows.empty()) {
        return failure("用户名或密码错误");
    }
    if (rows[0]["status"] != "approved") {
        return failure("账号尚未通过管理员审核");
    }

    json user = rows[0];
    user["token"] = "local-token-" + user["username"].get<std::string>();
    return success("登录成功", user);
}

bool add_message(const std::string& username, const std::string& content) {
    Db db;
    if (!db.ok()) {
        return false;
    }
    std::string sql = "INSERT INTO messages(username,content) VALUES('" + db.escape(username) +
                      "','" + db.escape(content) + "')";
    return db.exec(sql);
}

bool publish_product(const json& body) {
    Db db;
    if (!db.ok()) {
        return false;
    }

    std::string seller = body_value(body, "seller");
    std::string name = body_value(body, "name");
    std::string description = body_value(body, "description");
    std::string category = body_value(body, "category", "others");
    std::string image_url = body_value(body, "imageUrl");
    double price = to_double(body_value(body, "price"));
    int stock = to_int(body_value(body, "stock"));

    if (category != "digital" && category != "fresh-food" && category != "bags" &&
        category != "others") {
        category = "others";
    }

    if (seller.empty() || name.empty() || category.empty() || image_url.empty() || price <= 0 ||
        stock < 0 || !has_role(seller, "seller")) {
        return false;
    }

    std::string sql = "INSERT INTO products(seller_username,name,description,category,image_url,price,stock,status) "
                      "VALUES('" + db.escape(seller) + "','" + db.escape(name) + "','" +
                      db.escape(description) + "','" + db.escape(category) + "','" +
                      db.escape(image_url) + "'," + std::to_string(price) + "," +
                      std::to_string(stock) + ",'pending')";
    return db.exec(sql);
}

bool update_product(const json& body) {
    Db db;
    if (!db.ok()) {
        return false;
    }

    int product_id = to_int(body_value(body, "productId"));
    std::string seller = body_value(body, "seller");
    std::string name = body_value(body, "name");
    std::string description = body_value(body, "description");
    std::string category = body_value(body, "category", "others");
    std::string image_url = body_value(body, "imageUrl");
    double price = to_double(body_value(body, "price"));
    int stock = to_int(body_value(body, "stock"));

    if (category != "digital" && category != "fresh-food" && category != "bags" &&
        category != "others") {
        category = "others";
    }

    if (product_id <= 0 || seller.empty() || name.empty() || category.empty() || image_url.empty() ||
        price <= 0 || stock < 0 || !has_role(seller, "seller")) {
        return false;
    }

    std::string sql = "UPDATE products SET name='" + db.escape(name) +
                      "', description='" + db.escape(description) +
                      "', category='" + db.escape(category) +
                      "', image_url='" + db.escape(image_url) +
                      "', price=" + std::to_string(price) +
                      ", stock=" + std::to_string(stock) +
                      ", status='pending', reviewed_by=NULL, reviewed_at=NULL "
                      "WHERE id=" + std::to_string(product_id) +
                      " AND seller_username='" + db.escape(seller) + "'";
    return db.exec(sql) && mysql_affected_rows(db.conn) == 1;
}

bool delete_product(int product_id, const std::string& seller) {
    Db db;
    if (!db.ok() || product_id <= 0 || seller.empty() || !has_role(seller, "seller")) {
        return false;
    }

    std::string sql = "DELETE FROM products WHERE id=" + std::to_string(product_id) +
                      " AND seller_username='" + db.escape(seller) +
                      "' AND id NOT IN (SELECT product_id FROM orders)";
    return db.exec(sql) && mysql_affected_rows(db.conn) == 1;
}

json list_products(const std::string& keyword, bool approved_only) {
    Db db;
    if (!db.ok()) {
        return json::array();
    }

    std::string sql = "SELECT id,seller_username,name,description,category,image_url,price,stock,status,"
                      "created_at FROM products WHERE 1=1";
    if (approved_only) {
        sql += " AND status='approved'";
    }
    if (!keyword.empty()) {
        std::string key = db.escape(keyword);
        sql += " AND (name LIKE '%" + key + "%' OR description LIKE '%" + key + "%')";
    }
    sql += " ORDER BY created_at DESC";
    return db.rows(sql);
}

json list_users(const std::string& status) {
    Db db;
    if (!db.ok()) {
        return json::array();
    }

    std::string sql = "SELECT id,username,role,status,created_at,reviewed_by,reviewed_at FROM users";
    if (!status.empty()) {
        sql += " WHERE status='" + db.escape(status) + "'";
    }
    sql += " ORDER BY created_at DESC";
    return db.rows(sql);
}

bool review_item(const std::string& table, int id, const std::string& status,
                 const std::string& admin) {
    if (status != "approved" && status != "rejected") {
        return false;
    }
    if (!is_admin(admin)) {
        return false;
    }

    Db db;
    if (!db.ok()) {
        return false;
    }
    std::string sql = "UPDATE " + table + " SET status='" + status +
                      "', reviewed_by='" + db.escape(admin) +
                      "', reviewed_at=NOW() WHERE id=" + std::to_string(id);
    return db.exec(sql);
}

json create_order(const std::string& buyer, int product_id, int quantity,
                  const std::string& address) {
    Db db;
    if (!db.ok()) {
        return failure("数据库连接失败");
    }
    if (buyer.empty() || product_id <= 0 || quantity <= 0 || address.empty()) {
        return failure("订单参数不完整");
    }

    json products = db.rows("SELECT id,seller_username,price,stock,status FROM products WHERE id=" +
                            std::to_string(product_id));
    if (products.empty() || products[0]["status"] != "approved") {
        return failure("商品不存在或未通过审核");
    }

    int stock = to_int(products[0]["stock"].get<std::string>());
    if (stock < quantity) {
        return failure("库存不足");
    }

    std::string seller = products[0]["seller_username"].get<std::string>();
    double price = to_double(products[0]["price"].get<std::string>());
    double total = price * quantity;

    if (!db.exec("START TRANSACTION")) {
        return failure("创建订单失败");
    }

    std::string update_stock = "UPDATE products SET stock=stock-" + std::to_string(quantity) +
                               " WHERE id=" + std::to_string(product_id) +
                               " AND stock>=" + std::to_string(quantity);
    std::string insert_order =
        "INSERT INTO orders(buyer_username,seller_username,product_id,quantity,total_price,"
        "address,status) VALUES('" +
        db.escape(buyer) + "','" + db.escape(seller) + "'," + std::to_string(product_id) + "," +
        std::to_string(quantity) + "," + std::to_string(total) + ",'" + db.escape(address) +
        "','ordered')";

    bool ok = db.exec(update_stock) && mysql_affected_rows(db.conn) == 1 && db.exec(insert_order);
    long long order_id = db.insert_id();
    db.exec(ok ? "COMMIT" : "ROLLBACK");

    if (!ok) {
        return failure("创建订单失败");
    }
    return success("下单成功", {{"orderId", order_id}, {"status", "ordered"}});
}

json update_order_status(int order_id, const std::string& username, const std::string& action) {
    Db db;
    if (!db.ok()) {
        return failure("数据库连接失败");
    }

    std::string new_status;
    std::string old_status;
    std::string owner_column;

    if (action == "ship") {
        if (!has_role(username, "seller")) {
            return failure("只有已审核卖家可以发货");
        }
        old_status = "ordered";
        new_status = "shipped";
        owner_column = "seller_username";
    } else if (action == "receive") {
        old_status = "shipped";
        new_status = "received";
        owner_column = "buyer_username";
    } else {
        return failure("未知订单操作");
    }

    if (!is_order_owner(db, order_id, username, owner_column)) {
        return failure("没有权限操作该订单");
    }

    std::string sql = "UPDATE orders SET status='" + new_status + "', updated_at=NOW() WHERE id=" +
                      std::to_string(order_id) + " AND status='" + old_status + "'";
    if (!db.exec(sql) || mysql_affected_rows(db.conn) != 1) {
        return failure("订单状态不允许执行该操作");
    }
    return success("订单状态已更新", {{"orderId", order_id}, {"status", new_status}});
}

json list_orders(const std::string& username, const std::string& role) {
    Db db;
    if (!db.ok()) {
        return json::array();
    }

    std::string column = role == "seller" ? "seller_username" : "buyer_username";
    std::string sql = "SELECT o.id,o.buyer_username,o.seller_username,o.product_id,p.name AS product_name,"
                      "p.image_url,o.quantity,o.total_price,o.address,o.status,o.created_at,o.updated_at "
                      "FROM orders o JOIN products p ON p.id=o.product_id WHERE o." +
                      column + "='" + db.escape(username) + "' ORDER BY o.created_at DESC";
    return db.rows(sql);
}

int main() {
    httplib::Server svr;

    auto set_cors = [](httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "POST, GET, PUT, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    };

    svr.Options(R"(/.*)", [&](const httplib::Request&, httplib::Response& res) {
        set_cors(res);
        res.status = 200;
    });

    svr.Post("/register", [&](const httplib::Request& req, httplib::Response& res) {
        set_cors(res);
        try {
            json body = json::parse(req.body);
            std::string username = body_value(body, "username", body_value(body, "account"));
            std::string password = body_value(body, "password");
            std::string role = body_value(body, "role", "buyer");

            if (username.empty() || password.empty()) {
                write_json(res, failure("用户名或密码不能为空"), 400);
                return;
            }

            int status = register_user(username, password, role);
            if (status == 0) {
                write_json(res, success("注册成功，请等待管理员审核"));
            } else if (status == 1) {
                write_json(res, failure("用户名已被占用"), 409);
            } else {
                write_json(res, failure("数据库写入失败"), 500);
            }
        } catch (...) {
            write_json(res, failure("无效的 JSON 请求"), 400);
        }
    });

    svr.Post("/login", [&](const httplib::Request& req, httplib::Response& res) {
        set_cors(res);
        try {
            json body = json::parse(req.body);
            std::string username = body_value(body, "username", body_value(body, "account"));
            std::string password = body_value(body, "password");
            json result = login_user(username, password);
            write_json(res, result, result["success"].get<bool>() ? 200 : 401);
        } catch (...) {
            write_json(res, failure("无效的 JSON 请求"), 400);
        }
    });

    svr.Post("/messages", [&](const httplib::Request& req, httplib::Response& res) {
        set_cors(res);
        try {
            json body = json::parse(req.body);
            std::string username = body_value(body, "username");
            std::string content = body_value(body, "content");
            if (username.empty() || content.empty()) {
                write_json(res, failure("留言内容不能为空"), 400);
                return;
            }
            write_json(res, add_message(username, content) ? success("留言成功")
                                                           : failure("留言失败"));
        } catch (...) {
            write_json(res, failure("无效的 JSON 请求"), 400);
        }
    });

    svr.Get("/messages", [&](const httplib::Request& req, httplib::Response& res) {
        set_cors(res);
        Db db;
        if (!db.ok()) {
            write_json(res, failure("数据库连接失败"), 500);
            return;
        }
        write_json(res, success("查询成功", db.rows("SELECT id,username,content,created_at FROM "
                                                  "messages ORDER BY created_at DESC")));
    });

    svr.Post("/products", [&](const httplib::Request& req, httplib::Response& res) {
        set_cors(res);
        try {
            json body = json::parse(req.body);
            write_json(res, publish_product(body) ? success("商品发布成功，请等待管理员审核")
                                                  : failure("商品信息不完整或保存失败"));
        } catch (...) {
            write_json(res, failure("无效的 JSON 请求"), 400);
        }
    });

    svr.Put("/products", [&](const httplib::Request& req, httplib::Response& res) {
        set_cors(res);
        try {
            json body = json::parse(req.body);
            write_json(res, update_product(body) ? success("商品修改成功，请重新等待审核")
                                                 : failure("商品修改失败，请检查商品归属和参数"));
        } catch (...) {
            write_json(res, failure("无效的 JSON 请求"), 400);
        }
    });

    svr.Post("/products/delete", [&](const httplib::Request& req, httplib::Response& res) {
        set_cors(res);
        try {
            json body = json::parse(req.body);
            int product_id = to_int(body_value(body, "productId"));
            std::string seller = body_value(body, "seller");
            write_json(res, delete_product(product_id, seller)
                                ? success("商品删除成功")
                                : failure("商品删除失败，已有订单的商品不能直接删除"));
        } catch (...) {
            write_json(res, failure("无效的 JSON 请求"), 400);
        }
    });

    svr.Get("/products", [&](const httplib::Request& req, httplib::Response& res) {
        set_cors(res);
        std::string keyword = query_value(req, "keyword");
        bool approved_only = query_value(req, "approvedOnly", "1") != "0";
        write_json(res, success("查询成功", list_products(keyword, approved_only)));
    });

    svr.Get("/admin/users", [&](const httplib::Request& req, httplib::Response& res) {
        set_cors(res);
        std::string admin = query_value(req, "admin");
        if (!is_admin(admin)) {
            write_json(res, failure("没有管理员权限"), 403);
            return;
        }
        write_json(res, success("查询成功", list_users(query_value(req, "status"))));
    });

    svr.Post("/admin/users/review", [&](const httplib::Request& req, httplib::Response& res) {
        set_cors(res);
        try {
            json body = json::parse(req.body);
            int user_id = to_int(body_value(body, "userId"));
            std::string status = body_value(body, "status");
            std::string admin = body_value(body, "admin");
            write_json(res, review_item("users", user_id, status, admin)
                                ? success("用户审核完成")
                                : failure("审核失败，请检查管理员权限或状态参数"));
        } catch (...) {
            write_json(res, failure("无效的 JSON 请求"), 400);
        }
    });

    svr.Post("/admin/products/review", [&](const httplib::Request& req, httplib::Response& res) {
        set_cors(res);
        try {
            json body = json::parse(req.body);
            int product_id = to_int(body_value(body, "productId"));
            std::string status = body_value(body, "status");
            std::string admin = body_value(body, "admin");
            write_json(res, review_item("products", product_id, status, admin)
                                ? success("商品审核完成")
                                : failure("审核失败，请检查管理员权限或状态参数"));
        } catch (...) {
            write_json(res, failure("无效的 JSON 请求"), 400);
        }
    });

    svr.Post("/orders", [&](const httplib::Request& req, httplib::Response& res) {
        set_cors(res);
        try {
            json body = json::parse(req.body);
            std::string buyer = body_value(body, "buyer");
            int product_id = to_int(body_value(body, "productId"));
            int quantity = to_int(body_value(body, "quantity"), 1);
            std::string address = body_value(body, "address");
            json result = create_order(buyer, product_id, quantity, address);
            write_json(res, result, result["success"].get<bool>() ? 200 : 400);
        } catch (...) {
            write_json(res, failure("无效的 JSON 请求"), 400);
        }
    });

    svr.Get("/orders", [&](const httplib::Request& req, httplib::Response& res) {
        set_cors(res);
        std::string username = query_value(req, "username");
        std::string role = query_value(req, "role", "buyer");
        if (username.empty()) {
            write_json(res, failure("缺少 username"), 400);
            return;
        }
        write_json(res, success("查询成功", list_orders(username, role)));
    });

    svr.Post("/orders/ship", [&](const httplib::Request& req, httplib::Response& res) {
        set_cors(res);
        try {
            json body = json::parse(req.body);
            int order_id = to_int(body_value(body, "orderId"));
            std::string seller = body_value(body, "seller");
            json result = update_order_status(order_id, seller, "ship");
            write_json(res, result, result["success"].get<bool>() ? 200 : 400);
        } catch (...) {
            write_json(res, failure("无效的 JSON 请求"), 400);
        }
    });

    svr.Post("/orders/receive", [&](const httplib::Request& req, httplib::Response& res) {
        set_cors(res);
        try {
            json body = json::parse(req.body);
            int order_id = to_int(body_value(body, "orderId"));
            std::string buyer = body_value(body, "buyer");
            json result = update_order_status(order_id, buyer, "receive");
            write_json(res, result, result["success"].get<bool>() ? 200 : 400);
        } catch (...) {
            write_json(res, failure("无效的 JSON 请求"), 400);
        }
    });

    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("C++ ecommerce server is running at http://localhost:8000", "text/plain");
    });

    std::cout << "Server started: http://localhost:8000" << std::endl;
    svr.listen("0.0.0.0", 8000);
    return 0;
}
