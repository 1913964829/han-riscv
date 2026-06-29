# 01 语言设计计划

## 第一阶段：中文 C 子集

第一版语言名暂定为“汉 C”，文件后缀暂定为 `.han`。

第一版不追求自然语言，而是使用 C-like 结构：

```han
整 加(整 甲, 整 乙) {
    返 甲 + 乙;
}

整 主() {
    整 答 = 加(1, 2);
    返 答;
}
```

## 关键字表

| 功能 | 中文关键字 | 对应 C 概念 |
|---|---|---|
| 整型 | `整` | `int` / `i32` |
| 字符 | `字` | `char` / `i8` |
| 空类型 | `空` | `void` |
| 布尔 | `判` | `bool` |
| 返回 | `返` | `return` |
| 如果 | `若` | `if` |
| 否则 | `否` | `else` |
| 循环 | `当` | `while` |
| 计数循环 | `循` | `for` |
| 跳出 | `断` | `break` |
| 继续 | `续` | `continue` |
| 结构体 | `构` | `struct` |
| 枚举 | `举` | `enum` |
| 常量 | `恒` | `const` |
| 静态 | `静` | `static` |
| 外部 | `外` | `extern` |
| 大小 | `量` | `sizeof` |

## 第一版语法草案

```text
program        := function*

function       := type identifier "(" params? ")" block

params         := param ("," param)*
param          := type identifier

type           := "整" | "字" | "空" | "判" | pointer_type | identifier
pointer_type   := type "*"

block          := "{" statement* "}"

statement      := var_decl
                | return_stmt
                | if_stmt
                | while_stmt
                | expr_stmt
                | block

var_decl       := type identifier ("=" expr)? ";"

return_stmt    := "返" expr? ";"

if_stmt        := "若" "(" expr ")" block ("否" block)?

while_stmt     := "当" "(" expr ")" block

expr_stmt      := expr ";"
```

## 表达式优先级

第一版表达式保持 C 风格：

```text
()
函数调用
一元运算：! - * &
乘除模：* / %
加减：+ -
比较：< <= > >=
相等：== !=
逻辑与：&&
逻辑或：||
赋值：=
```

## 第一阶段必须支持的程序

### 返回常数

```han
整 主() {
    返 7;
}
```

### 函数调用

```han
整 加(整 甲, 整 乙) {
    返 甲 + 乙;
}

整 主() {
    返 加(1, 2);
}
```

### 条件

```han
整 最大(整 甲, 整 乙) {
    若 (甲 > 乙) {
        返 甲;
    } 否 {
        返 乙;
    }
}
```

### 循环

```han
整 求和(整 数) {
    整 和 = 0;
    整 i = 0;

    当 (i <= 数) {
        和 = 和 + i;
        i = i + 1;
    }

    返 和;
}
```

## 第二阶段补充

第二阶段加入：

```text
指针
数组
结构体
字符串
文件 IO
动态内存
命令行参数
```

这些是后续用本语言写编译器本身的基础。
