# 重构 Order Service 代码质量 Spec

## Why
当前 order_service.h 中的结构体定义存在代码质量问题：
1. 结构体命名不规范（使用下划线命名而非驼峰命名）
2. 缺少构造函数，无法方便地创建对象
3. 缺少默认值，可能导致未初始化的问题
4. 结构体之间缺少关联性，逻辑混乱
5. 缺少注释说明，可读性差

这些问题导致代码难以维护、容易出错，且不符合现代 C++ 的最佳实践。

## What Changes
- 重构 DTO 结构体，提升代码质量和可维护性
- 统一命名规范（驼峰命名法）
- 添加构造函数和默认值
- 改进结构体之间的关联关系
- 添加清晰的注释
- **不修改**业务逻辑，只改进代码结构

## Impact
- 受影响文件：services/order_service.h
- 可能影响：services/order_service.cpp（需要适配新的结构体名称）

## ADDED Requirements
### Requirement: DTO 结构体重构
The system SHALL 提供高质量的 DTO 结构体定义：

#### Scenario: 命名规范
- **WHEN** 定义结构体
- **THEN** 使用驼峰命名法（如 OrderItemDto 而非 order_item_dto）

#### Scenario: 构造函数
- **WHEN** 创建 DTO 对象
- **THEN** 提供便捷的构造函数

#### Scenario: 默认值
- **WHEN** 定义成员变量
- **THEN** 提供合理的默认值

#### Scenario: 嵌套关系
- **WHEN** 定义请求 DTO
- **THEN** 清晰地包含子 DTO

## MODIFIED Requirements
无

## REMOVED Requirements
无
