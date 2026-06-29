# src

编译器源码目录，后续建议按模块拆分：

```text
lexer/          词法分析
parser/         语法分析
ast/            抽象语法树
sema/           语义分析和类型检查
ir/             中间表示
opt/            优化 Pass
backend/riscv/  RISC-V 后端
```

第一版可以先集中写一个最小编译器，跑通后再拆分。
