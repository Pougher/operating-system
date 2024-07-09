#ifndef CORE_ALLOCATOR_H
#define CORE_ALLOCATOR_H

#include <stdint.h>

#include "virtual_memory_manager.h"

#define ALLOCATOR_HEAP_ADDRESS  ((void*)0x40000000)
#define ALLOCATOR_HEAP_SIZE     4096

// function macro to implicitly cast any memory to a heapchunk
#define HEAPCHUNK(mem) ((HeapChunk*)(mem))

// stores information about the current state of the heap, such as its current
// size and its virtual address
typedef struct {
    // pointer to the virtual address of the heap
    char *heap;

    // the size of the heap in bytes
    uint32_t size;

    // the number of bytes that are currently allocated
    uint32_t allocated;
} HeapMetadata;

// stores information about a specific portion of memory on the heap. Type is
// exactly 16 bytes to provide a 16-byte alignment to all malloc'd memory
typedef struct HeapChunk {
    // pointer to the next chunk in the heap, is NULL if there is no next heap
    // chunk (4 bytes)
    struct HeapChunk *next;

    // pointer to the last chunk in the heap, is NULL if there is no last heap
    // chunk (4 bytes)
    struct HeapChunk *last;

    // the length of the heap chunk in bytes (4 bytes)
    uint32_t length;

    // stores a boolean, padded to 4 bytes to make the structure 16 bytes,
    // dictates whether or not the heap chunk is free for use (4 bytes)
    uint32_t in_use;
} __attribute__((packed, aligned(16))) HeapChunk;

// initializes the heap structure by setting default size, and mapping some
// memory for the heap
void allocator_heap_init();

// initializes the entire allocator
void allocator_init();

// returns a pointer to the heap
void *allocator_get_heap();

// returns the number of bytes currently allocated on the heap
uint32_t allocator_get_allocated();

// splits a heap chunk into two consecutive heap chunks
void allocator_split_heapchunk(HeapChunk*, uint32_t);

// allocates memory of the size provided and returns it as a pointer to memory
void *kmalloc(uint32_t);

// frees the memory provided by kmalloc
void kfree(void*);

#endif
