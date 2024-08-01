<span style="font-family: Times">

# Racoon 0.0.1 Manual

### Table Of Contents
1. [Design Philosophy](#design-philosophy)
1. [System Memory Map](#system-memory-map)
2. [Pagetables](#pagetables)
3. [The Kernel](#the-kernel)
4. [The Heap](#the-heap)
5. [The Shell](#the-shell)
6. [Streams And Files](#streams-and-files)
7. [System Calls](#system-calls)
8. [Drivers](#drivers)
9. [User Applications](#user-applications)

### Design Philosophy
The core idea behind this operating system is an environment in which programs that are written have full access to all of the resources available to the operating system - that is all user programs run in whats known as "kernel mode". However, it must also be noted that all processes that are not the kernel can also not interact with internel kernel functions, as their addresses are not known at application compile time. Instead, kernel calls must be made indirectly through the use of system calls.

This methodology means that the user cna truly have full control over their experience, without being hindered by any security features over the system.

It may also appear strange to create an operating system without any security features, however the idea is that any person who uses the operating system knows what they are doing and will not install random software without first checking it for malicious code. In addition to this, any code that you decide to write could have a detrimental impact on the functioning of the system if you decide to write code that either causes undefined behaviour (UB) or overwrites any integral system data.

It is for this reason then that Racoon can be thought of as an axe - you can cut down trees with it by swinging it, but if you swing too hard you can break the axe or cause the tree to fall at an inopportune time, crushing you. You must practice steady hand control and spacial awareness whilst wielding the axe, much like what you have to do with the operating system itself.

On top of this, Racoon also has another core philosophy of simplicity - the entire operating system can be learnt and understood by one single person. This lack of complexity also means that the operating system is more beginner-friendly to programmers who want to get into operating system development for example.

This I also understand may come across as strange, because the operating system is inherently unstable and easy to break, but with enough patience, its infrastructure can be fully learnt and used to its full potential to accomplish any computing tasks you may want to complete.

This can be summarised as the following:
* Write an easy to understand operating system that is comprehensible by anyone that wishes to understand said operating system, whilst also keeping the operating system optimized enough to run quickly.
* Have the inner workings of the operating system be completely exposed to the user through the use of a defined API.
* Give the user complete control over their computer system, and place absolutely no security restrictions upon the user.

### System Memory Map
Upon booting, Racoon sets aside various virtual addresses that are assigned purposes within the kernel. These addresses range from 0xC0000000-0xC1400000 and may not be used by anything other than the kernel. This memory region is approximately all the memory Racoon needs to boot successfully, minus other memory that is needed by applications, such as the heap or the stack.

The Racoon memory map is as follows:
| Virtual Memory Address | Purpose  | Size |
|-|-|-|
|0xC0000000| The Kernel | 4 Mebibytes |
|0xC0400000 | Pagetable Data | 4 Mebibytes |
| 0xC0BFFFF0 | System Call Data | 16 Bytes |
| 0xC0800000 | Reserved | 4 Mebibytes |
| 0xC1000000 | Shell Code | 4 Mebibytes |
| 0x40000000 | The Heap | 4 Kibibytes+ |
| 0x00100000 | Loaded Applications | ~1 Gibibyte |
| 0xBFFFFFFF | The Stack | Grows Down |

_**The Kernel**_

Contains general code and functions relating to the maintaining of the proper function of the system. This code area also contains information regarding several internal datastructures which may be manipulated directly or indirectly via system calls. This memory area under no circumstances should be written to or modified, as doing so could impede the functioning of the operating system. Generally speaking, this virtual memory area also contains all loaded drivers and their respective data structures.

By default, the following drivers are loaded by the kernel:
- VGA Driver (writes to VGA registers and sets up the display to 640x480 16 colour bitplane mode, also provides functions for writing text and drawing to the screen using the internal 8x8 character font)
- Intel 8259 PIC Driver (Handles functions of the 8259 Programmable Interrupt Controller, which provides interfaces for interacting with hardware devices through Interrupt Requests, such as the keyboard or Real-Time Clock. This chip is present on all motherboards as either a physical chip or as an emulated one)

After the boot stage, the kernel is loaded first, as it must be set up prior to the running of any applications to ensure their proper function. Furthermore, it is guarunteed that the kernel has a total code and data size of less than 4MiB at compile time.

More information about the kernel can be found in the kernel section of the documentation.

_**Pagetable Data**_

In x86, addresses are represented by two different systems: virtual addresses and physical addresses. Physical addresses refer to the location of data in actual physical RAM, whereas virtual addresses are values that point to where data is in the virtual address space. For example, the physical addresses 0x049EFF90, 0x53280DF0 and 0x8C01000 may be mapped to the virtual addresses 0xA0000000...0xA0000003. This means that while the CPU may be writing to address 0xA0000000, the actual physical memory location that is being affected by this write lies at 0x049EFF90.

These addresses can be set up by writing into something called a "pagetable" which is a data structure used in x86 to represent the mapping of a set of physical addresses to their virtual counterparts. Simply speaking, you can think of a pagetable as a long list of 1024 "pages" in an array structure that has a start address in physical RAM. A "page" is the term used in x86 to refer to a 4KiB block of memory in RAM. Thus a single pagetable holds mappings for a 4MiB area of RAM. This means that the granularity of the virtual address space is 4MiB, or that is that in memory only 4MiB memory areas can be mapped at a time.

Upon booting, Racoon reserves a 4 Kibibyte region of memory to be used as a "master" pagetable, as when paging is enabled, you can only write to RAM that is mapped virtually in a pagetable.

Then, using this "master pagetable", a 4 Mebibyte region can be mapped that can then be used as RAM to store all the other pagetables the operating system could possibly need, as each pagetable being 4KiB in size * 1024 Pagetables is equal to the 4 Gibibytes accessible by a 32-Bit non-PAE architecture, and therefore this memory region is enough for all pagetable mappings.

This memory is virtually mapped to address 0xC0400000 and modifying this memory region may cause complete system memory corruption, and more informaiton regarding pagetables can be found in the pagetable section of the documentation.

**_System Call Data_**

The System Call Data region refers to 16 bytes that are used by the System Call API to store System Call IDs, operands and return values. By default the address that is used to store the system call ID is also the address used to store the return value of the system call. 

The 16 bytes is divided into 4 32-bit integer values, and writing to these locations will not cause any change in the functioning of the operating system unless an erroneous system call is raised (such as a syscall with 3 operands being called as a syscall with only 2 operands). More information can be found in the system calls section of the documentation.

_**Reserved**_

This area serves no initial purpose, however writing here is strictly prohibited as it may be used by the operating system at a later point in development.

_**Shell Code**_

[To be done]

_**The Heap**_

The Heap refers to a growing-upward section of memory that holds any data that has been allocated or deallocated by calling `malloc` or `free`. The Heap stores both Heap metadata and data contiguously in the same memory space, and so overwriting a metadata area via a heap overrun or arbitrary memory access can cause system failure. The Heap by default is allocated as a 4096 Byte block of memory, but when the Heap reaches maximum capacity, more memory may be allocated on top of this until no more physical memory is available to be used. For more information refer to the "The Heap" section of the documentation.

_**Loaded Applications**_

[To be done]

_**The Stack**_

The Stack is a data structure in x86 that refers to a section of memory and a pointer where the pointer moves downward whenever data is "pushed" onto the stack, and the pointer moves upward whenever data is "popped" from the stack.

This is done via the `push` and `pop` x86 instructions which both respectively modify the stack and also the contents of the `%esp` register (the stack pointer).

### Pagetables
_**What is a page?**_

For understanding of this section, we must first cover what a "page" physically is - in x86 for reasons regarding simplicity of memory mapping, memory is broken up into chunks called "pages". A page consists of 4096 bytes of contiguous memory and in a 32-bit architecture like this operating system is targetting, up to 4294967296 bytes of memory can be read or written to (since we only have an address bus that is 32 bits wide), and thus there are 4GiB / 4KiB or 1048576 pages in RAM. Due to the design decision to have a page have a size that is a power of 2, the addresses on the borders of pages have all lower 12 bits set to zero.

_**What is virtual memory?**_

To understand virtual memory, first think about how each address physically correlates to RAM - address 0 is the first byte in RAM, address 1 is the second and so on. Each of these addresses "maps" a value to a respective location.

Now think about how you would go about loading an application into memory, as programs that are loaded by the user must be compiled first into executable machine code before being able to be loaded. At compile time, a linker script may be used to specify an offset for things such as jumps and variable locations, so lets assume in this example that every loaded program is compiled with its offset as address 0x100000.

Take the following program:
```asm
        movl $0, %eax
loop:
        addl %eax, $1
        jmpl loop
```
This may be compiled and loaded by the operating system and run, no issues.
However, lets say that a second program is also loaded by the operating system after the first, and the first also must be kept in memory. How would this be done? We can attempt to load the second program at an address after the first, but this would break the rule that all programs start at address 0x100000, so an attempt at loading another program would cause the currently loaded program to either break, as moving it to a different place in RAM could destroy the `jmpl` instruction, but loading the second program at an address after 0x100000 would cause that program to run incorrectly, as all addresses calculated as an offset after 0x100000 would read or write to the wrong addresses.

This is where virtual memory comes in - think instead about having a second table that maps each address to its real or "physical" address, such that an address that has the value of 0x0A may actually represent a value at the physical address (the actual location in RAM) 0x2CF0. This means that when a program is loaded, its address can be virtually assigned to where the program expects to load even if the underlying RAM addresses are not where the program is designed to be loaded from. Doing it this way allows for other applications to be loaded in sequence, as they can be mapped to the correct address in place of the previously loaded application, that can then have its virtual mappings restored upon the subprocess exiting. It must however also be noted that virtual mappings can only be done on the scale of 4 MiB for reasons that will be discussed later.

_**What is a pagetable?**_

To put things simply, in x86 a pagetable is a structure that contains information regarding how each physical page maps to each virtual page. A pagetable holds 1024 entries for 1024 unique page addresses, and as discussed in the previous section, 1024 * 4096 = 4MiB per pagetable, which is where the value at the end of the previous section comes from.

Each pagetable is made up of 32 bit values as entries, and each entry can point in a completely different region of RAM.

_**How are pagetables mapped virtually?**_

To map a pagetable to a corresponding virtual address, we firstly must write the pagetable's address into the page directory, which is a structure that simply contains the addresses of all pagetables in RAM, placed at an index that corresponds to their mapped virtual address.

Upon doing this, you must then load the address of the page directory into the `cr3` register and enable paging, finally allowing for virtual mapping.

To calculate what index we need to write to, we can perform the following arithmetic operation: `address / 2^22`. So for example, if we wanted to map a pagetable to address 0xC0000000, we would write the pagetable's address to index 768 of the page directory.

_**How does Racoon do this internally?**_

Now after the brief introduction to pagetables in x86, we can finally discuss how this is done internally in the operating system.

Upon booting, we first find the regions of memory that can be used and mark them as usable. This is done via the `pmm_find_usable_memory` function. After this, paging is finally enabled, and the physical memory manager structure is initialized.

Initialisation is done through the "master pagetable" which is a pagetable that is placed statically at some offset in the kernel, and maps 4 MiB of RAM to address 0xC0400000 which can then be used by the kernel as space to place all the other pagetables that can possibly be used contiguously. This is done because after paging is enabled, any write to non-virtually mapped memory will result in a page fault. By mapping a region of memory to hold all the other pagetables that can possibly be used in the operating system, this issue is circumvented.

This block of memory then has 4 MiB of zeros written to it and is then divided up by subsequent functions to be used for the mappings of other virtual addresses that serve a predefined purpose within the operating system.

Interaction with the virtual memory manager and physical memory manager is done with the following kernel-level functions (not accessible from applications, only via system calls):

| Function name and Signature| Operands | Function | Return value |
|-|-|-|-|
| `vmm_map_memory(void*, u32)` | Virtual address, length | Maps an amount of memory specified by length (in bytes) to a specific virtual address (specified) | `void*` Virtual address that was mapped |
| `vmm_unmap_memory(void*, u32)`| Virtual address, length | Marks previously mapped region as unmapped (deallocates pages). Also invalidates the TLB entries of the memory. | None |
| `pmm_request_page()` | None | Returns a usable physical memory page, if no more memory can be allocated, NULL is returned. The page is marked as used internally. | `void*` The physical address of the page in RAM. |
| `pmm_free_page(void*)` | Physical address of a page | Marks the page as free (can be reused for another `pmm_request_page()` call) | None |

### The Kernel

The Kernel can be thought of as the heart of the operating system, providing all utility functions and maintinence functions to ensure the healthy running of a computer system. In this case, the operating system is also responsible for providing an interface by which a user may interact with the peripherals and components in a computer system.

[To be done]

### The Heap

The Heap is a data structure that refers to a large space of memory that can be carved up and "allocated" for general use by programs or by the kernel itself.

The heap always starts with a virtual base address of 0x40000000, controlled by the `ALLOCATOR_HEAP_ADDRESS` constant.

_**Heap Chunks**_

A heap chunk is a way of embedding metadata in the heap to provide information to the allocator on how the heap is divided at any one moment. All heap chunks also act as headers, and are always positioned at an offset of -16 bytes from any returned pointer by a function that uses the internal allocator.

A Heap chunk is defined as the following:
```c
typedef struct HeapChunk {
    struct HeapChunk *next;
    struct HeapChunk *last;
    uint32_t length;
    uint32_t in_use;
} HeapChunk;
```
At the surface level, a heap chunk appears to be a doubly-linked list, as it contains pointers to the next and last chunk on the heap, and if you made that assumption, you would be correct. The length field provides a value for the "size" of the chunk in bytes (eg. how large the allocation actually is), and the `in_use` variable's first bit is used to detect if the chunk is in use or not (free or used).

Heap chunk metadata abides by the following rules:
* Heap chunks are exactly 16 bytes in size to ensure 16 byte alignment for any pointer returned by a function that returns a pointer on the heap, meaning the memory address returned can be used in any x86 instruction.

* If no next or last heap chunks are present, the fields `next` and `last` are set to `NULL`.
* Length is always a multiple of 16, and cannot be 0.
* Only the first bit of `in_use` is used to signify if the heap chunk is in use or not.

_**The Heap Allocation Algorithm**_

1. The heap is first allocated with a size of 4096 bytes with `vmm_map_memory(0x40000000, 4096)`, and a single base heap header is written to the first 16 bytes of the heap. This heap header contains all zeros apart from the length which is set to be equal to 4096 - `sizeof(HeapChunk)`, which is equivalent to 4080 bytes.
2. When an allocation is requested, the requested allocation size is rounded up to the nearest 16 bytes. Then, starting with a pointer to the first heap chunk, the linked list of heap chunks is traversed forwards, checking for a heap chunk that is not in use (bit 0 of `in_use` is 0).
3. If a free chunk is found, its length is tested and if it is greater than or equal to the requested allocation size, then the algorithm proceeds to test if the length of the chunk minus the allocation size is more than `2 * sizeof(HeapChunk)`. If it is, then the heap chunk is split into several chunks with the first being the rounded allocation size and the second being a free heap chunk containing the remaining length of the original heap chunk.
4. The address of the first split heap chunk + 16 is then returned as the address of the allocation, and the first split heap chunk has its `in_use` bit 0 set.
5. If no heap chunk is large enough for the rounded allocation, then additional memory is allocated to the heap with the size of the requested allocation rounded up to the nearest 4096 bytes.
6. If this allocation fails, then `NULL` is returned indicating that the heap failed to find enough memory to complete the rounded allocation.
7. When a chunk is requested to be freed, its `in_use` bit 0 is cleared. Then, the chunks behind and in front of the chunk are tested to see if they are freed or not as well. If they are, the chunks are merged into a single chunk at the lowest possible heap address (the address of the lowest free chunk relative to the chunk being freed, so if the next chunk is free it will be merged backwards, and if the last chunk is free the current chunk will be merged backwards).

This algorithm offers the following constraints:
- Every allocation must be freed at some point, otherwise memory will be leaked.
- Allocations become progressively slower as the heap becomes more fragmented.
- The heap is not very secure against buffer overruns, which can corrupt heap metadata.

_**Heap Metadata Corruption Detection**_

The allocator provided with Racoon does have a level of security provided through a checksum that is stored in the upper 24 bits of the `in_use` variable.

Whenever a heap operation is performed on a heap chunk, its checksum is tested by calculating the checksum of the heap chunk in its current state and testing for equality against the stored heap chunk. If the heap chunk's calculated checksum does not match its stored checksum, then an error is raised. This does however come with the downside that whenever an operation is performed on a heap chunk that mutates it in any way, its checksum must be recalculated.

The heap metadata checksum is calculated in the following way:
$$
checksum = (3 \cdot (next + last)) \oplus (7 \cdot length)
$$
With the final 8 bits of the checksum being disregarded when it is written to `in_use`.

The error raised by the corruption detection algorithm is a kernel-level panic that causes the system to lock up. Furthermore, the checksum is only tested upon allocation, so deallocation will not raise a corruption error.

_**Provided Kernel Level Functions**_

The following functions are provieded by the allocator to the kernel, and are accessible by users via system calls:

| Function name and Signature| Operands | Function | Return value |
|-|-|-|-|
| `kmalloc(u32)` | Size of allocation | Allocates an area of size `size` in bytes on the heap | `void*` Allocation address |
| `kfree(void*)` | Address of a memory allocation provided by the allocator (from `kmalloc`) | Marks an area as free to the allocator | None |

</span>
