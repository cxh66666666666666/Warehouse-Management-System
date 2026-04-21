**错误码设计规范（待落地）**

*注：以下为建议采用的错误码规范，当前代码**尚未按此规范修改**，仅供参考。*

---

*   **格式**：`[1位错误大类] + [2位业务模块] + [2位具体错误]` (例如 `10101`)。
*   **错误大类**：
    *   `1`：参数错误 (Bad Request)
    *   `2`：资源不存在 (Not Found)
    *   `3`：业务逻辑错误 (Business Error / Conflict)
    *   `4`：系统与数据库错误 (Internal Server Error)
*   **业务模块**：
    *   `00`：全局通用
    *   `01`：商品模块 (Product)
    *   `02`：订单模块 (Order)
    *   `03`：库存模块 (Inventory)

---

### 1. 参数错误 (1xxxx)
*主要对应 HTTP 400 状态码，表示客户端传来的数据有问题。*

| 错误码 | 英文枚举 (枚举名) | 中文描述 | 对应场景 |
| :--- | :--- | :--- | :--- |
| **10001** | `PARAM_IS_EMPTY` | 请求体为空 | `product_controller.cc` 里的 `No JSON data provided.` |
| **10002** | `PARAM_VALIDATE_FAILED` | 参数格式校验失败 | JSON 解析失败或缺少必填字段 |
| **10101** | `PRODUCT_FIELD_INVALID` | 商品字段不合法 | `Product::validateJsonForCreation` 抛出的具体字段错误 |
| **10201** | `ORDER_ITEM_INVALID` | 订单明细非法 | `order_service.cpp` 里的 `非法商品ID或数量` |
| **10202** | `ORDER_PRICE_MISMATCH` | 提交价格与实际不符 | `order_service.cpp` 里的 `商品价格校验失败` |

---

### 2. 资源不存在 (2xxxx)
*主要对应 HTTP 404 状态码，表示客户端请求的数据在数据库里找不到。*

| 错误码 | 英文枚举 (枚举名) | 中文描述 | 对应场景 |
| :--- | :--- | :--- | :--- |
| **20101** | `PRODUCT_NOT_FOUND` | 商品不存在 | `product_controller.cc` 捕获到 `UnexpectedRows`，或 `Product not found.` |
| **20201** | `ORDER_NOT_FOUND` | 订单不存在 | 未来查询订单详情时找不到 |

---

### 3. 业务错误 (3xxxx)
*主要对应 HTTP 409 或 400 状态码，参数虽然合法，但违反了系统的核心业务规则。*

| 错误码 | 英文枚举 (枚举名) | 中文描述 | 对应场景 |
| :--- | :--- | :--- | :--- |
| **30201** | `ORDER_AMOUNT_OVERFLOW` | 订单金额计算溢出 | `order_service.cpp` 里的 `订单金额溢出` |
| **30202** | `ORDER_AMOUNT_MISMATCH`| 订单总金额不一致 | `order_service.cpp` 里的 `订单总金额不一致` |
| **30301** | `INVENTORY_CONFLICT` | 库存不足或并发冲突 | `order_service.cpp` 里的 `库存冲突/超卖` (`InventoryConflictError`) |
| **30302** | `PRODUCT_NOT_AVAILABLE`| 商品已下架 | 未来基于 `available_listing` 字段拦截下单 |

---

### 4. 数据库与系统错误 (4xxxx)
*主要对应 HTTP 500 状态码，表示服务器内部崩溃或数据库异常。*

| 错误码 | 英文枚举 (枚举名) | 中文描述 | 对应场景 |
| :--- | :--- | :--- | :--- |
| **40001** | `DB_CLIENT_MISSING` | 数据库连接丢失 | `Database client not found.` |
| **40002** | `DB_OPERATION_ERROR` | 数据库执行异常 | 捕获到底层的 `DrogonDbException` |
| **40099** | `SYSTEM_INTERNAL_ERROR`| 系统未知异常 | `order_service.cpp` 里最后的 `catch (const std::exception& e)` |

---

**当前落地状态**：此错误码规范为**待落地设计**，当前代码中的错误返回尚未完全按此规范实现。
