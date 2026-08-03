# XuanJi-OS

一个从零搭建的x86微系统，专注于内核基础组件，学习操作系统原理的实践项目。
**状态：** 开发中 · 目前已完成引导和基础输出
- 语言：C++ / 汇编 (x86)
- 构建：GCC + NASM + LD
- 调试：QEMU + GDB
# 克隆仓库
git clone https://github.com/你的账户名/XuanJi-OS.git

# 进入目录
cd XuanJi-OS

# 构建（后续再填具体命令）
make

# 运行
make run
# 项目结构
XuanJi-OS/
├── boot/
│   └── multiboot/
│       └── boot.asm          # 引导程序 (GRUB Multiboot)
├── kernel/
│   ├── arch/                 # 处理器架构相关
│   │   └── x86_64/
│   │       ├── boot/         # 启动入口
│   │       ├── cpu/          # CPU 相关 (GDT/IDT)
│   │       └── mm/           # 内存管理 (分页/物理内存)
│   ├── include/              # 内核头文件
│   │   ├── kernel.h
│   │   ├── memory.h
│   │   ├── process.h
│   │   └── ipc.h
│   └── src/                  # 内核核心源码
│       ├── main.cpp          # 内核入口 kmain()
│       ├── memory.cpp        # 物理内存管理
│       ├── paging.cpp        # 页表管理
│       ├── process.cpp       # 进程管理
│       └── ipc.cpp           # 进程间通信
├── libs/                     # 内核专用库
│   ├── libc/                 # 精简C库
│   └── libkern/              # 内核辅助函数
├── drivers/                  # 设备驱动 (微内核用户态)
│   ├── serial/
│   ├── keyboard/
│   └── framebuffer/
├── user/                     # 用户态程序
│   ├── init/
│   └── shell/
├── docs/                     # 设计文档
│   ├── architecture.md
│   ├── memory.md
│   ├── scheduler.md
│   ├── ipc.md
│   └── boot.md
├── scripts/
│   ├── build.sh
│   └── run.sh
├── tools/                    # 工具链 (交叉编译等)
├── tests/
├── Makefile                  # 顶级构建文件
├── linker.ld                 # 链接脚本
├── README.md
├── BUILD.md
├── LICENSE
└── .gitignore

本项目采用 GPLv3 许可证。