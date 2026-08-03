#include "print.h"

extern "C" void kmain() {
    printf("=== XuanJi OS ===\n");
    printf("Kernel loaded successfully.\n");
    printf("Printf module test: %d, 0x%x\n", 2026, 0xB8000);
    printf("Next step: GDT initialization.\n");
    printf("System halted.\n");
    
    while (1) {}
}