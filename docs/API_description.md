### 1. 获取所有商品列表
*   **请求方法**: `GET`
*   **请求路径**: `/products`
*   **请求体**: 无
*   **成功返回体 (200 OK)**: 返回包含所有商品的 JSON 数组。
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
        ```json
        { "status": 500, "error": "Database client not found. / 具体异常信息", "data": null }
        ```

---

### 2. 获取单件商品
*   **请求方法**: `GET`
*   **请求路径**: `/products/{id}` (例如 `/products/1`)
*   **请求体**: 无
*   **成功返回体 (200 OK)**: 返回对应 ID 的商品 JSON 对象。
    ```json
    {
        "status": 200,
        "data": { "product_id": 1, "product_name": "商品A", "price": 100, ... }
    }
    ```
*   **错误情况**:
    *   **404 Not Found**: 数据库中不存在该 ID 的商品。
        ```json
        { "status": 404, "error": "Product not found.", "data": null }
        ```
    *   **500 Internal Server Error**: 数据库连接失败或查询异常。

---

### 3. 录入新商品
*   **请求方法**: `POST`
*   **请求路径**: `/products`
*   **请求体 (JSON)**: 包含要创建的商品信息（主键 ID 由数据库自增，无需传）。
    ```json
    {
        "product_name": "新商品",
        "price": 150,
        "available_listing": 1,
        "sales_volume": 0
    }
    ```
*   **成功返回体 (201 Created)**: 返回插入成功后带有自增 ID 的完整商品对象。
    ```json
    {
        "status": 201,
        "data": { "product_id": 3, "product_name": "新商品", "price": 150, ... }
    }
    ```
*   **错误情况**:
    *   **400 Bad Request**: 未提供 JSON 数据，或字段校验失败（如缺少必填项、类型错误等）。
        ```json
        { "status": 400, "error": "No JSON data provided. / 具体校验错误信息", "data": null }
        ```
    *   **500 Internal Server Error**: 数据库插入失败等异常。

---

### 4. 更新商品信息
*   **请求方法**: `PUT`
*   **请求路径**: `/products/{id}` (例如 `/products/1`)
*   **请求体 (JSON)**: 仅包含需要更新的字段即可。（代码中强制忽略 JSON 里传的 `product_id`，以 URL 路径中的 ID 为准）。
    ```json
    {
        "price": 180,
        "sales_volume": 10
    }
    ```
*   **成功返回体 (200 OK)**: 返回更新后的商品对象。
    ```json
    {
        "status": 200,
        "data": { "product_id": 1, "product_name": "商品A", "price": 180, ... }
    }
    ```
*   **错误情况**:
    *   **400 Bad Request**: 字段校验失败，或者 JSON 中没有提供任何可更新的字段（如 `product_name`, `price`, `available_listing`, `sales_volume`）。
        ```json
        { "status": 400, "error": "No updatable fields provided. / 校验错误信息", "data": null }
        ```
    *   **404 Not Found**: 要更新的商品 ID 在数据库中不存在。
    *   **500 Internal Server Error**: 数据库更新异常。

---

### 5. 删除商品信息
*   **请求方法**: `DELETE`
*   **请求路径**: `/products/{id}` (例如 `/products/1`)
*   **请求体**: 无
*   **成功返回体 (200 OK)**: *(注意：此接口使用 `message` 字段而不是 `error`)*
    ```json
    {
        "status": 200,
        "message": "Delete success.",
        "data": null
    }
    ```
*   **错误情况**:
    *   **404 Not Found**: 要删除的商品 ID 不存在。*(同样使用 `message` 字段返回)*
        ```json
        { "status": 404, "message": "Product not found.", "data": null }
        ```
    *   **500 Internal Server Error**: 数据库连接失败或删除异常（此处统一用 `error` 字段返回内部异常）。