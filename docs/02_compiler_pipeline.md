# 02 编译器流水线

## 总体流水线

```text
.han 源码
  -> Lexer 词法分析
  -> Parser 语法分析
  -> AST 抽象语法树
  -> Sema 语义分析 / 类型检查
  -> IR 中间表示
  -> Opt 优化 Pass
  -> RISC-V Codegen
  -> 汇编 / 目标文件 / ELF
```

## 1. Lexer

输入：

```han
整 主() {
    返 0;
}
```

输出：

```text
关键字(整)
标识符(主)
左括号
右括号
左花括号
关键字(返)
整数(0)
分号
右花括号
EOF
```

Lexer 需要处理：

```text
UTF-8 中文关键字
中文标识符
英文标识符
数字
字符串
字符
注释
符号
```

第一版可以先不做复杂 Unicode 分类，简单按 UTF-8 字符流处理：

```text
中文、英文、下划线开头 -> 标识符或关键字
数字 -> 整数字面量
双引号 -> 字符串
其他 -> 运算符或分隔符
```

## 2. Parser

第一版使用递归下降 Parser。

表达式解析建议使用 Pratt Parser 或按优先级拆函数：

```text
parse_assignment
parse_logical_or
parse_logical_and
parse_equality
parse_comparison
parse_add
parse_mul
parse_unary
parse_primary
```

## 3. AST

第一版 AST 节点：

```text
Program
FunctionDecl
ParamDecl
BlockStmt
VarDecl
ReturnStmt
IfStmt
WhileStmt
ExprStmt
BinaryExpr
UnaryExpr
CallExpr
VarExpr
IntLiteral
StringLiteral
```

## 4. Sema

语义分析做：

```text
符号表
变量重复定义检查
变量未定义检查
函数声明检查
函数参数数量检查
return 类型检查
条件表达式类型检查
break / continue 位置检查
```

## 5. IR

第一版 IR 可以先使用三地址码，不必立刻 SSA。

例如：

```han
整 加(整 甲, 整 乙) {
    返 甲 + 乙;
}
```

生成：

```text
函 @加(甲: 整, 乙: 整) -> 整 {
入口:
    临0 = 加 甲, 乙
    返 临0
}
```

内部也可以用英文结构体名实现，但调试打印使用中文 IR。

## 6. 优化 Pass

第一批优化只做最基础的：

```text
常量折叠
死代码删除
简单代数化简
不可达块删除
```

例如：

```han
返 1 + 2 * 3;
```

优化成：

```text
返 7;
```

## 7. 后端

第一版后端优先生成 RISC-V 汇编，目标是 RV32I 或 RV64I 的最小子集。

建议第一版先选 RV64 Linux 用户态，便于在 QEMU 中运行。后续再做 RV32I / MCU 裸机。
