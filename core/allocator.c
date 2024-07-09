#include "allocator.h"

static HeapMetadata heap_meta;

void allocator_heap_init() {
    heap_meta.heap = vmm_map_memory(
        ALLOCATOR_HEAP_ADDRESS,
        ALLOCATOR_HEAP_SIZE
    );

    heap_meta.size = ALLOCATOR_HEAP_SIZE;
    heap_meta.allocated = 16;
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

uint32_t allocator_get_allocated() {
    return heap_meta.allocated;
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
    if (size % 16 != 0) size = (size | 15) + 1;

    HeapChunk *current_chunk = HEAPCHUNK(heap_meta.heap);
    HeapChunk *last_chunk = current_chunk;

    while (current_chunk) {
        if (!current_chunk->in_use && current_chunk->length >= size) {
            if ((current_chunk->length - size) >= (2 * sizeof(HeapChunk))) {
                allocator_split_heapchunk(current_chunk, size);
                current_chunk->in_use = 1;

                heap_meta.allocated += (sizeof(HeapChunk) + size);

                // return the area of memory 16 bytes after the start of the
                // heap chunk
                return (void*)((char*)current_chunk + sizeof(HeapChunk));
            }

            // there was not enough free space after the heap chunk to split
            // the chunk, so just set the chunk as in use and return a pointer
            // to its memory
            current_chunk->in_use = 1;
            heap_meta.allocated += current_chunk->length;
            return (void*)((char*)current_chunk + sizeof(HeapChunk));
        }

        last_chunk = current_chunk;
        current_chunk = current_chunk->next;
    }

    // if there isn't enough memory for the new heap chunk to fit, grow the heap
    // to a multiple of 4096 bytes that is large enough to contain the memory
    // being requested
    uint32_t additional_memory = (size % 4096 == 0) ?
        size : ((size | 4095) + 1);

    if (vmm_map_memory(
            heap_meta.heap + heap_meta.size,
            additional_memory) == NULL) {
        return NULL;
    }

    last_chunk->length += additional_memory;
    heap_meta.size += additional_memory;

    return kmalloc(size);
}

void kfree(void *pointer) {
    HeapChunk *chunk = HEAPCHUNK((char*)pointer - sizeof(HeapChunk));
    HeapChunk *next_chunk = chunk->next;
    HeapChunk *last_chunk = chunk->last;

    chunk->in_use = 0;

    heap_meta.allocated -= chunk->length;

    // coalesce chunks that are adjacent and also free
    if (next_chunk) {
        if (!next_chunk->in_use) {
            next_chunk->last = chunk;
            chunk->next = next_chunk->next;
            chunk->length = next_chunk->length + sizeof(HeapChunk) +
                chunk->length;
            heap_meta.allocated -= sizeof(HeapChunk);
        }
    }
    if (last_chunk) {
        if (!last_chunk->in_use) {
            last_chunk->next = chunk->next;
            last_chunk->length = chunk->length + last_chunk->length +
                sizeof(HeapChunk);
            next_chunk->last = last_chunk;
            heap_meta.allocated -= sizeof(HeapChunk);
        }
    }

    return;
}
