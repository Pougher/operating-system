#include "allocator.h"

static HeapMetadata heap_meta;

void allocator_heap_init() {
    heap_meta.heap = vmm_map_memory(
        ALLOCATOR_HEAP_ADDRESS,
        ALLOCATOR_HEAP_SIZE
    );

    heap_meta.size = ALLOCATOR_HEAP_SIZE;
}

void allocator_init() {
    allocator_heap_init();

    // write a single heap chunk to the start of the heap that indicates that
    // the heap is completely free
    HeapChunk *first_chunk = HEAPCHUNK(heap_meta.heap);

    // initialize the first heap chunk
    first_chunk->next = NULL;
    first_chunk->last = NULL;
    first_chunk->length = heap_meta.size - sizeof(HeapChunk);
    first_chunk->in_use = 0;
}

void *allocator_get_heap() {
    return (void*)heap_meta.heap;
}

void allocator_split_heapchunk(HeapChunk *chunk, uint32_t size) {
    uint32_t new_size = chunk->length - size - sizeof(HeapChunk);
    HeapChunk *old_next_chunk = chunk->next;

    chunk->next = HEAPCHUNK(size + sizeof(HeapChunk) + (uint32_t)chunk);
    chunk->length = size;

    chunk->next->next = old_next_chunk;
    chunk->next->length = new_size;
    chunk->next->last = chunk;
    chunk->next->in_use = 0;
}

void *kmalloc(uint32_t size) {
    // first, round size up to the nearest multiple of 16
    size = (size | 15) + 1;

    HeapChunk *current_chunk = HEAPCHUNK(heap_meta.heap);

    while (current_chunk) {
        if (!current_chunk->in_use && current_chunk->length >= size) {
            if ((current_chunk->length - size) >= (2 * sizeof(HeapChunk))) {
                allocator_split_heapchunk(current_chunk, size);
                current_chunk->in_use = 1;

                // return the area of memory 16 bytes after the start of the
                // heap chunk
                return (void*)((char*)current_chunk + sizeof(HeapChunk));
            }

            // there was not enough free space after the heap chunk to split
            // the chunk, so just set the chunk as in use and return a pointer
            // to its memory
            current_chunk->in_use = 1;
            return (void*)((char*)current_chunk + sizeof(HeapChunk));
        }
        current_chunk = current_chunk->next;
    }

    return NULL;
}

void kfree(void *pointer) {
    return;
}
