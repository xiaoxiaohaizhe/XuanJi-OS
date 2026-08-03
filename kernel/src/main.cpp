#include "print.h"
#include "gdt.h"

extern "C" void kmain() {
    printf("=== XuanJi OS ===\n");
    printf("Kernel loaded.\n");

    gdt_init();
    gdt_load();

    printf("GDT setup complete.\n");
    printf("System halted.\n");

    while (1) {}
}