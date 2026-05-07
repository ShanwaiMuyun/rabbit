#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h> 
#include <iostream>
#include <string>
#include <mysql.h>         // MySQL C API
#include "httplib.h"       // cpp-httplib
#include <json.hpp>        // nlohmann/json

using json = nlohmann::json;

// --- 数据库配置 (统一管理) ---
const char* DB_HOST = "localhost";
const char* DB_USER = "root";
const char* DB_PASS = "Zjq@050910";
const char* DB_NAME = "ecommerce";

// --- 1. 登录逻辑 ---
bool login(const std::string& username, const std::string& password) {
    MYSQL* conn = mysql_init(nullptr);
    if (!mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, nullptr, 0)) {
        std::cerr << "MySQL connection failed: " << mysql_error(conn) << std::endl;
        return false;
    }

    char username_esc[256], password_esc[256];
    mysql_real_escape_string(conn, username_esc, username.c_str(), (unsigned long)username.length());
    mysql_real_escape_string(conn, password_esc, password.c_str(), (unsigned long)password.length());

    std::string query = "SELECT * FROM users WHERE username='" + std::string(username_esc) +
                        "' AND password='" + std::string(password_esc) + "'";
    bool success = false;
    if (mysql_query(conn, query.c_str()) == 0) {
        MYSQL_RES* res = mysql_store_result(conn);
        if (res && mysql_num_rows(res) == 1) success = true;
        mysql_free_result(res);
    }

    mysql_close(conn);
    return success;
}

// --- 2. 注册逻辑 ---
// 返回值: 0-成功, 1-用户已存在, 2-数据库错误
int register_user(const std::string& username, const std::string& password) {
    MYSQL* conn = mysql_init(nullptr);
    if (!mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, nullptr, 0)) {
        return 2;
    }

    char username_esc[256], password_esc[256];
    mysql_real_escape_string(conn, username_esc, username.c_str(), (unsigned long)username.length());
    mysql_real_escape_string(conn, password_esc, password.c_str(), (unsigned long)password.length());

    // A. 检查用户名是否存在
    std::string check_query = "SELECT id FROM users WHERE username='" + std::string(username_esc) + "'";
    mysql_query(conn, check_query.c_str());
    MYSQL_RES* res = mysql_store_result(conn);
    if (res && mysql_num_rows(res) > 0) {
        mysql_free_result(res);
        mysql_close(conn);
        return 1; // 已存在
    }
    if (res) mysql_free_result(res);

    // B. 插入新记录
    std::string insert_query = "INSERT INTO users (username, password) VALUES ('" + 
                               std::string(username_esc) + "', '" + 
                               std::string(password_esc) + "')";
    
    int result = 0;
    if (mysql_query(conn, insert_query.c_str())) {
        std::cerr << "Insert error: " << mysql_error(conn) << std::endl;
        result = 2; // 插入失败
    }

    mysql_close(conn);
    return result;
}

// 写入购物车数据库
bool save_to_cart(const std::string& username, const json& item) {
    MYSQL* conn = mysql_init(nullptr);
    if (!mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, nullptr, 0)) {
        return false;
    }

    // 设置编码，防止中文规格乱码
    mysql_set_character_set(conn, "utf8mb4");

    // 提取并转义数据 (安全起见)
    std::string product_id = std::to_string(item["id"].get<int>());
    std::string name       = item["name"].get<std::string>();
    std::string picture    = item["picture"].get<std::string>();
    double price           = item["price"].get<double>();
    int count              = item["count"].get<int>();
    std::string attrs_text = item["attrsText"].get<std::string>();
    int selected          = item["selected"].get<bool>() ? 1 : 0;

    // 对字符串进行 SQL 转义
    auto escape = [&](std::string str) {
        char* out = new char[str.length() * 2 + 1];
        mysql_real_escape_string(conn, out, str.c_str(), str.length());
        std::string res(out);
        delete[] out;
        return res;
    };

    // 构建 SQL 语句
    // 如果 username + sku_id 已存在，则更新数量、价格和规格文本
    std::string sql = "INSERT INTO cart (username, product_id, name, picture, price, count, attrs_text, selected) "
                      "VALUES ('" + escape(username) + "', " + product_id + ", '"  +
                      escape(name) + "', '" + escape(picture) + "', " + std::to_string(price) + ", " + 
                      std::to_string(count) + ", '" + escape(attrs_text) + "', " + std::to_string(selected) + ") "
                      "ON DUPLICATE KEY UPDATE count = count + " + std::to_string(count) + 
                      ", price = " + std::to_string(price) + ", attrs_text = '" + escape(attrs_text) + "';";

    bool success = (mysql_query(conn, sql.c_str()) == 0);
    if (!success) {
        std::cerr << "Cart SQL Error: " << mysql_error(conn) << std::endl;
    }

    mysql_close(conn);
    return success;
}

// 结算后清空购物车（删除已选中的商品）
// 返回值: 0-成功, 1-数据库错误
int delete_selected_cart(const std::string& username) {
    MYSQL* conn = mysql_init(nullptr);
    if (!mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, nullptr, 0)) {
        return 1;
    }

    // 转义用户名防止注入
    char username_esc[256];
    mysql_real_escape_string(conn, username_esc, username.c_str(), (unsigned long)username.length());

    // SQL 逻辑：删除该用户下所有处于“选中”状态的商品
    // 如果你想“全清”（不考虑是否选中），去掉 AND selected = 1 即可
    std::string sql = "DELETE FROM cart WHERE username = '" + std::string(username_esc) + "' AND selected = 1";

    int result = 0;
    if (mysql_query(conn, sql.c_str())) {
        std::cerr << "Clear Cart Error: " << mysql_error(conn) << std::endl;
        result = 1;
    }

    mysql_close(conn);
    return result;
}

int main() {
    httplib::Server svr;

    // --- 统一 CORS 设置函数 ---
    auto set_cors = [](httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
    };

    // --- 处理 OPTIONS 预检请求 ---
    svr.Options(R"(/.*)", [&](const httplib::Request& req, httplib::Response& res) {
        set_cors(res);
        res.status = 200;
    });

    // --- 登录路由 ---
    svr.Post("/login", [&](const httplib::Request& req, httplib::Response& res) {
        set_cors(res);
        try {
            auto j = json::parse(req.body);
            std::string user = j.value("username", "");
            std::string pass = j.value("password", "");

            bool ok = login(user, pass);
            json resp = { {"success", ok}, {"message", ok ? "登录成功" : "用户名或密码错误"} };
            res.set_content(resp.dump(), "application/json");
        } catch (...) {
            res.status = 400;
            res.set_content(R"({"error":"Invalid JSON"})", "application/json");
        }
    });

    // --- 注册路由 ---
    svr.Post("/register", [&](const httplib::Request& req, httplib::Response& res) {
        set_cors(res);
        try {
            auto j = json::parse(req.body);
            std::string user = j.value("username", "");
            std::string pass = j.value("password", "");

            if (user.empty() || pass.empty()) {
                res.set_content(R"({"success":false,"message":"用户名或密码不能为空"})", "application/json");
                return;
            }

            int status = register_user(user, pass);
            json resp;
            if (status == 0) {
                resp = {{"success", true}, {"message", "注册成功"}};
            } else if (status == 1) {
                resp = {{"success", false}, {"message", "该用户名已被占用"}};
            } else {
                res.status = 500;
                resp = {{"success", false}, {"message", "服务器数据库错误"}};
            }
            res.set_content(resp.dump(), "application/json");
        } catch (...) {
            res.status = 400;
            res.set_content(R"({"error":"Invalid JSON"})", "application/json");
        }
    });
    // --- 购物车导入接口 ---
    svr.Post("/add_cart", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        try {
            auto j = json::parse(req.body);
            
            std::string username = j.value("username", "guest_user"); 
            
            // j["cartItem"] 对应前端传来的对象
            if (save_to_cart(username, j)) {
                res.set_content(R"({"success":true, "message":"已同步至服务器购物车"})", "application/json");
            } else {
                res.status = 500;
                res.set_content(R"({"success":false, "message":"数据库写入失败"})", "application/json");
            }
        } catch (const std::exception& e) {
            res.status = 400;
            res.set_content(R"({"error":"数据格式不正确"})", "application/json");
        }
    });
    // --- 购物车删除接口 ---
    svr.Post("/delete", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        try {
            auto j = json::parse(req.body);
            std::string username = j.value("username", "");

            if (username.empty()) {
                res.set_content(R"({"success":false,"message":"未提供用户信息"})", "application/json");
                return;
            }

            // 结算成功后，执行清空操作
            int status = delete_selected_cart(username);

            if (status == 0) {
                res.set_content(R"({"success":true, "message":"结算成功，购物车已更新"})", "application/json");
            } else {
                res.status = 500;
                res.set_content(R"({"success":false, "message":"结算失败：数据库同步错误"})", "application/json");
            }
        } catch (...) {
            res.status = 400;
            res.set_content(R"({"error":"无效的请求内容"})", "application/json");
        }
    });

    // --- 测试接口 ---
    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("C++ Auth Server is Running!", "text/plain");
    });

    std::cout << "服务已启动: http://localhost:8000" << std::endl;
    svr.listen("0.0.0.0", 8000);

    return 0;
}