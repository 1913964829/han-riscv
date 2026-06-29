# 06 下一步开发任务

本文档用于记录 `han-riscv` 的近期开发入口。当前目标不是一次性完成完整 C 编译器，而是先完成一个能跑通的最小中文 C 子集编译器。

## 第一目标：最小闭环

输入：

```han
整 主() {
    返 7;
}
```

输出 RISC-V 汇编：

```asm
.globl main
main:
    li a0, 7
    ret
```

验证方式：

1. 编译 `.han` 源码。
2. 生成 `.s` 汇编文件。
3. 使用 RISC-V 工具链汇编链接。
4. 使用 QEMU 或开发板运行。
5. 程序返回值为 `7`。

## 阶段 1：词法分析器

先实现 UTF-8 源码读取和 token 切分。

最小 token：

```text
整 返 标识符 整数字面量
( ) { } ; , + - * / =
```

建议先支持：

```text
关键字：整、返、若、否、当
标识符：中文、英文、下划线、数字组合
数字：十进制整数
注释：// 行注释
```

## 阶段 2：语法分析器

最小语法：

```text
program  := function*
function := type ident '(' params? ')' block
block    := '{' stmt* '}'
stmt     := return_stmt | expr_stmt | var_decl
return_stmt := '返' expr ';'
```

先只解析：

```han
整 主() {
    返 7;
}
```

再扩展到：

```han
整 加(整 甲, 整 乙) {
    返 甲 + 乙;
}
```

## 阶段 3：AST

最小 AST 节点：

```text
程序
函数定义
参数
代码块
返回语句
整数字面量
变量引用
二元表达式
函数调用
```

## 阶段 4：RISC-V 后端

第一版不做复杂寄存器分配。

规则：

```text
a0 保存表达式结果
t0/t1 做临时寄存器
返回值放 a0
函数用 ret 返回
```

先生成 RV64 汇编；如果后续要面向 RV32 MCU，再切换 `lw/sw`、4 字节栈槽和 RV32 ABI。

## 阶段 5：测试样例

最小测试集：

```text
examples/hello.han       返回 7
examples/factorial.han   返回 120
```

之后添加：

```text
变量声明
if/else
while
函数调用
递归
局部变量
数组
指针
结构体
```

## 当前建议

下一步优先写一个非常小的编译器原型：

```text
src/main.cpp
src/lexer.cpp
src/parser.cpp
src/ast.cpp
src/backend/riscv.cpp
```

先不要接 LLVM，也不要做中文 IR。第一轮只完成：

```text
.han -> AST -> RISC-V .s
```

跑通后再把中间 IR 加进去。
