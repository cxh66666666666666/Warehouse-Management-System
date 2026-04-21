### 1. 获取所有商品列表
*   **请求方法**: `GET`
*   **请求路径**: `/products`
*   **请求体**: 无
*   **成功返回体 (200 OK)**:
    ```json
    {
        "status": 200,
        "data": [
            { "product_id": 1, "product_name": "商品A", "price": 100, ... },
            { "product_id": 2, "product_name": "商品B", "price": 200, ... }
        ]
    }
    ```
*   **错误情况**:
    *   **500 Internal Server Error**: 数据库连接失败或查询异常。

---

### 2. 获取单件商品
*   **请求方法**: `GET`
*   **请求路径**: `/products/{id}` (例如 `/products/1`)
*   **请求体**: 无
*   **成功返回体 (200 OK)**:
    ```json
    {
        "status": 200,
        "data": { "product_id": 1, "product_name": "商品A", "price": 100, ... }
    }
    ```
*   **错误情况**:
    *   **404 Not Found**: 商品不存在。
    *   **500 Internal Server Error**: 数据库异常。

---

### 3. 录入新商品
*   **请求方法**: `POST`
*   **请求路径**: `/products`
*   **请求体 (JSON)**:
    ```json
    {
        "product_name": "新商品",
        "price": 150,
        "available_listing": 1,
        "sales_volume": 0
    }
    ```
    **字段类型说明**：
    - `product_name`: `string`，商品名称，varchar(50)
    - `price`: `int32`，价格，**单位为"分"**（非元）
    - `available_listing`: `int8`，上架状态，**0=下架，1=上架**
    - `sales_volume`: `int32`，销量

*   **成功返回体 (201 Created)**:
    ```json
    {
        "status": 201,
        "data": { "product_id": 3, "product_name": "新商品", "price": 150, ... }
    }
    ```
*   **错误情况**:
    *   **400 Bad Request**: 未提供 JSON 数据，或字段校验失败。
    *   **500 Internal Server Error**: 数据库插入失败。

---

### 4. 更新商品信息
*   **请求方法**: `PUT`
*   **请求路径**: `/products/{id}` (例如 `/products/1`)
*   **请求体 (JSON)**:
    ```json
    {
        "price": 180,
        "sales_volume": 10
    }
    ```
    **说明**：URL 路径中的 `id` 优先，JSON 中的 `product_id` 会被忽略。

*   **成功返回体 (200 OK)**:
    ```json
    {
        "status": 200,
        "data": { "product_id": 1, "product_name": "商品A", "price": 180, ... }
    }
    ```
*   **错误情况**:
    *   **400 Bad Request**: 字段校验失败，或没有提供有效更新字段。
    *   **404 Not Found**: 商品不存在。
    *   **500 Internal Server Error**: 数据库异常。

---

### 5. 删除商品信息
*   **请求方法**: `DELETE`
*   **请求路径**: `/products/{id}` (例如 `/products/1`)
*   **请求体**: 无
*   **成功返回体 (200 OK)**:
    ```json
    {
        "status": 200,
        "message": "Delete success.",
        "data": null
    }
    ```
*   **错误情况**:
    *   **404 Not Found**: 商品不存在。
    *   **500 Internal Server Error**: 数据库异常。

---

## 返回格式说明

> **当前实现状态**：以下为**当前实现**使用的返回格式。
>
> **目标规范**：未来计划统一为 `{ "code": 0, "message": "", "data": {} }` 格式。

当前返回格式存在不一致：
- 多数接口使用 `status` + `error`/`message` + `data`
- 删除成功使用 `message`，失败使用 `error`
