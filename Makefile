# 版本号自动生成
VERSION := $(shell ./version.sh 2>/dev/null || echo "v1.0.0")

# 编译器与链接器
CC = g++
ASM = nasm
LD = ld
QEMU = qemu-system-x86_64
GRUB_MKRESCUE = grub-mkrescue

# 编译选项
CXXFLAGS = -m32 -ffreestanding -nostdlib -fno-exceptions -fno-rtti -I kernel/include -DVERSION=\"$(VERSION)\"
ASMFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld

# 源文件列表
CXX_SOURCES = kernel/src/main.cpp kernel/src/printf.cpp kernel/src/gdt.cpp kernel/src/memory.cpp kernel/src/idt.cpp kernel/src/isr.cpp kernel/src/paging.cpp kernel/src/shell.cpp
CXX_OBJECTS = $(CXX_SOURCES:.cpp=.o)
ASM_SOURCES = boot/multiboot/boot.asm kernel/src/interrupt.asm
ASM_OBJECTS = $(ASM_SOURCES:.asm=.o)
OBJECTS = $(CXX_OBJECTS) $(ASM_OBJECTS)

# 目标文件
KERNEL_BIN = kernel.bin
ISO_FILE = xuanji.iso
ISO_DIR = iso

# 默认目标
all: $(ISO_FILE)

# 编译 C++ 源文件
%.o: %.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@

# 编译汇编源文件
%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

# 链接内核
$(KERNEL_BIN): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

# 构建 ISO
$(ISO_FILE): $(KERNEL_BIN)
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(KERNEL_BIN) $(ISO_DIR)/boot/
	cp boot/grub/grub.cfg $(ISO_DIR)/boot/grub/
	$(GRUB_MKRESCUE) -o $@ $(ISO_DIR)

# 运行 QEMU
run: $(ISO_FILE)
	$(QEMU) -cdrom $^ -m 256M -vga std

# 清理构建产物
clean:
	rm -f $(OBJECTS) $(KERNEL_BIN) $(ISO_FILE)
	rm -rf $(ISO_DIR)

.PHONY: all run clean