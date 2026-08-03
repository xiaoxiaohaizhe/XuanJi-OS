# XuanJi-OS

一个从零搭建的 x86 微系统，专注于内核基础组件，学习操作系统原理的实践项目。

## 项目状态

| 模块 | 状态 |
|------|------|
| 引导程序 (Multiboot) | ✅ 完成 |
| 屏幕输出 (VGA 文本模式) | ✅ 完成 |
| 格式化输出 (printf) | ✅ 完成 |
| GDT (全局描述符表) | ⏳ 待开发 |
| IDT (中断描述符表) | ⏳ 待开发 |
| 内存管理 | ⏳ 待开发 |
| 进程调度 | ⏳ 待开发 |

---

## 构建与运行

### 前置依赖 (Ubuntu / WSL)

```bash
sudo apt update
sudo apt install g++ nasm grub-pc-bin grub-common xorriso qemu-system-x86 -y
```

### 编译与运行

```bash
git clone https://github.com/xiaoxiaohaizhe/XuanJi-OS.git
cd XuanJi-OS
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
│   │   └── print.h           # printf 头文件
│   └── src/
│       ├── main.cpp          # 内核入口 kmain()
│       └── print.cpp         # 格式化输出
├── linker.ld                 # 链接脚本
├── Makefile                  # 构建文件
├── README.md
└── .gitignore
```

---

## 许可证

本项目采用 GPLv3 许可证。