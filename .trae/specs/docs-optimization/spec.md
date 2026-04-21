# Docs Optimization Spec

## Why

根据 `docs/DOCS_REVIEW_REPORT.md` 的批改报告，当前 docs 目录下的文档存在以下共性问题：
- 部分内容写成"已经实现"，但实际只是"设计目标"
- 文档与当前代码状态不一致
- 部分表述有概念性误差
- 部分文档需要更工程化的表达

## What Changes

根据批改报告，需要修改以下 7 个文档：

### 1. `docs/return_format.txt`
- **现状**：声称"已将所有代码返回格式统一为上述格式"
- **问题**：与当前代码实际使用的 `status/error/message/data` 不一致
- **修改**：改为"目标统一返回格式规范（设计中）"

### 2. `docs/error_code_table.md`
- **现状**：第60行写"代码已按错误码修改"
- **问题**：当前代码并未完整落地统一错误码返回结构
- **修改**：改为"待落地的错误码设计规范"

### 3. `docs/sequence_diagram.md`
- **现状**：描述为"当前系统时序图"
- **问题**：`order_controller.cc` 中 `create_order()` 尚未实现，仍在 service 层
- **修改**：明确标注为"订单创建流程设计图"（目标规范）

### 4. `docs/directory_description.txt`
- **现状**：描述不够工程化，概念有误差
- **问题**：
  - "不需要再在代码中查询数据库"表述不准确
  - 使用口语化描述如"服务员"
- **修改**：
  - 修正 ORM 模型职责表述
  - 替换为工程化术语（请求接入层/业务编排层/数据映射层）

### 5. `docs/configuration_item.md`
- **现状**：同时使用 `threads_num`/`connection_number` 和 `number_of_threads`/`number_of_connections`
- **问题**：当前项目 `config.json` 实际使用 `number_of_threads` 和 `number_of_connections`
- **修改**：统一使用当前项目实际配置键名

### 6. `docs/API_description.md`
- **现状**：商品接口描述较准确
- **问题**：缺少字段类型说明，未明确金额单位，未明确 `available_listing` 取值语义
- **修改**：补充上述信息，区分"当前实现"和"希望统一的规范"

### 7. `docs/mysql_description.md`
- **现状**：表关系理解到位
- **问题**：未区分 ORM 映射推断与数据库 DDL 事实，未说明索引设计
- **修改**：补充 DDL 事实说明、索引设计说明、金额字段避免浮点的原因

## Impact

- Affected specs: 当前实现文档 vs 设计规范文档的区分意识
- Affected code: 无代码变更，仅文档优化

## ADDED Requirements

### Requirement: 文档状态标注
文档应明确区分"当前实现"和"设计目标"

#### Scenario: 区分现状和目标
- **WHEN** 编写或修改文档时
- **THEN** 必须明确标注哪些是当前真实实现，哪些是设计方案或待落地规范

## MODIFIED Requirements

### Requirement: 配置文档准确性
配置项文档中的键名必须与 `config.json` 实际使用的键名一致

### Requirement: ORM 模型职责准确性
models 目录的 ORM 类提供的是"表结构映射"，不是"自动查询数据库"
