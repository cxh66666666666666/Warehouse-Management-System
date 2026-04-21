# 订单创建流程设计图（目标规范）

> **⚠️ 注意**：此为订单创建流程的**设计规范**，不是当前已完全可运行的系统时序图。
>
> 当前状态：
> - `controllers/order_controller.h` 已创建但未提交
> - `controllers/order_controller.cc` 中 `create_order()` 尚未实现
> - 当前订单能力主要仍在 `service` 层

---

```sequenceDiagram
    autonumber
    actor Client as 客户端
    participant Ctrl as OrderController
    participant Svc as OrderService
    participant DB as 数据库 (MySQL)

    Client->>Ctrl: POST /orders (提交订单 JSON)

    %% 第一阶段：基础校验（无数据库交互）
    rect rgb(240, 248, 255)
        Note over Ctrl: 【校验 1】JSON 格式与必填字段
        alt JSON 无效或缺少必填字段
            Ctrl-->>Client: 400 Bad Request (Fail-fast)
        end
    end

    Ctrl->>Svc: createOrder(CreateOrderRequest)

    %% 开启数据库事务
    Svc->>DB: 开启事务 (newTransactionCoro)
    activate DB

    %% 循环处理每个商品
    loop 遍历请求中的所有商品 (items)
        rect rgb(255, 250, 205)
            Note over Svc: 【校验 2】商品 ID 与数量合法性
            alt ID 或 数量 <= 0
                Svc->>DB: 触发回滚 (rollback)
                Svc-->>Ctrl: 返回 ValidationError (10201)
            end
        end

        %% 查价与比对
        rect rgb(240, 255, 240)
            Svc->>DB: 【查价】SELECT price FROM product
            DB-->>Svc: 返回最新单价 db_price
            Note over Svc: 【校验 3】价格防篡改与存在性校验
            alt 商品不存在 或 db_price != price_at_purchase
                Svc->>DB: 触发回滚 (rollback)
                Svc-->>Ctrl: 返回 ValidationError (20101 / 10202)
            end
        end

        %% 乐观锁扣减库存
        rect rgb(255, 228, 225)
            Note over Svc: 【扣库存】乐观锁防超卖
            Svc->>DB: UPDATE inventory SET ... WHERE version = ? AND quantity >= ?
            DB-->>Svc: 返回受影响行数 (affectedRows)
            alt affectedRows == 0
                Note over Svc: 库存不足或版本冲突
                Svc->>DB: 触发回滚 (rollback)
                Svc-->>Ctrl: 返回 InventoryConflictError (30301)
            end
        end

        %% 金额累加与溢出检查
        Note over Svc: 【校验 4】累加总金额并检查是否溢出
        alt 发生整型溢出
            Svc->>DB: 触发回滚 (rollback)
            Svc-->>Ctrl: 返回 ValidationError (30201)
        end
    end

    %% 总金额一致性核对
    rect rgb(240, 248, 255)
        Note over Svc: 【校验 5】总金额一致性
        alt 累加金额 != total_order_amount
            Svc->>DB: 触发回滚 (rollback)
            Svc-->>Ctrl: 返回 ValidationError (30202)
        end
    end

    %% 写入订单数据
    rect rgb(230, 230, 250)
        Note over Svc: 【写订单】生成订单主表和明细表
        Svc->>DB: INSERT INTO order_main_table (生成 UUID)
        DB-->>Svc: 返回 order_main_table_id

        loop 遍历 items
            Svc->>DB: INSERT INTO order_detail_table
        end
    end

    %% 事务提交
    Svc->>DB: 事务提交 (trans_ptr.reset() 隐式提交)
    deactivate DB

    Svc-->>Ctrl: 返回成功结果 (带 order_number)
    Ctrl-->>Client: 200 OK (订单创建成功)

    %% 全局异常捕获回滚机制
    Note over Svc,DB: 【兜底回滚】任何未捕获的 DB 异常/未知异常，都会在 catch 块中触发 rollback()
```

---

## 关键设计要点

1. **事务性**：整个订单创建流程在一个数据库事务中完成，任何一步失败都会触发回滚
2. **乐观锁**：通过 `version` 字段防止库存超卖
3. **价格防篡改**：下单时记录的价格需与数据库当前价格比对，防止前端篡改
4. **金额一致性**：客户端提交的总金额需与服务器计算结果一致
