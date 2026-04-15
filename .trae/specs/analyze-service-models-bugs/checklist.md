# Checklist

## Bug 分析检查项

- [x] Bug 1 已确认 - order_service.cpp 第3行头文件拼写错误 `Utiliies.h`
- [x] Bug 2 已确认 - order_service.cpp 第33行使用不存在的 `req.order_number`
- [x] Bug 3 已确认 - order_service.cpp 第37行变量名 `transPtr` 应为 `trans_ptr`
- [x] Bug 4 已确认 - order_service.cpp 第25行 `item.quantity` 应为 `item.quantity_purchased`
- [x] Bug 5 已确认 - order_service.cpp 插入明细表时缺少 `order_main_table_id` 外键

## Models 更新检查项

- [x] drogon_ctl create model 命令执行成功
- [x] OrderMainTable.h 已检查
- [x] OrderMainTable.cc 已检查
- [x] OrderDetailTable.h 已检查
- [x] OrderDetailTable.cc 已检查
- [x] 模型文件与数据库 schema 一致

## 新增 Schema 变更项

- [x] OrderMainTable 新增 user_id 字段已确认
