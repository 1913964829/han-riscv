# 07 第一版 hancc 原型

本阶段开始从规划进入代码实现。

当前新增了一个极小的 C++17 编译器原型：

```text
src/hancc.cpp
```

它的目标不是一次性完整实现 C，而是先跑通最小链路：

```text
.han 源码
  -> Lexer
  -> Parser
  -> AST
  -> RISC-V 汇编
```

## 当前支持的源码

当前最小支持：

```han
整 主() {
    返 7;
}
```

也支持返回表达式：

```han
整 主() {
    返 1 + 2 * 3;
}
```

## 当前支持的语法

```text
program  := function*
function := '整' ident '(' ')' '{' '返' expr ';' '}'
expr     := additive
additive := multiplicative (('+' | '-') multiplicative)*
multiplicative := primary (('*' | '/') primary)*
primary  := number | '(' expr ')'
```

## 当前支持的 token

```text
关键字：整、返
标识符：中文、英文、下划线、数字组合
数字：十进制整数
符号：( ) { } ; + - * /
注释：// 行注释
```

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 使用

输出到标准输出：

```bash
./build/hancc examples/hello.han
```

输出到文件：

```bash
./build/hancc examples/hello.han -o hello.s
```

## 当前输出示例

输入：

```han
整 主() {
    返 7;
}
```

输出：

```asm
    .text
    .globl main
main:
    addi sp, sp, -16
    sd ra, 8(sp)
    sd s0, 0(sp)
    addi s0, sp, 16
    li a0, 7
    ld ra, 8(sp)
    ld s0, 0(sp)
    addi sp, sp, 16
    ret
```

## 当前限制

当前原型还不支持：

```text
函数参数
局部变量
if/else
while
比较运算
函数调用
递归
数组
指针
结构体
RV32 汇编输出
```

所以 `examples/factorial.han` 暂时是目标示例，还不能被当前 hancc 编译。

## 下一步

推荐开发顺序：

1. 支持函数参数。
2. 支持变量声明：`整 甲 = 1;`。
3. 支持变量引用和赋值。
4. 支持比较运算：`<=`、`<`、`==`。
5. 支持 `若/否`。
6. 支持函数调用。
7. 让 `examples/factorial.han` 真正通过。
8. 增加 RV32 输出选项。
