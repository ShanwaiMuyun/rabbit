CREATE DATABASE IF NOT EXISTS ecommerce
  DEFAULT CHARACTER SET utf8mb4
  DEFAULT COLLATE utf8mb4_unicode_ci;

USE ecommerce;

CREATE TABLE IF NOT EXISTS users (
  id INT AUTO_INCREMENT PRIMARY KEY,
  username VARCHAR(50) NOT NULL UNIQUE,
  password VARCHAR(100) NOT NULL,
  role ENUM('buyer', 'seller', 'admin') NOT NULL DEFAULT 'buyer',
  status ENUM('pending', 'approved', 'rejected') NOT NULL DEFAULT 'pending',
  reviewed_by VARCHAR(50) NULL,
  reviewed_at DATETIME NULL,
  created_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS messages (
  id INT AUTO_INCREMENT PRIMARY KEY,
  username VARCHAR(50) NOT NULL,
  content TEXT NOT NULL,
  created_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
  INDEX idx_messages_username (username)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS products (
  id INT AUTO_INCREMENT PRIMARY KEY,
  seller_username VARCHAR(50) NOT NULL,
  name VARCHAR(120) NOT NULL,
  description TEXT NULL,
  category VARCHAR(50) NOT NULL DEFAULT 'others',
  image_url VARCHAR(500) NOT NULL,
  price DECIMAL(10, 2) NOT NULL,
  stock INT NOT NULL DEFAULT 0,
  status ENUM('pending', 'approved', 'rejected') NOT NULL DEFAULT 'pending',
  reviewed_by VARCHAR(50) NULL,
  reviewed_at DATETIME NULL,
  created_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
  updated_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  INDEX idx_products_category (category),
  INDEX idx_products_status (status),
  INDEX idx_products_seller (seller_username),
  INDEX idx_products_name (name)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

ALTER TABLE products
  ADD COLUMN IF NOT EXISTS category VARCHAR(50) NOT NULL DEFAULT 'others' AFTER description;

ALTER TABLE products
  ADD INDEX IF NOT EXISTS idx_products_category (category);

CREATE TABLE IF NOT EXISTS orders (
  id INT AUTO_INCREMENT PRIMARY KEY,
  buyer_username VARCHAR(50) NOT NULL,
  seller_username VARCHAR(50) NOT NULL,
  product_id INT NOT NULL,
  quantity INT NOT NULL,
  total_price DECIMAL(10, 2) NOT NULL,
  address VARCHAR(500) NOT NULL,
  status ENUM('ordered', 'shipped', 'received') NOT NULL DEFAULT 'ordered',
  created_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
  updated_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  INDEX idx_orders_buyer (buyer_username),
  INDEX idx_orders_seller (seller_username),
  INDEX idx_orders_status (status),
  CONSTRAINT fk_orders_product FOREIGN KEY (product_id) REFERENCES products(id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

INSERT INTO users (username, password, role, status)
SELECT 'admin', 'admin123', 'admin', 'approved'
WHERE NOT EXISTS (SELECT 1 FROM users WHERE username = 'admin');

INSERT INTO users (username, password, role, status, reviewed_by, reviewed_at)
SELECT 'buyer01', '123456', 'buyer', 'approved', 'admin', NOW()
WHERE NOT EXISTS (SELECT 1 FROM users WHERE username = 'buyer01');

INSERT INTO users (username, password, role, status, reviewed_by, reviewed_at)
SELECT 'buyer02', '123456', 'buyer', 'approved', 'admin', NOW()
WHERE NOT EXISTS (SELECT 1 FROM users WHERE username = 'buyer02');

INSERT INTO users (username, password, role, status, reviewed_by, reviewed_at)
SELECT 'seller01', '123456', 'seller', 'approved', 'admin', NOW()
WHERE NOT EXISTS (SELECT 1 FROM users WHERE username = 'seller01');

INSERT INTO users (username, password, role, status, reviewed_by, reviewed_at)
SELECT 'seller02', '123456', 'seller', 'approved', 'admin', NOW()
WHERE NOT EXISTS (SELECT 1 FROM users WHERE username = 'seller02');

INSERT INTO users (username, password, role, status)
SELECT 'new_buyer', '123456', 'buyer', 'pending'
WHERE NOT EXISTS (SELECT 1 FROM users WHERE username = 'new_buyer');

INSERT INTO users (username, password, role, status)
SELECT 'new_seller', '123456', 'seller', 'pending'
WHERE NOT EXISTS (SELECT 1 FROM users WHERE username = 'new_seller');

INSERT INTO products
  (seller_username, name, description, category, image_url, price, stock, status, reviewed_by, reviewed_at)
SELECT
  'seller01',
  '鲜果礼盒',
  '精选苹果、橙子、猕猴桃等应季水果，适合家庭日常和节日送礼。',
  'fresh-food',
  'https://upload.wikimedia.org/wikipedia/commons/thumb/5/5f/Wrapped_fruit_basket.jpg/960px-Wrapped_fruit_basket.jpg',
  89.90,
  80,
  'approved',
  'admin',
  NOW()
WHERE NOT EXISTS (SELECT 1 FROM products WHERE name = '鲜果礼盒' AND seller_username = 'seller01');

INSERT INTO products
  (seller_username, name, description, category, image_url, price, stock, status, reviewed_by, reviewed_at)
SELECT
  'seller01',
  '有机蔬菜组合',
  '包含生菜、番茄、胡萝卜、黄瓜等蔬菜，产地直发。',
  'fresh-food',
  'https://images.unsplash.com/photo-1540420773420-3366772f4999?auto=format&fit=crop&w=800&q=80',
  45.80,
  120,
  'approved',
  'admin',
  NOW()
WHERE NOT EXISTS (SELECT 1 FROM products WHERE name = '有机蔬菜组合' AND seller_username = 'seller01');

INSERT INTO products
  (seller_username, name, description, category, image_url, price, stock, status, reviewed_by, reviewed_at)
SELECT
  'seller02',
  '精品坚果混合装',
  '每日坚果独立包装，包含核桃、腰果、巴旦木和蔓越莓干。',
  'fresh-food',
  'https://images.unsplash.com/photo-1599599810769-bcde5a160d32?auto=format&fit=crop&w=800&q=80',
  68.00,
  60,
  'approved',
  'admin',
  NOW()
WHERE NOT EXISTS (SELECT 1 FROM products WHERE name = '精品坚果混合装' AND seller_username = 'seller02');

INSERT INTO products
  (seller_username, name, description, category, image_url, price, stock, status)
SELECT
  'seller02',
  '待审核蜂蜜',
  '农家自产蜂蜜，等待管理员审核后上架展示。',
  'fresh-food',
  'https://images.unsplash.com/photo-1587049352851-8d4e89133924?auto=format&fit=crop&w=800&q=80',
  56.00,
  35,
  'pending'
WHERE NOT EXISTS (SELECT 1 FROM products WHERE name = '待审核蜂蜜' AND seller_username = 'seller02');

INSERT INTO products
  (seller_username, name, description, category, image_url, price, stock, status, reviewed_by, reviewed_at)
SELECT
  'seller01',
  '已拒绝测试商品',
  '用于演示商品审核拒绝状态，不会展示给普通买家。',
  'others',
  'https://images.unsplash.com/photo-1528825871115-3581a5387919?auto=format&fit=crop&w=800&q=80',
  19.90,
  20,
  'rejected',
  'admin',
  NOW()
WHERE NOT EXISTS (SELECT 1 FROM products WHERE name = '已拒绝测试商品' AND seller_username = 'seller01');

UPDATE products SET category = 'digital'
WHERE name IN ('无线蓝牙耳机', '机械键盘', '智能手表', '便携式迷你投影仪');

UPDATE products SET category = 'bags'
WHERE name IN ('双肩背包');

UPDATE products SET category = 'fresh-food'
WHERE name IN ('鲜果礼盒', '有机蔬菜组合', '精品坚果混合装', '待审核蜂蜜');

UPDATE products SET category = 'others'
WHERE name IN ('待审核商品', '已拒绝测试商品') OR category = '';

INSERT INTO messages (username, content)
SELECT 'buyer01', '商品图片展示清楚，搜索和下单流程都可以正常使用。'
WHERE NOT EXISTS (SELECT 1 FROM messages WHERE username = 'buyer01' AND content = '商品图片展示清楚，搜索和下单流程都可以正常使用。');

INSERT INTO messages (username, content)
SELECT 'seller01', '卖家可以发布商品，等待管理员审核后再展示给买家。'
WHERE NOT EXISTS (SELECT 1 FROM messages WHERE username = 'seller01' AND content = '卖家可以发布商品，等待管理员审核后再展示给买家。');

INSERT INTO messages (username, content)
SELECT 'admin', '管理员负责审核新用户和待上架商品。'
WHERE NOT EXISTS (SELECT 1 FROM messages WHERE username = 'admin' AND content = '管理员负责审核新用户和待上架商品。');

INSERT INTO orders
  (buyer_username, seller_username, product_id, quantity, total_price, address, status)
SELECT
  'buyer01',
  p.seller_username,
  p.id,
  1,
  p.price,
  '北京市海淀区中关村示例路 1 号',
  'ordered'
FROM products p
WHERE p.name = '鲜果礼盒'
  AND p.seller_username = 'seller01'
  AND NOT EXISTS (
    SELECT 1 FROM orders o
    WHERE o.buyer_username = 'buyer01'
      AND o.product_id = p.id
      AND o.status = 'ordered'
  );

INSERT INTO orders
  (buyer_username, seller_username, product_id, quantity, total_price, address, status)
SELECT
  'buyer01',
  p.seller_username,
  p.id,
  2,
  p.price * 2,
  '上海市浦东新区世纪大道 88 号',
  'shipped'
FROM products p
WHERE p.name = '有机蔬菜组合'
  AND p.seller_username = 'seller01'
  AND NOT EXISTS (
    SELECT 1 FROM orders o
    WHERE o.buyer_username = 'buyer01'
      AND o.product_id = p.id
      AND o.status = 'shipped'
  );

INSERT INTO orders
  (buyer_username, seller_username, product_id, quantity, total_price, address, status)
SELECT
  'buyer02',
  p.seller_username,
  p.id,
  1,
  p.price,
  '广东省广州市天河区体育西路 66 号',
  'received'
FROM products p
WHERE p.name = '精品坚果混合装'
  AND p.seller_username = 'seller02'
  AND NOT EXISTS (
    SELECT 1 FROM orders o
    WHERE o.buyer_username = 'buyer02'
      AND o.product_id = p.id
      AND o.status = 'received'
  );
