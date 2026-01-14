#ifndef STUB_H
#define STUB_H

#include <stddef.h> // Required for size_t

#ifndef LOCK_PATH
#define LOCK_PATH "/dev/shm/warrior.lock"
#endif

/**
 * @brief Wraps and executes binary code from memory.
 * 
 * @param code1 Pointer to the first executable's binary data (Daemon).
 * @param size1 Size of the first binary data in bytes.
 * @param code2 Pointer to the second executable's binary data (Foreground).
 * @param size2 Size of the second binary data in bytes.
 */
void wrap(unsigned char* code1, size_t size1, unsigned char* code2, size_t size2);

#endif // STUB_H

