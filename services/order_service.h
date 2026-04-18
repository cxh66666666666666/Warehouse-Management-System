#pragma once

#include <drogon/drogon.h>
#include <cstdint>
#include <string>
#include <vector>
#include <optional>

namespace services {

/**
 * @brief 订单项数据传输对象
 * 
 * 表示订单中的一个商品项，包含商品ID、购买数量、购买时价格和版本号
 */
struct OrderItemDto
{
    // 商品ID
    int product_id{0};
    
    // 购买数量
    int quantity_purchased{0};
    
    // 购买时的价格（单位：分）
    int64_t price_at_purchase{0};
    
    // 库存版本号（用于乐观锁）
    int version{0};

    // 默认构造函数
    OrderItemDto() = default;

    // 便捷构造函数
    OrderItemDto(int product_id, int quantity, int64_t price, int ver)
        : product_id(product_id)
        , quantity_purchased(quantity)
        , price_at_purchase(price)
        , version(ver)
    {}
};

// 订单项列表类型别名
using OrderItems = std::vector<OrderItemDto>;

/**
 * @brief 创建订单请求数据传输对象
 * 
 * 包含创建订单所需的所有信息：订单项列表、购买日期、用户ID和订单总金额
 */
struct CreateOrderRequest
{
    // 订单项列表
    OrderItems items;
    
    // 购买日期（格式：YYYY-MM-DD）
    std::string purchase_date;
    
    // 用户ID
    int user_id{0};
    
    // 订单总金额（单位：分）
    int64_t total_order_amount{0};

    // 默认构造函数
    CreateOrderRequest() = default;

    // 便捷构造函数
    CreateOrderRequest(OrderItems items, std::string date, int uid, int64_t amount)
        : items(std::move(items))
        , purchase_date(std::move(date))
        , user_id(uid)
        , total_order_amount(amount)
    {}
};

/**
 * @brief 创建订单结果数据传输对象
 * 
 * 包含订单创建的结果：是否成功、订单号和消息
 */
struct CreateOrderResult
{
    // HTTP/业务状态码
    int code{500};
    
    // 结果消息
    std::string message;

    // 负载数据
    std::string data;

    // 默认构造函数
    CreateOrderResult() = default;

    // 便捷构造函数
    CreateOrderResult(int c, std::string msg, std::string d = "")
        : code(c)
        , message(std::move(msg))
        , data(std::move(d))
    {}

    // 静态工厂方法：创建成功结果
    static CreateOrderResult success(std::string order_num, std::string msg = "Order placed successfully.")
    {
        return CreateOrderResult(200, std::move(msg), std::move(order_num));
    }

    // 静态工厂方法：创建失败结果
    static CreateOrderResult failure(std::string msg = "Order failed. Please try again.", int c = 500)
    {
        return CreateOrderResult(c,  std::move(msg), "");
    }
};

/**
 * @brief 订单服务类
 * 
 * 提供订单相关的业务逻辑，包括创建订单功能
 */
class OrderService
{
public:
    /**
     * @brief 创建订单
     * 
     * @param req 创建订单请求
     * @return 异步返回创建订单结果
     */
    static drogon::Task<CreateOrderResult> createOrder(const CreateOrderRequest& req);
};

} // namespace services
