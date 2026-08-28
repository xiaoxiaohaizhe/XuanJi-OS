# XuanJi-OS

一个从零搭建的 x86 微系统，专注于内核基础组件，学习操作系统原理的实践项目。

## 项目状态

| 模块 | 状态 |
|------|------|
| 引导程序 (Multiboot) | ✅ 完成 |
| 屏幕输出 (VGA 文本模式) | ✅ 完成 |
| 格式化输出 (printf) | ✅ 完成 |
| GDT (全局描述符表) | ✅ 完成 |
| IDT (中断描述符表) | ✅ 完成 |
| 内存管理 | ⏳ 待开发 |
| 进程调度 | ⏳ 待开发 |
......

---
## 版本号规则

## 版本号规则

版本号格式：vX.Y.Z(D)

| 段 | 含义 |
|----|------|
| X | 大版本：提交次数+1 ÷ 100 + 1 |
| Y | 小版本：提交次数+1 % 100(补零到两位) |
| Z | 补丁版本：修复 Bug +1(该大版本内累计)|
|(D)| 提交日期 (YYYY-MM-DD) |
**示例：**
- 第 1 次提交 → v1.01.0(2026-xx-xx)
- 第 21 次提交 → v1.21.0(2026-xx-xx)
- 第 100 次提交 → v2.00.0(xxxx-xx-x)

当前版本：**v1.23.0**（2026-08-28）
### 开发者说明

- 提交次数：`git rev-list --count HEAD`
- 版本号写在 `kernel/src/shell.cpp` 的 `version` 函数中 格式vX.Y.Z(YYYY-MM-DD)
- 每次提交后，根据提交次数和补丁情况手动更新版本号
- 每次提交后，根据提交次数手动更新 vX.Y(YYYY-MM-DD)，修复 Bug 时更新 Z
- **大版本升级（X+1）时且Y = 00 时，Z 才清零**
### 前置依赖 (Ubuntu / WSL)

```bash
sudo apt update
sudo apt install g++ nasm grub-pc-bin grub-common xorriso qemu-system-x86 -y
```

### 编译与运行

```bash
git clone https://github.com/xiaoxiaohaizhe/XuanJi-OS.git
cd ~/XuanJi-OS
make
make run
```

---

## 项目结构

```
XuanJi-OS/
├── boot/
│   └── multiboot/
│       └── boot.asm          # 引导程序
├── kernel/
│   ├── include/
│   │   └── print.h  # printf 头文件
│   │   └── idt.h
│   │   └── isr.h
│   │   └── memory.h
│   │   └── gdt.h
│   │   └── shell.h
│   └── src/
│       ├── main.cpp          # 内核入口 kmain()
│       └── printf.cpp         # 格式化输出
│       └── interrupt.asm
│       └── isr.cpp
│       └── memory.cpp
│       └── idt.cpp
│       └── gdt.cpp
│       └── shell.cpp
├── linker.ld                 # 链接脚本
├── Makefile                  # 构建文件
├── README.md
└── .gitignore
```

---

## 许可证

本项目采用 GPLv3 许可证。