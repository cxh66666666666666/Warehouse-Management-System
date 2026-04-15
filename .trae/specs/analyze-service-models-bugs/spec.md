# 分析 services 和 models 的 Bug Spec

## Why
通过代码审查发现 order_service.cpp 中存在多处与 models 不匹配的 bug，包括拼写错误、变量名不一致、SQL参数绑定错误等问题，需要记录并修复这些bug以确保订单创建功能的正确性。

## What Changes
- 识别并记录 order_service.cpp 中的5个bug
- 运行 drogon_ctl 命令更新 models（检查是否有数据库schema变更）
- **不修改代码**，仅做分析和记录

## Impact
- 受影响文件：services/order_service.cpp, services/order_service.h
- 受影响模型：OrderMainTable, OrderDetailTable

## ADDED Requirements
### Requirement: Bug 分析与记录
系统 SHALL 识别并记录以下bug：

#### Bug 1: 头文件拼写错误
- **位置**: order_service.cpp 第3行
- **问题**: `#include <drogon/utils/Utiliies.h>` 拼写错误
- **期望**: 应该是 `Utilities.h`（注意 `Utiliies` vs `Utilities`）

#### Bug 2: 变量名不一致 - order_number
- **位置**: order_service.cpp 第31-33行
- **问题**: 
  - 第31行生成 order_number: `int order_number=drogon::utils::getUuid();`
  - 第33行使用 `req.order_number`，但 create_order_request 结构体中没有此字段
- **期望**: 应该使用生成的 `order_number` 变量，而不是 `req.order_number`

#### Bug 3: 变量名大小写不一致
- **位置**: order_service.cpp 第37行
- **问题**: 使用 `transPtr`（P大写），但变量定义是 `trans_ptr`（第18行）
- **期望**: 统一使用 `trans_ptr`

#### Bug 4: SQL参数绑定错误
- **位置**: order_service.cpp 第24-25行
- **问题**: SQL语句有4个`?`占位符，但最后一个参数绑定了 `item.quantity`
- **期望**: 应该绑定 `item.quantity_purchased`（与DTO字段名一致）

#### Bug 5: OrderDetailTable 缺少外键字段
- **位置**: order_service.cpp 第36-40行
- **问题**: 插入 order_detail_table 时没有包含 `order_main_table_id` 外键
- **期望**: 应该先获取插入主表后的自增ID，然后作为外键插入明细表

## MODIFIED Requirements
无

## REMOVED Requirements
无
