#include "order_service.h"
#include <drogon/drogon.h>
#include <drogon/utils/Utilities.h>

using namespace services;

drogon::Task<CreateOrderResult> OrderService::createOrder(const CreateOrderRequest& req)
{
    auto db_client = app().getDbClient("default");

    // 检查数据库客户端是否存在
    if (!db_client)
    {
        co_return CreateOrderResult::failure("Database client not found");
    }

    try
    {
        // 开启事务
        auto trans_ptr = co_await db_client->newTransactionCoro();

        // 扣减库存（乐观锁）
        int64_t total_amount = 0;
        for (auto& item : req.items)
        {
            auto result = co_await trans_ptr->execSqlCoro(
                "UPDATE inventory SET inventory_quantity = inventory_quantity - ?, version = version + 1 "
                "WHERE product_id = ? AND version = ? AND inventory_quantity >= ?",
                item.quantity_purchased, item.product_id, item.version, item.quantity_purchased);
            
            if (result.affectedRows() == 0)
            {
                throw std::runtime_error("库存冲突/超卖");
            }
            
            total_amount += item.price_at_purchase * item.quantity_purchased;
        }

        // 插入主表
        std::string order_number = drogon::utils::getUuid();
        auto order_res = co_await trans_ptr->execSqlCoro(
            "INSERT INTO order_main_table (user_id, order_number, purchase_date, total_order_amount) VALUES (?, ?, ?, ?)",
            req.user_id, order_number, req.purchase_date, total_amount);

        // 获取插入主表后的自增ID
        int64_t order_main_table_id = order_res.insertId();

        // 插入订单明细表
        for (auto& item : req.items)
        {
            co_await trans_ptr->execSqlCoro(
                "INSERT INTO order_detail_table (order_main_table_id, product_id, quantity_purchased, price_at_purchase) VALUES (?, ?, ?, ?)",
                order_main_table_id, item.product_id, item.quantity_purchased, item.price_at_purchase);
        }

        co_await trans_ptr->commitCoro();
        co_return CreateOrderResult::success(order_number);
    }
    catch (const std::exception& e)
    {
        co_return CreateOrderResult::failure();
    }
}
