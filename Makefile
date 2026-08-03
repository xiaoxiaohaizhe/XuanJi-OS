# 编译器与链接器
CC = g++
ASM = nasm
LD = ld
QEMU = qemu-system-x86_64
GRUB_MKRESCUE = grub-mkrescue

# 编译选项
CFLAGS = -m32 -ffreestanding -nostdlib -fno-exceptions -fno-rtti -I kernel/include
ASMFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld

# 源文件列表
CXX_SOURCES = kernel/src/main.cpp kernel/src/printf.cpp
CXX_OBJECTS = $(CXX_SOURCES:.cpp=.o)
ASM_SOURCES = boot/multiboot/boot.asm
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
	$(CC) $(CFLAGS) -c $< -o $@

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
	$(QEMU) -cdrom $^

# 清理构建产物
clean:
	rm -f $(OBJECTS) $(KERNEL_BIN) $(ISO_FILE)
	rm -rf $(ISO_DIR)

.PHONY: all run clean