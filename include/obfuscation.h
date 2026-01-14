#ifndef OBFUSCATION_H
#define OBFUSCATION_H

#include <stddef.h> // Required for size_t

#define XOR_Key 0x77

void xor(unsigned char* code, size_t size);

#endif // OBFUSCATION_H
