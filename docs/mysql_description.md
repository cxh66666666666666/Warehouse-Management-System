### **1. product（商品表）**
- **主键**: `product_id`
- **字段信息**:
  - `product_id` (int, 对应 `int32_t`) - 商品ID
  - `product_name` (varchar(50), 对应 `std::string`) - 商品名称
  - `price` (int, 对应 `int32_t`) - 价格
  - `available_listing` (tinyint, 对应 `int8_t`) - 是否上架/有效状态
  - `sales_volume` (int, 对应 `int32_t`) - 销量
- **关联关系**:
  - 被 `inventory` 表关联，`inventory` 表同样使用 `product_id` 作为主键。
  - 被 `order_detail_table` 表关联（订单明细中的购买商品）。

### **2. inventory（库存表）**
- **主键**: `product_id`
- **字段信息**:
  - `product_id` (int, 对应 `int32_t`) - 商品ID（主键，同时也起到了外键的作用，指向 `product` 表）
  - `inventory_quantity` (int, 对应 `int32_t`) - 库存数量
  - `version` (int, 对应 `int32_t`) - 版本号（通常用于实现乐观锁并发控制）
- **关联关系**:
  - 与 `product` 表是 **一对一** 关系，通过 `product_id` 直接绑定商品及其对应的库存。

### **3. order_main_table（订单主表）**
- **主键**: `order_main_table_id`
- **字段信息**:
  - `order_main_table_id` (int, 对应 `int32_t`) - 订单主表ID
  - `user_id` (int, 对应 `int32_t`) - 用户ID（关联用户，不过用户表不在本次分析范围内）
  - `order_number` (varchar(32), 对应 `std::string`) - 订单号
  - `purchase_date` (datetime, 对应 `::trantor::Date`) - 购买日期/下单时间
  - `total_order_amount` (decimal(10,0), 对应 `std::string`) - 订单总金额
- **关联关系**:
  - 与 `order_detail_table` 表是 **一对多** 关系，一个订单主表记录可以对应多个订单明细记录。

### **4. order_detail_table（订单明细表）**
- **主键**: `order_detail_table_id`
- **字段信息**:
  - `order_detail_table_id` (int, 对应 `int32_t`) - 订单明细表ID
  - `order_main_table_id` (int, 对应 `int32_t`) - 订单主表ID
  - `product_id` (int, 对应 `int32_t`) - 商品ID
  - `quantity_purchased` (int, 对应 `int32_t`) - 购买数量
  - `price_at_purchase` (decimal(10,0), 对应 `std::string`) - 购买时的单价
- **关联关系**:
  - **属于订单**：通过 `order_main_table_id` 字段关联到 `order_main_table`，表示该明细属于哪一个主订单。
  - **包含商品**：通过 `product_id` 字段关联到 `product` 表，记录用户具体购买的哪一款商品。

### **总结：表之间的整体流转与关系**
- `product` (商品) 和 `inventory` (库存) 通过相同的 `product_id` 进行一对一的关联，解耦了商品基础信息和高频变动的库存信息。
- 用户下单时，生成一条 `order_main_table` 记录。
- 订单中包含的各种商品，对应生成一条或多条 `order_detail_table` 记录，每条明细通过 `order_main_table_id` 指向主订单，通过 `product_id` 指向具体商品，并记录购买时的快照价格 (`price_at_purchase`) 和数量 (`quantity_purchased`)。