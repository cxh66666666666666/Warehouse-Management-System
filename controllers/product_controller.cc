#include "product_controller.h"
#include "models/Product.h"

using namespace drogon;
using namespace drogon_model::testdb;

// 获取所有商品列表接口实现
void product_controller::get_all_products(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) const
{
    auto db_client=app().getDbClient("default");

    // 检查数据库客户端是否存在
    if(!db_client)
    {
        Json::Value ret;
        ret["code"]=40001;
        ret["message"]="Database client not found. Please check database configuration.";
        ret["data"] = Json::nullValue;
        auto resp=HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(HttpStatusCode::k500InternalServerError);
        callback(resp);
        return;
    }
    
    drogon::orm::Mapper<Product> mapper(db_client);

    // 开始异步查询
    mapper.findAll
    (
        // 成功信封
        [callback](const std::vector<Product> &products)
        {
            Json::Value ret;
            ret["code"]=200;
            ret["message"]="Success";
            Json::Value product_array(Json::arrayValue);

            // 取出查询到的数据
            for(const auto& product : products)
            {
                product_array.append(product.toJson());
            }

            ret["data"]=product_array;
            auto resp=HttpResponse::newHttpJsonResponse(ret);
            callback(resp);
        },
        
        // 失败信封
        [callback](const drogon::orm::DrogonDbException& e)
        {
            Json::Value ret;
            ret["code"]=40002;
            ret["message"]=e.base().what();
            ret["data"] = Json::nullValue;
            auto resp=HttpResponse::newHttpJsonResponse(ret);
            resp->setStatusCode(HttpStatusCode::k500InternalServerError);
            callback(resp);
        }
    );

    
}

// 获取单件商品接口实现
void product_controller::get_product_by_id(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback,int product_id) const
{
    auto db_client=app().getDbClient("default");

    // 检查数据库客户端是否存在
    if(!db_client)
    {
        Json::Value ret;
        ret["code"]=40001;
        ret["message"]="Database client not found. Please check database configuration.";
        ret["data"] = Json::nullValue;
        auto resp=HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(HttpStatusCode::k500InternalServerError);
        callback(resp);
        return;
    }

    drogon::orm::Mapper<Product> mapper(db_client);

    // 开始异步查询
    mapper.findByPrimaryKey
    (
        product_id,

        // 成功信封
        [callback](const Product &product)
        {
            Json::Value ret;
            ret["code"]=200;
            ret["message"]="Success";
            ret["data"]=product.toJson();
            auto resp=HttpResponse::newHttpJsonResponse(ret);
            callback(resp);
        },

        // 失败信封
        [callback](const drogon::orm::DrogonDbException& e)
        {
            Json::Value ret;
            HttpResponsePtr resp;

            if (const auto* notFound =
                dynamic_cast<const drogon::orm::UnexpectedRows*>(&e.base()))
            {
                ret["code"] = 20101;
                ret["message"] = "Product not found.";
                ret["data"] = Json::nullValue;
                resp = HttpResponse::newHttpJsonResponse(ret);
                resp->setStatusCode(HttpStatusCode::k404NotFound);
            } 
            else
            {
                LOG_ERROR << "get_product_by_id db error: " << e.base().what();
                ret["code"] = 40002;
                ret["message"] = e.base().what();
                ret["data"] = Json::nullValue;
                resp = HttpResponse::newHttpJsonResponse(ret);
                resp->setStatusCode(HttpStatusCode::k500InternalServerError);
            }
            callback(resp);
        }
    );
}

// 录入新商品接口实现
void product_controller::create_product(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) const
{
    auto db_client=app().getDbClient("default");

    // 检查数据库客户端是否存在
    if(!db_client)
    {
        Json::Value ret;
        ret["code"]=40001;
        ret["message"]="Database client not found. Please check database configuration.";
        ret["data"] = Json::nullValue;
        auto resp=HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(HttpStatusCode::k500InternalServerError);
        callback(resp);
        return;
    }

    // 获取前端传来的Json智能指针
    auto json_ptr=req->getJsonObject();

    // 判断指针是否为空
    if(!json_ptr)
    {
        Json::Value ret;
        ret["code"]=10001;
        ret["message"]="No JSON data provided.";
        ret["data"] = Json::nullValue;
        auto resp=HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(HttpStatusCode::k400BadRequest);
        callback(resp);
        return;
    }

    // 检查格式
    std::string validation_error;
    if(!Product::validateJsonForCreation(*json_ptr, validation_error))
    {
        Json::Value ret;
        ret["code"]=10101;
        ret["message"]=validation_error;
        ret["data"] = Json::nullValue;
        auto resp=HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(HttpStatusCode::k400BadRequest);
        callback(resp);
        return;
    }

    Product new_product;
    try
    {
        new_product = Product(*json_ptr);
    }
    catch (const std::exception& e)
    {
        Json::Value ret;
        ret["code"]=10002;
        ret["message"]=e.what();
        ret["data"] = Json::nullValue;
        auto resp=HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(HttpStatusCode::k400BadRequest);
        callback(resp);
        return;
    }
    
    drogon::orm::Mapper<Product> mapper(db_client);

    mapper.insert
    (
        new_product,

        // 成功信封
        [callback](const Product &inserted_product)
        {
            Json::Value ret;
            ret["code"]=201;
            ret["message"]="Created";
            ret["data"]=inserted_product.toJson();
            auto resp=HttpResponse::newHttpJsonResponse(ret);
            resp->setStatusCode(HttpStatusCode::k201Created);
            callback(resp);
        },

        // 失败信封
        [callback](const drogon::orm::DrogonDbException& e)
        {
            Json::Value ret;
            ret["code"]=40002;
            ret["message"]=e.base().what();
            ret["data"] = Json::nullValue;
            auto resp=HttpResponse::newHttpJsonResponse(ret);
            resp->setStatusCode(HttpStatusCode::k500InternalServerError);
            callback(resp);
        }
    );
}

// 更新商品信息接口实现
void product_controller::update_product(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback,int product_id) const
{
    auto db_client=app().getDbClient("default");

    // 检查数据库客户端是否存在
    if(!db_client)
    {
        Json::Value ret;
        ret["code"]=40001;
        ret["message"]="Database client not found. Please check database configuration.";
        ret["data"] = Json::nullValue;
        auto resp=HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(HttpStatusCode::k500InternalServerError);
        callback(resp);
        return;
    }

    // 获取前端传来的指针
    auto json_ptr=req->getJsonObject();

    // 判断指针是否为空
    if(!json_ptr)
    {
        Json::Value ret;
        ret["code"]=10001;
        ret["message"]="No JSON data provided.";
        ret["data"] = Json::nullValue;
        auto resp=HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(HttpStatusCode::k400BadRequest);
        callback(resp);
        return;
    }

    // 只信任路径参数中的主键；请求体里的 product_id 会被忽略
    auto update_json = std::make_shared<Json::Value>(*json_ptr);
    update_json->removeMember("product_id");

    // 检查格式
    std::string validation_error;
    size_t update_field_count = 0;

    if(update_json->isMember("product_name"))
    {
        ++update_field_count;
        if(!Product::validJsonOfField(1, "product_name", (*update_json)["product_name"], validation_error, false))
        {
            Json::Value ret;
            ret["code"]=10101;
            ret["message"]=validation_error;
            ret["data"] = Json::nullValue;
            auto resp=HttpResponse::newHttpJsonResponse(ret);
            resp->setStatusCode(HttpStatusCode::k400BadRequest);
            callback(resp);
            return;
        }
    }

    if(update_json->isMember("price"))
    {
        ++update_field_count;
        if(!Product::validJsonOfField(2, "price", (*update_json)["price"], validation_error, false))
        {
            Json::Value ret;
            ret["code"]=10101;
            ret["message"]=validation_error;
            ret["data"] = Json::nullValue;
            auto resp=HttpResponse::newHttpJsonResponse(ret);
            resp->setStatusCode(HttpStatusCode::k400BadRequest);
            callback(resp);
            return;
        }
    }

    if(update_json->isMember("available_listing"))
    {
        ++update_field_count;
        if(!Product::validJsonOfField(3, "available_listing", (*update_json)["available_listing"], validation_error, false))
        {
            Json::Value ret;
            ret["code"]=10101;
            ret["message"]=validation_error;
            ret["data"] = Json::nullValue;
            auto resp=HttpResponse::newHttpJsonResponse(ret);
            resp->setStatusCode(HttpStatusCode::k400BadRequest);
            callback(resp);
            return;
        }
    }

    if(update_json->isMember("sales_volume"))
    {
        ++update_field_count;
        if(!Product::validJsonOfField(4, "sales_volume", (*update_json)["sales_volume"], validation_error, false))
        {
            Json::Value ret;
            ret["code"]=10101;
            ret["message"]=validation_error;
            ret["data"] = Json::nullValue;
            auto resp=HttpResponse::newHttpJsonResponse(ret);
            resp->setStatusCode(HttpStatusCode::k400BadRequest);
            callback(resp);
            return;
        }
    }

    if(update_field_count == 0)
    {
        Json::Value ret;
        ret["code"]=10001;
        ret["message"]="No updatable fields provided.";
        ret["data"] = Json::nullValue;
        auto resp=HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(HttpStatusCode::k400BadRequest);
        callback(resp);
        return;
    }

    drogon::orm::Mapper<Product> mapper(db_client);

    // 开始异步查询
    mapper.findByPrimaryKey
    (
        product_id,

        // 成功信封
        [callback, update_json, db_client](Product product)
        {
            // 更新数据
            try
            {
                product.updateByJson(*update_json);
            }
            catch(const std::exception& e)
            {
                Json::Value ret;
                ret["code"]=10002;
                ret["message"]=e.what();
                ret["data"] = Json::nullValue;
                auto resp=HttpResponse::newHttpJsonResponse(ret);
                resp->setStatusCode(HttpStatusCode::k400BadRequest);
                callback(resp);
                return;
            }

            // 写回数据库
            drogon::orm::Mapper<Product> update_mapper(db_client);
            update_mapper.update
            (
                product,

                // 成功信封
                [callback, db_client, product](size_t rows)
                {
                    if (rows == 0)
                    {
                        Json::Value ret;
                        ret["code"] = 20101;
                        ret["message"] = "Update failed, product not found";
                        ret["data"] = Json::nullValue;
                        auto resp = HttpResponse::newHttpJsonResponse(ret);
                        resp->setStatusCode(HttpStatusCode::k404NotFound);
                        callback(resp);
                        return;
                    }

                    // 获取更新后的数据
                    Json::Value ret;
                    ret["code"] = 200;
                    ret["message"] = "Success";
                    ret["data"] = product.toJson();
                    auto resp = HttpResponse::newHttpJsonResponse(ret);
                    callback(resp);
                },

                // 失败信封
                [callback](const drogon::orm::DrogonDbException& e)
                {
                    Json::Value ret;
                    ret["code"]=40002;
                    ret["message"]=e.base().what();
                    ret["data"] = Json::nullValue;
                    auto resp=HttpResponse::newHttpJsonResponse(ret);
                    resp->setStatusCode(HttpStatusCode::k500InternalServerError);
                    callback(resp);
                }
            );
        },

        // 失败信封
        [callback](const drogon::orm::DrogonDbException& e)
        {
            Json::Value ret;
            HttpResponsePtr resp;

            if (const auto* notFound =
                dynamic_cast<const drogon::orm::UnexpectedRows*>(&e.base()))
            {
                ret["code"] = 20101;
                ret["message"] = "Product not found.";
                ret["data"] = Json::nullValue;
                resp = HttpResponse::newHttpJsonResponse(ret);
                resp->setStatusCode(HttpStatusCode::k404NotFound);
            } 
            else
            {
                LOG_ERROR << "update_product db error: " << e.base().what();
                ret["code"] = 40002;
                ret["message"] = e.base().what();
                ret["data"] = Json::nullValue;
                resp = HttpResponse::newHttpJsonResponse(ret);
                resp->setStatusCode(HttpStatusCode::k500InternalServerError);
            }
            callback(resp);
        }
    );
}

// 删除商品接口实现
void product_controller::delete_product(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback,int product_id) const
{
    auto db_client=app().getDbClient("default");

    // 检查数据库客户端是否存在
    if(!db_client)
    {
        Json::Value ret;
        ret["code"]=40001;
        ret["message"]="Database client not found. Please check database configuration.";
        ret["data"] = Json::nullValue;
        auto resp=HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(HttpStatusCode::k500InternalServerError);
        callback(resp);
        return;
    }

    drogon::orm::Mapper<Product> mapper(db_client);

    // 开始异步查询
    mapper.deleteByPrimaryKey
    (
        product_id,

        // 成功信封
        [callback](size_t rows)
        {
            Json::Value ret;
            if(rows==0)
            {
                ret["code"]=20101;
                ret["message"]="Product not found.";
                ret["data"]=Json::nullValue;
                auto resp=HttpResponse::newHttpJsonResponse(ret);
                resp->setStatusCode(HttpStatusCode::k404NotFound);
                callback(resp);
            }
            else
            {
                ret["code"]=200;
                ret["message"]="Delete success.";
                ret["data"] = Json::nullValue;
                auto resp=HttpResponse::newHttpJsonResponse(ret);
                callback(resp);
            }
        },

        // 失败信封
        [callback](const drogon::orm::DrogonDbException& e)
        {
            Json::Value ret;
            HttpResponsePtr resp;

            ret["code"] = 40002;
            ret["message"] = e.base().what();
            ret["data"] = Json::nullValue;
            resp = HttpResponse::newHttpJsonResponse(ret);
            resp->setStatusCode(HttpStatusCode::k500InternalServerError);
            callback(resp);
        }
    );
}
