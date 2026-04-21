### **1. product（商品表）**

#### ORM 映射信息（来自 models/）
- **主键**: `product_id` (int, 对应 `int32_t`)
- **字段**:
  - `product_id` - 商品ID
  - `product_name` (varchar(50)) - 商品名称
  - `price` (int) - 价格，**单位为分**
  - `available_listing` (tinyint) - 上架状态，**0=下架，1=上架**
  - `sales_volume` (int) - 销量

#### 关联关系
- 被 `inventory` 表一对一关联
- 被 `order_detail_table` 表关联（订单明细中的购买商品）

---

### **2. inventory（库存表）**

#### ORM 映射信息（来自 models/）
- **主键**: `product_id` (int) - 商品ID，同时也是外键
- **字段**:
  - `inventory_quantity` (int) - 库存数量
  - `version` (int) - 版本号，用于**乐观锁**并发控制

#### 关联关系
- 与 `product` 表是 **一对一** 关系

---

### **3. order_main_table（订单主表）**

#### ORM 映射信息（来自 models/）
- **主键**: `order_main_table_id` (int)
- **字段**:
  - `user_id` (int) - 用户ID
  - `order_number` (varchar(32)) - 订单号
  - `purchase_date` (datetime) - 下单时间
  - `total_order_amount` (decimal(10,0)) - 订单总金额，**使用字符串存储避免浮点精度问题**

#### 关联关系
- 与 `order_detail_table` 是 **一对多** 关系

---

### **4. order_detail_table（订单明细表）**

#### ORM 映射信息（来自 models/）
- **主键**: `order_detail_table_id` (int)
- **字段**:
  - `order_main_table_id` (int) - 关联订单主表
  - `product_id` (int) - 关联商品
  - `quantity_purchased` (int) - 购买数量
  - `price_at_purchase` (decimal(10,0)) - 购买时的单价，**快照价格**

#### 关联关系
- 通过 `order_main_table_id` 属于某个主订单
- 通过 `product_id` 记录具体购买的商品

---

## 设计说明

### 为什么金额字段使用 decimal(10,0) 而非浮点？
- 浮点数（float/double）在计算时存在精度损失
- 金额计算要求精确，使用字符串存储 decimal 或整数（分）是常见做法

### 为什么 inventory 要独立成表？
- 商品信息（名称、价格）变动频率低
- 库存数量高频变动，独立成表便于**乐观锁并发控制**
- 解耦频繁变更的库存数据与相对稳定的商品基础信息

### 为什么使用乐观锁（version 字段）？
- 防止库存超卖
- 通过 `WHERE version = ? AND quantity >= ?` 原子性扣减

---

## 索引设计说明（推断）

根据业务场景，推测索引设计如下：

| 表名 | 索引类型 | 索引字段 | 用途 |
| :--- | :--- | :--- | :--- |
| `product` | PRIMARY | `product_id` | 主键 |
| `inventory` | PRIMARY | `product_id` | 主键+外键 |
| `order_main_table` | PRIMARY | `order_main_table_id` | 主键 |
| `order_main_table` | UNIQUE | `order_number` | 订单号唯一 |
| `order_detail_table` | PRIMARY | `order_detail_table_id` | 主键 |
| `order_detail_table` | INDEX | `order_main_table_id` | 关联查询 |

*注：具体索引以数据库 DDL 文件为准，此处为基于 ORM 映射的合理推断。*
