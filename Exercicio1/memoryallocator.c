#include <stdio.h>
#include <unistd.h>
#include <stddef.h>

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
static struct mem_block *head = NULL;
static void *initial_break = NULL;

void init_memory(size_t maxmemory) {
    initial_break = sbrk(0);  // Get the initial program break
    printf("Initial program break: %p\n", initial_break);
    
    void *mem = sbrk(maxmemory);
    if (mem == (void*)-1) {
        printf("Error allocating memory\n");
    } else {
        void *current_break = sbrk(0);
        printf("Initial Heap size: %ld bytes\n", (char*)current_break - (char*)initial_break);
        printf("Allocated %zu bytes of memory\n", maxmemory);

        head = (struct mem_block*)mem;
        head->is_free = 1;
        head->size = maxmemory - sizeof(struct mem_block);
        head->mem_ptr = (void*)((char*)mem + sizeof(struct mem_block));
        head->next = NULL;
        head->prior = NULL;
    }
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

void check_heap_size() {
    void *current_break = sbrk(0);  // Get the current program break
    printf("Final program break: %p\n", current_break);
    printf("Final Heap size: %ld bytes\n", (char*)current_break - (char*)initial_break);
}

int main() {
    init_memory(MEMORY_SIZE);
    
    void *p1 = smalloc(100);
    void *p2 = smalloc(200);
    
    sfree(p1);
    void *p3 = smalloc(50);
    
    check_heap_size(); 
    
    return 0;
}
