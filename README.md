# first_drogon_app

一个基于 `C++20 + Drogon + MySQL + CMake` 的后端练手项目，当前主线是做一个围绕**商品、库存、订单**展开的库存敏感下单系统。

这个项目是边学边做的过程型项目，但目标不是停留在简单 CRUD，而是逐步打造成一个可以写进简历、并且有一定技术含金量的 C++ 后端项目。

## 项目目标

项目最终希望覆盖这些能力：

- 商品管理
- 库存管理
- 订单创建
- 事务处理
- 乐观锁防超卖
- 基础测试
- 日志与监控
- Docker 部署
- 可选 Qt6 管理端

目前项目还在持续完善中，已经完成了一部分商品接口和订单服务雏形。

## 当前进度

### 已完成

- 已搭建 `Drogon + CMake` 后端工程
- 已配置 MySQL 数据库连接
- 已生成 `Product`、`Inventory`、`OrderMainTable`、`OrderDetailTable` 模型
- 已实现商品模块的基础 REST 接口：
  - `GET /products`
  - `GET /products/{id}`
  - `POST /products`
  - `PUT /products/{id}`
  - `DELETE /products/{id}`
- 已实现商品创建和更新时的基础 JSON 校验
- 已编写订单服务雏形，包含：
  - 下单事务
  - 商品价格校验
  - 库存扣减
  - 乐观锁版本校验
  - 失败回滚
- 已建立测试目录和 Drogon 测试入口
- 已创建 Qt6 前端骨架目录 `qt/`

### 进行中

- 将订单服务正式接入 HTTP 接口
- 补齐库存模块接口
- 完善订单查询接口
- 补充测试用例
- 重构文档和工程化配置

### 暂未完成

- 完整的订单 API
- 完整的库存 API
- 自动化测试覆盖
- Docker 部署方案
- CI
- 生产级配置管理
- 完整的 Qt6 管理端

## 技术栈

- `C++20`
- `Drogon`
- `MySQL`
- `CMake`
- `JsonCpp`
- `Drogon ORM`
- `Qt6`（当前仅创建骨架目录，未接入实现）

## 目录结构

```text
first_drogon_app/
├── controllers/         # HTTP 控制器，当前主要是商品接口
├── services/            # 业务逻辑层，当前有订单服务雏形
├── models/              # Drogon ORM 生成的数据模型
├── views/               # Drogon 视图目录
├── test/                # Drogon 测试入口
├── qt/                  # Qt6 前端骨架目录
├── config.json          # 当前使用的主要运行配置
├── config.yaml          # Drogon 默认示例配置
├── CMakeLists.txt       # 主工程构建文件
├── PROJECT_ROADMAP.md   # 项目后续任务与学习路线图
└── main.cc              # 服务入口
```

## 模块职责

当前项目按比较典型的后端分层来组织，虽然还在持续重构中，但目标职责已经比较明确：

- `controllers/`
  - 负责 HTTP 请求接入、路径映射、请求参数解析和响应返回
  - 理想状态下应尽量少直接处理数据库细节，更适合把业务编排下沉到 `services/`
- `services/`
  - 负责业务逻辑和流程编排
  - 当前订单创建相关逻辑主要集中在这一层
- `models/`
  - 由 Drogon ORM 根据数据库表结构生成
  - 用于把数据表映射为 C++ 类
  - 这部分文件不应该手工修改
- `test/`
  - 用于放测试入口和后续测试用例
- `qt/`
  - 预留的 Qt6 前端目录
  - 当前只有项目骨架，尚未接入具体实现

## 当前已有接口

### 商品接口

#### 获取所有商品

```http
GET /products
```

#### 获取单个商品

```http
GET /products/{id}
```

#### 创建商品

```http
POST /products
Content-Type: application/json
```

请求体示例：

```json
{
  "product_name": "keyboard",
  "price": 19900,
  "available_listing": 1,
  "sales_volume": 0
}
```

说明：

- `price` 建议统一按“分”处理
- `available_listing` 表示是否上架

#### 更新商品

```http
PUT /products/{id}
Content-Type: application/json
```

说明：

- 路径参数中的 `id` 为准
- 请求体里的 `product_id` 会被忽略
- 支持部分字段更新

#### 删除商品

```http
DELETE /products/{id}
```

## 当前订单能力说明

目前订单相关逻辑主要在 [`services/order_service.cpp`](/home/chen/first_drogon_app/services/order_service.cpp) 中，已经有一个比较重要的业务雏形：

- 校验商品是否存在
- 校验价格是否匹配
- 使用数据库事务创建订单
- 扣减库存
- 使用 `version` 字段做乐观锁控制
- 失败时回滚事务

不过截至当前版本，订单功能还没有正式暴露成 HTTP 接口，所以 README 中不把它写成“已完成的对外 API”。

如果想了解当前的订单流程设计思路，可以参考：

- [`docs/sequence_diagram.md`](/home/chen/first_drogon_app/docs/sequence_diagram.md)

这份文档更适合被理解为“订单创建目标流程设计图”，而不是当前已经完全交付的 API 行为说明。

## 数据模型

从当前 ORM 模型来看，项目核心表大致包括：

- `product`
- `inventory`
- `order_main_table`
- `order_detail_table`

当前可以先按下面的关系理解：

- `product` 和 `inventory` 基于 `product_id` 形成一对一关系
- `order_main_table` 和 `order_detail_table` 是一对多关系
- `order_detail_table` 通过 `product_id` 关联商品，保存购买时的商品快照信息

目前仓库中还没有单独整理好的建表 SQL 文档，后续会补上。

## 文档索引

为了把项目逐步从“能跑”推进到“更像正式工程”，仓库中已经开始补充一些设计和说明文档：

- [`docs/API_description.md`](/home/chen/first_drogon_app/docs/API_description.md)
  - 当前商品接口说明
- [`docs/mysql_description.md`](/home/chen/first_drogon_app/docs/mysql_description.md)
  - 当前数据表关系说明
- [`docs/configuration_item.md`](/home/chen/first_drogon_app/docs/configuration_item.md)
  - 配置项学习与说明
- [`docs/error_code_table.md`](/home/chen/first_drogon_app/docs/error_code_table.md)
  - 错误码设计草案
- [`docs/sequence_diagram.md`](/home/chen/first_drogon_app/docs/sequence_diagram.md)
  - 订单创建流程设计图
- [`docs/DOCS_REVIEW_REPORT.md`](/home/chen/first_drogon_app/docs/DOCS_REVIEW_REPORT.md)
  - 本轮文档产出的批改报告

说明：

- 这些文档里有一部分属于“当前实现说明”
- 也有一部分属于“下一步目标设计”
- 后续会继续整理，让“现状”和“规划”更清晰地区分开

## 运行环境

建议环境：

- Linux
- `g++` 或 `clang++`，支持 `C++20`
- `cmake >= 3.5`
- 已安装 `Drogon`
- 已安装并运行 `MySQL`

## 配置说明

当前默认使用根目录下的 [`config.json`](/home/chen/first_drogon_app/config.json)。

关键配置包括：

- 监听地址：`0.0.0.0`
- 监听端口：`8080`
- 数据库：`testdb`
- 数据库客户端名：`default`

当前配置文件中直接写了本地数据库账号密码，这只适合学习阶段使用，不适合生产环境。后续会改成更安全的配置方式。

## 构建与运行

### 1. 配置数据库

先确保本地 MySQL 中已经有项目所需数据库和数据表，并与 [`config.json`](/home/chen/first_drogon_app/config.json) 中的配置保持一致。

当前配置示例：

```json
{
  "db_clients": [
    {
      "name": "default",
      "rdbms": "mysql",
      "host": "127.0.0.1",
      "port": 3306,
      "dbname": "testdb",
      "user": "root",
      "passwd": "123456"
    }
  ]
}
```

### 2. 构建项目

如果你还没有创建 `build/` 目录，可以执行：

```bash
cmake -S . -B build
cmake --build build
```

如果已经有 `build/` 目录，也可以在其中直接重新编译：

```bash
cmake --build build
```

### 3. 启动服务

```bash
./build/first_drogon_app
```

如果你想指定配置文件：

```bash
./build/first_drogon_app config.json
```

服务启动后，默认监听：

```text
http://127.0.0.1:8080
```

## 测试

当前项目已经有 Drogon 测试入口，但测试内容还比较少。

构建完成后，可以尝试运行测试二进制：

```bash
./build/test/first_drogon_app_test
```

当前测试还处于起步阶段，后续会重点补充：

- 商品接口测试
- 订单创建成功路径测试
- 库存不足测试
- 金额不一致测试
- 并发扣库存场景测试

## 开发重点

这个项目接下来最重要的方向不是“继续无边界堆接口”，而是把主业务链路做扎实。

优先级大致如下：

1. 完善订单创建主流程
2. 补齐库存模块
3. 建立测试
4. 统一错误处理和日志
5. 增加部署与工程化能力

更完整的任务路线请看：

- [`PROJECT_ROADMAP.md`](/home/chen/first_drogon_app/PROJECT_ROADMAP.md)

## 学习重点

这个项目适合配合这些内容一起学习：

- C++ 对象、引用、生命周期
- STL 容器与常用算法
- CMake 基础
- HTTP 和 REST API 设计
- MySQL 事务、索引、表关系
- Drogon 控制器、ORM、异步处理
- 测试基础
- 日志、监控、部署

## 当前已知问题

由于项目还在学习和迭代阶段，目前存在一些明显还需要继续完善的点：

- `README` 之外的项目文档还不完整
- 数据库建表脚本还没单独整理
- 订单功能还未正式暴露为 HTTP 接口
- 测试覆盖很少
- 配置中存在硬编码账号密码
- 工程化程度还不高

这些问题后续会逐步处理，它们也是项目成长的一部分。

## 后续计划

下一阶段重点会放在：

- 完成订单 API
- 完成库存 API
- 增加测试
- 改善文档
- 做 Docker 部署
- 视进度补 Qt6 管理端

## 说明

这是一个以学习驱动、但努力按工程化标准推进的 C++ 后端项目。

如果后面能把订单、库存、事务一致性、测试、部署这些部分逐步做扎实，它会比单纯的 CRUD 练手项目更有简历价值，也更能体现对后端系统设计的理解。
