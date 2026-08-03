extern "C" void kmain() {
    char* video_memory = (char*) 0xB8000;
    const char* msg = "Hello, XuanJi OS!";
    for (int i = 0; msg[i] != '\0'; i++) {
        video_memory[i * 2] = msg[i];
        video_memory[i * 2 + 1] = 0x0F;
    }
    while (1) {}
}
