#ifndef MEMORYALLOCATOR_H
#define MEMORYALLOCATOR_H

#include <stddef.h>

// Inicializa o pool de memória
void init_memory(void);

// Aloca um bloco de memória de tamanho `size`
void* smalloc(size_t size);

// Libera o bloco de memória apontado por `ptr`
void sfree(void *ptr);

#endif // MEMORYALLOCATOR_H
