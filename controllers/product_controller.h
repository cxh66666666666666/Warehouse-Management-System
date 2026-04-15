#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class product_controller : public drogon::HttpController<product_controller>
{
  public:
    METHOD_LIST_BEGIN
    // get_all_products接口映射，相对路径"/products"，HTTP方法为Get
    ADD_METHOD_TO(product_controller::get_all_products, "/products", Get);
    // get_product_by_id接口映射，相对路径"/products/{1}"，HTTP方法为Get
    ADD_METHOD_TO(product_controller::get_product_by_id, "/products/{1}", Get);
    // creat_product接口映射，相对路径"/products"，HTTP方法为Post
    ADD_METHOD_TO(product_controller::create_product, "/products", Post);
    // update_product接口映射，相对路径"/products/{1}"，HTTP方法为Put
    ADD_METHOD_TO(product_controller::update_product, "/products/{1}", Put);
    // delete_product接口映射，相对路径"/products/{1}"，HTTP方法为Delete
    ADD_METHOD_TO(product_controller::delete_product, "/products/{1}", Delete);


    METHOD_LIST_END
    // 获取所有商品列表
    void get_all_products(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) const;
    // 获取单件商品
    void get_product_by_id(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback,int product_id) const;
    // 录入新商品
    void create_product(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) const;
    // 更新商品信息
    void update_product(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback,int product_id) const;
    // 删除商品信息
    void delete_product(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback,int product_id) const;
};
