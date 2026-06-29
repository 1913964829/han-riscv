# 03 RISC-V 后端计划

## 为什么选 RISC-V

RISC-V 适合作为本项目第一目标后端：

1. 指令集开放，资料清晰。
2. 指令格式规整，适合学习后端。
3. 可以用 QEMU 快速测试。
4. 可以逐步落到 MCU / FPGA / 自研软核。
5. 与本项目“中文体系结构工程化”的长期方向一致。

## 第一目标

第一版只生成最小 RISC-V 汇编：

```asm
.globl main
main:
    li a0, 7
    ret
```

对应源码：

```han
整 主() {
    返 7;
}
```

## 推荐路线

### 阶段 A：栈式表达式后端

先不做复杂寄存器分配。

表达式生成时使用栈或固定临时寄存器：

```text
a0 保存表达式结果
t0/t1 做临时寄存器
局部变量放栈上
```

### 阶段 B：函数调用

实现基本调用约定：

```text
a0-a7：参数和返回值
ra：返回地址
sp：栈指针
s0/fp：帧指针
t0-t6：临时寄存器
```

第一版限制参数不超过 8 个。

### 阶段 C：栈帧

函数进入：

```asm
addi sp, sp, -16
sd ra, 8(sp)
sd s0, 0(sp)
addi s0, sp, 16
```

函数退出：

```asm
ld ra, 8(sp)
ld s0, 0(sp)
addi sp, sp, 16
ret
```

RV32 时改用 `sw/lw` 和 4 字节槽位。

### 阶段 D：控制流

把 `若`、`当` 降成标签和跳转：

```han
若 (甲 > 乙) {
    返 甲;
} 否 {
    返 乙;
}
```

变成：

```asm
    ble a0, a1, .Lelse
    # then
    j .Lend
.Lelse:
    # else
.Lend:
```

### 阶段 E：局部变量

局部变量映射到栈偏移：

```text
变量 甲 -> -8(s0)
变量 乙 -> -16(s0)
```

### 阶段 F：数组、指针、结构体

逐步实现：

```text
指针取值
取地址
数组下标
结构体字段偏移
```

## 第一版暂定目标平台

为了快速验证，先选择：

```text
riscv64-linux-gnu
qemu-riscv64
```

后续增加：

```text
rv32i-none-elf
rv32imac-none-elf
rv32imfc-none-elf
```

这可以和 MCU / IM110GW / PX4 相关经验衔接。

## 中文汇编映射

后期可以增加中文助记符层：

| 中文助记符 | RISC-V 助记符 | 含义 |
|---|---|---|
| 载即 | `li` | load immediate |
| 加 | `add` | add |
| 减 | `sub` | subtract |
| 取 | `lw/ld` | load |
| 存 | `sw/sd` | store |
| 跳 | `j` | jump |
| 调 | `call` | call |
| 返 | `ret` | return |

中文汇编不是最终机器执行格式，而是人可读的指令层表示，最后仍需汇编成真实机器码。
