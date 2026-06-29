# han-riscv

`han-riscv` 是一个用中文逐步复刻 C-like 系统语言、编译器、IR、汇编、RISC-V 后端和自举流程的工程化实验。

这个项目的目标不是做中文宏替换，也不是把中文源码简单翻译成 C，而是从编译器工程本身出发，逐步建立一套中文表达的系统语言工具链。

## 核心目标

```text
中文 C-like 源码
  -> 词法分析
  -> 语法分析
  -> AST
  -> 语义分析 / 类型检查
  -> 中文 IR / 普通 IR
  -> 优化 Pass
  -> RISC-V 汇编 / 机器码
  -> 运行时 / 标准库
  -> 用中文语言重写编译器
  -> 自举
```

长期目标是：

1. 先抄 C，做中文关键字的 C 子集。
2. 再做自己的编译器前端，不依赖 C 预处理宏。
3. 中端建立自己的 IR，并逐步中文化 IR 表达。
4. 后端优先面向 RISC-V，覆盖函数调用、栈帧、寄存器、链接、运行时。
5. 用这门中文语言重写编译器，完成自举。
6. 最后探索中文汇编、中文字节码、中文指令集表示。

## 不做什么

本项目暂时不追求自然语言编程。

不做：

```text
若甲大于乙则如何如何
```

而是做受控的中文系统语言：

```han
整 加(整 甲, 整 乙) {
    返 甲 + 乙;
}

整 主() {
    返 加(1, 2);
}
```

## 第一阶段语言形态

第一版保留 C 的表达式和块结构，只把关键字、类型名、标准库接口逐步中文化。

| C 概念 | C 关键字 | 中文关键字 |
|---|---|---|
| int | `int` | `整` |
| char | `char` | `字` |
| void | `void` | `空` |
| bool | `bool` | `判` |
| return | `return` | `返` |
| if | `if` | `若` |
| else | `else` | `否` |
| while | `while` | `当` |
| for | `for` | `循` |
| break | `break` | `断` |
| continue | `continue` | `续` |
| struct | `struct` | `构` |
| enum | `enum` | `举` |
| const | `const` | `恒` |
| static | `static` | `静` |
| extern | `extern` | `外` |

## 文档入口

- [项目愿景](docs/00_project_vision.md)
- [语言设计计划](docs/01_language_plan.md)
- [编译器流水线](docs/02_compiler_pipeline.md)
- [RISC-V 后端计划](docs/03_riscv_backend.md)
- [中文 IR、中文汇编与中文字节码](docs/04_chinese_ir_and_asm.md)
- [自举计划](docs/05_bootstrap_plan.md)

## 示例

- [hello.han](examples/hello.han)
- [factorial.han](examples/factorial.han)

## 项目原则

```text
先让语言能跑。
再让编译器正规。
再让后端落到 RISC-V。
再用中文重写自己。
最后再谈中文机器码表达。
```

真正的 CPU 执行的是比特，不是英文也不是中文。所谓“中文机器码”在本项目中先定义为：

1. 中文汇编助记符。
2. 中文 IR 表示。
3. 中文字节码 / 虚拟机指令。
4. 最终映射到真实 RISC-V 机器码。

## 推荐初始目录

```text
han-riscv/
  README.md
  docs/
    00_project_vision.md
    01_language_plan.md
    02_compiler_pipeline.md
    03_riscv_backend.md
    04_chinese_ir_and_asm.md
    05_bootstrap_plan.md
  examples/
    hello.han
    factorial.han
  src/
    lexer/
    parser/
    ast/
    sema/
    ir/
    opt/
    backend/riscv/
  runtime/
  tests/
```

## 当前状态

项目处于规划期。第一步目标是实现最小中文 C 子集：

```han
整 主() {
    返 7;
}
```

并编译到 RISC-V 汇编：

```asm
.globl main
main:
    li a0, 7
    ret
```
