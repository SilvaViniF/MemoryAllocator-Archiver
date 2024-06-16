#include <stdio.h>
#include "memoryallocator.h"

#pragma pack(push, 1)
struct mem_block {
    int is_free;
    size_t size;
    void *mem_ptr;
    struct mem_block *next;
    struct mem_block *prior;
};
#pragma pack(pop)

#define MEMORY_SIZE 1024
static char memory_pool[MEMORY_SIZE];
static struct mem_block *head = NULL;

void init_memory() {
    head = (struct mem_block*)memory_pool;
    head->is_free = 1;
    head->size = MEMORY_SIZE - sizeof(struct mem_block);
    head->mem_ptr = (void*)(memory_pool + sizeof(struct mem_block));
    head->next = NULL;
    head->prior = NULL;
}

void* smalloc(size_t size) {
    struct mem_block *current = head;

    while (current != NULL) {
        if (current->is_free && current->size >= size) {
            if (current->size >= size + sizeof(struct mem_block)) {
                struct mem_block *new_block = (struct mem_block*)((char*)current->mem_ptr + size);
                new_block->is_free = 1;
                new_block->size = current->size - size - sizeof(struct mem_block);
                new_block->mem_ptr = (void*)((char*)new_block + sizeof(struct mem_block));
                new_block->next = current->next;
                new_block->prior = current;
                if (current->next) current->next->prior = new_block;
                current->next = new_block;
                current->size = size;
            }
            current->is_free = 0;
            return current->mem_ptr;
        }
        current = current->next;
    }
    return NULL;
}

void sfree(void *ptr) {
    if (ptr == NULL) return;

    struct mem_block *current = head;

    while (current != NULL) {
        if (current->mem_ptr == ptr) {
            current->is_free = 1;

            if (current->prior && current->prior->is_free) {
                current->prior->size += current->size + sizeof(struct mem_block);
                current->prior->next = current->next;
                if (current->next) current->next->prior = current->prior;
                current = current->prior;
            }

            if (current->next && current->next->is_free) {
                current->size += current->next->size + sizeof(struct mem_block);
                current->next = current->next->next;
                if (current->next) current->next->prior = current;
            }

            return;
        }
        current = current->next;
    }
}
