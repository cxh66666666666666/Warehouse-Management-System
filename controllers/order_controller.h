#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class order_controller : public drogon::HttpController<order_controller>
{
  public:
    METHOD_LIST_BEGIN
    // create_order接口映射，相对路径为"/orders"，HTTP方法为Post
    ADD_METHOD_TO(order_controller::create_order, "/orders", drogon::Post);

    METHOD_LIST_END
    // 创建订单
    drogon::Task<drogon::HttpResponsePtr> create_order(const drogon::HttpRequestPtr& req);
};
