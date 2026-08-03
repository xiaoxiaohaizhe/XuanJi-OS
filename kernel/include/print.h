#ifndef PRINT_H
#define PRINT_H

#include <stdarg.h>

void print_char(char c);
void print_string(const char* str);
void print_int(int num);
void print_hex(unsigned int num);
void printf(const char* format, ...);

#endif