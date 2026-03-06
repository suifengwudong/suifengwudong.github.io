## Copilot 指南

### 1. Commit 风格

采用 [Conventional Commits](https://www.conventionalcommits.org/) 规范，提交信息格式如下：

```<type>[optional scope]: <description>
```

其中：
- `type`：必选，表示提交的类型，如 `feat`（新功能）、`fix`（修复 bug）、`docs`（文档更新）等。
- `scope`：可选，表示提交的范围，如模块或功能。
- `description`：必选，简要描述提交的内容。

### 2. 代码风格
- 使用 Prettier 进行代码格式化，确保代码风格一致。
- 使用 ESLint 进行代码质量检查，遵循 Airbnb JavaScript 规范。