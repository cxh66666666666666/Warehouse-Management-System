# Tasks

- [x] Task 1: 优化 `docs/return_format.txt`
  - [x] Task 1.1: 修正"已将所有代码返回格式统一"为"目标统一返回格式规范（设计中）"
  - [x] Task 1.2: 补充说明当前代码实际使用的返回格式与目标格式的差异

- [x] Task 2: 优化 `docs/error_code_table.md`
  - [x] Task 2.1: 删除"代码已按错误码修改"表述
  - [x] Task 2.2: 改为"待落地的错误码设计规范"或"建议采用的错误码规范"

- [x] Task 3: 优化 `docs/sequence_diagram.md`
  - [x] Task 3.1: 文档标题改为"订单创建流程设计图"
  - [x] Task 3.2: 明确标注为"目标实现时序图"而非"当前系统时序图"
  - [x] Task 3.3: 说明 `create_order()` 尚未在 controller 层实现

- [x] Task 4: 优化 `docs/directory_description.txt`
  - [x] Task 4.1: 修正 models 描述，ORM 类是"表结构映射"不是"自动查询"
  - [x] Task 4.2: 将口语化描述替换为工程化术语（请求接入层/业务编排层/数据映射层）

- [x] Task 5: 优化 `docs/configuration_item.md`
  - [x] Task 5.1: 统一使用 `number_of_threads`（当前项目实际使用的键名）
  - [x] Task 5.2: 统一使用 `number_of_connections`（当前项目实际使用的键名）

- [x] Task 6: 优化 `docs/API_description.md`
  - [x] Task 6.1: 增加字段类型说明
  - [x] Task 6.2: 明确金额单位（如"分"）
  - [x] Task 6.3: 明确 `available_listing` 的取值语义（0=下架，1=上架）
  - [x] Task 6.4: 区分"当前实现"和"希望统一的规范"

- [x] Task 7: 优化 `docs/mysql_description.md`
  - [x] Task 7.1: 区分"从 ORM 映射推断出来的信息"和"数据库 DDL 的最终事实"
  - [x] Task 7.2: 补充索引设计说明
  - [x] Task 7.3: 补充金额字段为什么要避免浮点的原因

# Task Dependencies
- 所有任务已完成，无依赖关系
