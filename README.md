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

版本号格式：**vX.Y.Z**

由 `version.py` 在编译时自动生成，**无需手动维护**。

| 段 | 含义 | 生成方式 |
|----|------|----------|
| X | 大版本 | `(提交次数 + 1) ÷ 100 + 1`，自动计算 |
| Y | 小版本 | `(提交次数 + 1) % 100`，两位补零，自动计算 |
| Z | 补丁版本 | 默认为 `0`，修复 Bug 时手动 +1 |

**示例：**
- 第 1 次提交 → v1.01.0
- 第 21 次提交 → v1.21.0
- 第 100 次提交 → v2.00.0

**开发者说明：**
- 正常开发：版本号自动计算，无需任何手动操作
- 修复 Bug：手动修改 `version.sh` 中的 `Z` 值 +1
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
|   |   └── paging.h
│   └── src/
│       ├── main.cpp          # 内核入口 kmain()
│       └── printf.cpp         # 格式化输出
│       └── interrupt.asm
│       └── isr.cpp
│       └── memory.cpp
│       └── idt.cpp
│       └── gdt.cpp
│       └── shell.cpp
|       └── paging.cpp
├── linker.ld                 # 链接脚本
├── Makefile                  # 构建文件
├── README.md
├── version.sh
└── .gitignore
```

---

## 许可证

本项目采用 GPLv3 许可证。