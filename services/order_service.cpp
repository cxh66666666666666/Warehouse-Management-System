#include "order_service.h"
#include <drogon/drogon.h>
#include <drogon/utils/Utilities.h>
#include <limits>
#include <memory>
#include <vector>

using namespace services;

namespace
{
class InventoryConflictError : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

class ValidationError : public std::runtime_error
{
public:
    int code;
    using std::runtime_error::runtime_error;
};
} // namespace

drogon::Task<CreateOrderResult> OrderService::createOrder(const CreateOrderRequest& req)
{
    auto db_client = drogon::app().getDbClient("default");

    if (!db_client)
    {
        co_return CreateOrderResult::failure("Database client not found.",40001);
    }

    std::shared_ptr<drogon::orm::Transaction> trans_ptr;
    try
    {
        trans_ptr = co_await db_client->newTransactionCoro();

        std::vector<int64_t> item_prices;
        item_prices.reserve(req.items.size());

        int64_t total_amount = 0;
        for (size_t i = 0; i < req.items.size(); ++i)
        {
            const auto& item = req.items[i];
            if (item.product_id <= 0 || item.quantity_purchased <= 0)
            {
                throw ValidationError("Invalid product ID or quantity.", 10201);
            }

            auto price_res = co_await trans_ptr->execSqlCoro(
                "SELECT price FROM product WHERE product_id = ?",
                item.product_id);
            if (price_res.size() == 0)
            {
                throw ValidationError("Product does not exist.", 20101);
            }

            const auto db_price = price_res[0]["price"].as<int64_t>();
            if (item.price_at_purchase != 0 && item.price_at_purchase != db_price)
            {
                throw ValidationError("Product price verification failed.", 10202);
            }
            item_prices.push_back(db_price);

            auto result = co_await trans_ptr->execSqlCoro(
                "UPDATE inventory SET inventory_quantity = inventory_quantity - ?, version = version + 1 "
                "WHERE product_id = ? AND version = ? AND inventory_quantity >= ?",
                item.quantity_purchased, item.product_id, item.version, item.quantity_purchased);
            
            if (result.affectedRows() == 0)
            {
                throw InventoryConflictError("Inventory conflict / Oversold.");
            }
            
            const __int128 line_amount = static_cast<__int128>(db_price) *
                                         static_cast<__int128>(item.quantity_purchased);
            if (line_amount > static_cast<__int128>(std::numeric_limits<int64_t>::max()))
            {
                throw ValidationError("Order amount overflow.", 30201);
            }
            const int64_t line_amount_i64 = static_cast<int64_t>(line_amount);
            if (total_amount > std::numeric_limits<int64_t>::max() - line_amount_i64)
            {
                throw ValidationError("Order amount overflow.", 30201);
            }
            total_amount += line_amount_i64;
        }

        if (req.total_order_amount != 0 && req.total_order_amount != total_amount)
        {
            throw ValidationError("The total order amount is inconsistent.", 30202);
        }

        std::string order_number = drogon::utils::getUuid();
        auto order_res = co_await trans_ptr->execSqlCoro(
            "INSERT INTO order_main_table (user_id, order_number, purchase_date, total_order_amount) VALUES (?, ?, ?, ?)",
            req.user_id, order_number, req.purchase_date, total_amount);

        int64_t order_main_table_id = order_res.insertId();

        for (size_t i = 0; i < req.items.size(); ++i)
        {
            const auto& item = req.items[i];
            co_await trans_ptr->execSqlCoro(
                "INSERT INTO order_detail_table (order_main_table_id, product_id, quantity_purchased, price_at_purchase) VALUES (?, ?, ?, ?)",
                order_main_table_id,
                item.product_id,
                item.quantity_purchased,
                item_prices[i]);
        }

        trans_ptr.reset();
        co_return CreateOrderResult::success(order_number);
    }
    catch (const ValidationError& e)
    {
        if (trans_ptr)
        {
            try
            {
                trans_ptr->rollback();
            }
            catch (const std::exception& ex)
            {
                LOG_ERROR << "createOrder rollback error: " << ex.what();
            }
            trans_ptr.reset();
        }
        LOG_ERROR << "createOrder validation error: " << e.what();
        co_return CreateOrderResult::failure(e.what(), 400);
    }
    catch (const InventoryConflictError& e)
    {
        if (trans_ptr)
        {
            try
            {
                trans_ptr->rollback();
            }
            catch (const std::exception& ex)
            {
                LOG_ERROR << "createOrder rollback error: " << ex.what();
            }
            trans_ptr.reset();
        }
        LOG_ERROR << "createOrder inventory conflict: " << e.what();
        co_return CreateOrderResult::failure(e.what(), 30301);
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        if (trans_ptr)
        {
            try
            {
                trans_ptr->rollback();
            }
            catch (const std::exception& ex)
            {
                LOG_ERROR << "createOrder rollback error: " << ex.what();
            }
            trans_ptr.reset();
        }
        LOG_ERROR << "createOrder db error: " << e.base().what();
        co_return CreateOrderResult::failure("Database error.", 40002);
    }
    catch (const std::exception& e)
    {
        if (trans_ptr)
        {
            try
            {
                trans_ptr->rollback();
            }
            catch (const std::exception& ex)
            {
                LOG_ERROR << "createOrder rollback error: " << ex.what();
            }
            trans_ptr.reset();
        }
        LOG_ERROR << "createOrder unknown error: " << e.what();
        co_return CreateOrderResult::failure("Order failed. Please try again.", 40099);
    }
}
