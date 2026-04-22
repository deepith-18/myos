// Simple memory manager
// Heap starts at 2MB and grows up
#define HEAP_START 0x200000
#define HEAP_SIZE  0x100000  // 1MB heap

// Block header — sits before every allocation
struct mem_block {
    unsigned int size;
    unsigned int free;
    struct mem_block *next;
};

// Size of header itself
#define HEADER_SIZE sizeof(struct mem_block)

// Start of heap
struct mem_block *heap_start = 0;

// Initialize memory manager
void mem_init() {
    heap_start = (struct mem_block *)HEAP_START;
    heap_start->size = HEAP_SIZE - HEADER_SIZE;
    heap_start->free = 1;
    heap_start->next = 0;
}

// Allocate memory block
void *kmalloc(unsigned int size) {
    struct mem_block *current = heap_start;

    // Find a free block big enough
    while (current) {
        if (current->free && current->size >= size) {
            // Split block if much bigger than needed
            if (current->size >= size + HEADER_SIZE + 4) {
                struct mem_block *new_block =
                    (struct mem_block *)((unsigned char *)current + HEADER_SIZE + size);
                new_block->size = current->size - size - HEADER_SIZE;
                new_block->free = 1;
                new_block->next = current->next;
                current->size = size;
                current->next = new_block;
            }
            current->free = 0;
            return (void *)((unsigned char *)current + HEADER_SIZE);
        }
        current = current->next;
    }
    return 0;  // out of memory
}

// Free a memory block
void kfree(void *ptr) {
    if (!ptr) return;
    struct mem_block *block =
        (struct mem_block *)((unsigned char *)ptr - HEADER_SIZE);
    block->free = 1;

    // Merge adjacent free blocks
    struct mem_block *current = heap_start;
    while (current && current->next) {
        if (current->free && current->next->free) {
            current->size += HEADER_SIZE + current->next->size;
            current->next  = current->next->next;
        }
        current = current->next;
    }
}

// Count free and used bytes
void mem_stats(unsigned int *free_bytes, unsigned int *used_bytes) {
    *free_bytes = 0;
    *used_bytes = 0;
    struct mem_block *current = heap_start;
    while (current) {
        if (current->free) *free_bytes += current->size;
        else               *used_bytes += current->size;
        current = current->next;
    }
}
