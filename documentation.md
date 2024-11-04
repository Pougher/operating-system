<span style="font-family: Times">

# Racoon 0.0.1 Manual

### Table Of Contents
I. [Design Philosophy](#design-philosophy)
II. [System Memory Map](#system-memory-map)
III. [Pagetables](#pagetables)
IV. [The Kernel](#the-kernel)
V. [The Heap](#the-heap)
VI. [The Shell](#the-shell)
VII. [Streams And Files](#streams-and-files)
VIII. [System Calls](#system-calls)
IX. [System Call List](#system-call-list)
X. [Drivers](#drivers)
XI. [User Applications](#user-applications)
XII. [Style Guide](#style-guide)
XIII. [Documentation Assembly Guide](#assembly-guide-documentation-only)
XIV. [Glossary](#glossary)

### Design Philosophy
The core idea behind this operating system is an environment in which programs that are written have full access to all of the resources available to the operating system - that is all user programs run in whats known as "kernel mode". However, it must also be noted that all processes that are not the kernel can also not interact with internel kernel functions, as their addresses are not known at application compile time. Instead, kernel calls must be made indirectly through the use of system calls.

This methodology means that the user can truly have full control over their experience, without being hindered by any security features over the system.

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
| 0x40000000 | The Heap | 4 Kibibytes+ |
| 0x00100000 | Loaded Applications | ~1 Gibibyte |
| 0xBFFFF000 | The Stack | Grows Down, default size is 8 Mebibytes |

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

_**The Heap**_

The Heap refers to a growing-upward section of memory that holds any data that has been allocated or deallocated by calling `malloc` or `free`. The Heap stores both Heap metadata and data contiguously in the same memory space, and so overwriting a metadata area via a heap overrun or arbitrary memory access can cause system failure. The Heap by default is allocated as a 4096 Byte block of memory, but when the Heap reaches maximum capacity, more memory may be allocated on top of this until no more physical memory is available to be used. For more information refer to the "The Heap" section of the documentation.

_**Loaded Applications**_

[To be done]

_**The Stack**_

The Stack is a data structure in x86 that refers to a section of memory and a pointer where the pointer moves downward whenever data is "pushed" onto the stack, and the pointer moves upward whenever data is "popped" from the stack.

This is done via the `push` and `pop` x86 instructions which both respectively modify the stack and also the contents of the `%esp` register (the stack pointer).

When the kernel is loaded, low level setup is first performed by `kernel_init_low_level()`, and then afterward the kernel stack is moved - upon boot, 16KiB is assigned to the kernel stack space, but that amount of stack space wont get you very far, so after low level setup an 8MiB user space is allocated for use as a larger kernel stack. This is done in `kernel_stack_relocate()` and must be done in its own separate function to prevent the corruption of stack space variables that are used in other functions - it must also be noted that after the stack is moved, local variables can be used in functions _other than_ `kernel_main`, but before the stack is moved, stack space variables risk being corrupted by the stack move.

**Warning: After the stack switch, kernel main cannot use any local variables allocated in its own stack frame**

The stack is moved to the memory region 0xBF800000 -> 0xBFFFF000 spanning 8MiB, and the move has the following effects on the following CPU registers:

* `%esp` is set to 0xBFFFF000 (the top of the new stack)
* `%ebp` is also set to 0xBFFFF000

This has the side effect of all stack frames set up before the stack move being corrupted, hence why the stack move function must not be called anywhere other than where it is called in `kernel_main`.

### Pagetables
_**What is a page?**_

For understanding of this section, we must first cover what a "page" physically is - in x86 for reasons regarding simplicity of memory mapping, memory is broken up into chunks called "pages". A page consists of 4096 bytes of contiguous memory and in a 32-bit architecture like this operating system is targetting, up to 4294967296 bytes of memory can be read or written to (since we only have an address bus that is 32 bits wide), and thus there are 4GiB / 4KiB or 1048576 pages in RAM. Due to the design decision to have a page have a size that is a power of 2, the addresses on the borders of pages have all lower 12 bits set to zero.

_**What is virtual memory?**_

To understand virtual memory, first think about how each address physically correlates to RAM - address 0 is the first byte in RAM, address 1 is the second and so on. Each of these addresses "maps" a value to a respective location.

Now think about how you would go about loading an application into memory, as programs that are loaded by the user must be compiled first into executable machine code before being able to be loaded. At compile time, a linker script may be used to specify an offset for things such as jumps and variable locations, so lets assume in this example that every loaded program is compiled with its offset as address 0x100000.

Take the following program:
```asm
                movl $0x00, %eax
loop:           addl %eax, $0x01
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

### The Shell
[To be done]

### Streams And Files

Similarly to many *nix operating systems, racoon has a similar way of loading things using files. Put simply, a file simply represents an area of memory that can be either written to or read from, and have its state be mutated in some way. Files contrary to common belief do not have to correspond to physical files on a disk somewhere, and are simply virtual constructs that refer to a piece of information stored in memory. In this way, a file can be thought of as a "stream" where information can be "streamed" out of the file and likewise can be "streamed" into the file.

_**The strucutre of a file**_

A file is structured as having a "file pointer", a "state" and several functions that are run whenever a file read of write is performed. In addition, files also have a value that indicates whether or not the file is open or not.

Simply put, a file's structure looks like the following C code:
```c
typedef struct File {
    uint32_t file_pointer;
    void *state;
    void (*init_function)(struct File*);
    void (*write_function)(struct File*, char*, uint32_t);
    void (*read_function)(struct File*, void*, uint32_t);
    void (*close_function)(struct File*);
    bool is_open;
} File;
```

The arguments to each of the file manipulation functions other than the `struct File*` represent buffers and lengths of the data that is being written to or read from the file.

_**How Files are stored**_

Files are organized as a large 2048-file long consecutive array in kernel space. Each file may be referred to by either a file descriptor or by a pointer to the file. The difference between the two is that a file descriptor represents a single index into the file array (representing a single file) whereas a pointer to a file represents a file that exists at some location in memory. This means that a file _can_ be created outside of the internal file array, but will not have a file descriptor and can only be referred to by a pointer to a file stored somewhere else in memory.

Since this distinction is hard to describe, I will include the following example to illustrate the difference between the two methodologies:
```c
File *file; // pointer to a file
uint32_t file; // a file descriptor
```

In addition to the differences described in the section above, a pointer to a file may not have certain functionality be usable to itself, for example many functions within the kernel deal with file descriptors only, and so a pointer to a file would not work in those instances. However, having a pointer to a file may be useful in a scenario where you want to use the file data structure in a way that does not impact the total number of allocated files.

Furthermore, I have in these examples used the type `uint32_t` to refer to the file descriptor's type, but it should be noted that in the kernel's source code, the file descriptor is referenced by the `FileDescriptor` type, which is simply a wrapper for the `uint32_t` type.

_**File Creation**_

When file construction is initiated, the operating system first searches for an open file slot (that is, a slot that indicates the file is not open) of which 2048 files can exist concurrently.

Once the open slot is found, the file struct is initialized with the functions that control file reads, file writes and file opening and closing. These do not have to be specified, and NULL functions can be written to the function fields of the struct, indicating to the respective file manipulation functions that the functions are not the be called. Furthermore, upon file construction the file pointer is initialized to 0 and `is_open` is set to `true`.

After the file creation step, the file's `init_function` is called to initialize any prerequesites the file may need before being usable.

It should be noted that the `close_function` is called upon the file being destroyed, in which the file is simply marked is closed, such that it can be reused for later file creation. This means that if you are working with a file that has its structure allocated by an allocator such as `malloc` then the memory allocated to the structure must be cleaned up after the file is marked as closed. Furthermore, it is good practise to clean up memory allocated by the file initialization function in the file close function to reduce the risk of memory leaks.

_**Default Files**_

By default, the kernel opens several files to provide functionality for kernel functions, however these files can also be used by user applications exclusively to recieve input or to return output, specifically to the terminal. These default file descriptors are labelled as the following:

* **stdin** (Standard Input) - recieves input bytes into a 4096 byte buffer. This buffer can be read from and written to, and when the buffer reaches maximum capacity, any further recieved input bytes are ignored.
* **stdout** (Standard Output) - this file cannot be read from, but can be written to. Bytes written to this file are output to the terminal or to whatever process currently has control over the standard output stream.

These default files are useful because they allow multiple subsequent processes to communicate with one another - for example if process A _opens_ process B, process B's `stdout` can be rerouted to a temporary buffer and then be read from by process A, allowing process A to capture the output of another process. This also provides a universal interface by which processes can output information and display it to the user.

_**Kernel Level Functions**_

The following functions are provieded by the allocator to the kernel, and are accessible by users via system calls (with the exception of `file_create`, more information is provided in the system calls section of the documentation):

| Function name and Signature | Operands | Function | Return value |
|-|-|-|-|
| `file_write(FileDescriptor, char*, u32)` | The file descriptor, pointer to data to write, length of data in bytes | Writes `length` bytes of data to the file descriptor | None |
| `file_read(FileDescriptor, void*, u32)` | The file descriptor, pointer to the object to write data to, the number of bytes that should be written to the object | Reads a number of bytes from a file descriptor into an object | None |
| `file_delete(FileDescriptor)` | The file descriptor to delete | Marks a file as closed and runs its termination function | None |
| `file_create(FileInitFunc, FileWriteFunc, FileReadFunc, FileCloseFunc)` | File initialization function, file writing function, file reading function, file close function | Finds a file open slot and marks it as open, initializes the file's structure and returns a file descriptor | `u32` The new file's descriptor |

### System Calls

A system call refers to a number and an interrupt, where the interrupt triggers the system call interrupt service routine (ISR) and the number indicates what system call is to be run. This is done through software interrupt 64 (0x40).

_**API**_

The system call API for racoon is relatively simple - 4 memory addresses correspond to the parameters and number of any given system call. These addresses are the following:

* 0xC0BFFFF0: 32-Bit system call number
* 0xC0BFFFF4: 32-Bit system call operand number 1
* 0xC0BFFFF8: 32-Bit system call operand number 2
* 0xC0BFFFFC: 32-Bit system call operand number 3

As seen in the list above, any system call can have up to 3 operands that are passed to the system call handler by the interrupt, and must be set to the correct values before the software interrupt instruction (`int 0x40`) is run. When a system call routine is run however, it is given that the registers used during the system call handler will be preserved as they were before the system call.

Because system calls usually return a value, upon completion of a system call the address of the system call number (0xC0BFFFF0) is overwritten with the return value of the system call handler, meaning that in order to get the result of a system call you must first load a register back with the system call number address to retrieve the return value.

It should also be noted that not all system calls return a value at all, and not all system calls expect up to 3 arguments (the maximum number of arguments). In the case of no return value, a single value indicating system call execution success or failure is written to the return address, with a success being indicated by a 0 and a failure being indicated by -1. When a system call expects less than 3 operands, you dont have to load the other operand addresses, and it should be noted that if you call a system call with more or less arguments than it expects then the result is undefined, however in the majority of circumstances as long as _at least_ the correct number of arguments are provided then the system call will behave as expected.

Elaborating further on system calls having variable arguments, the source code for the kernel defines 4 functions that can also be used by a loaded program - `syscall`, `syscall1`, `syscall2` and `syscall3`. The number at the end of the label indicates the number of arguments the syscall takes, and each subroutine simply takes the arguments (if any) provided and writes them to the syscall operand and number addresses, before causing software interrupt 64. These subroutines can be used from higher level C code via functions that share the same names. The example below shows the source code for `syscall3`:

```asm
syscall3:       movl 4(%esp), %eax
                movl %eax, $C0BFFFF0
                movl 8(%esp), %eax
                movl %eax, $C0BFFFF4
                movl 12(%esp), %eax
                movl %eax, $C0BFFFF8
                movl 16(%esp), %eax
                movl %eax, $C0BFFFFC
                int $0x64
                movl $C0BFFFF0, %eax
                retl
```

_**Defining System Calls**_

_This is for kernel development only, but the implementation details may be useful for user applications_

In order to define a system call, you first need to write a system call _handler_. A handler is simply the piece of code that is executed when the system call is called, and racoon provides a few utilities to assist in writing handlers, namely the `SYSCALL_DEFINEn(number, ...)` macros, where _n_ represents the number of arugments that the system call takes. Every one of these macros expects at least one argument representing the system call number that the handler corresponds to. Furthermore, these macros are used to define functions, so the arguments named in the macro represent the actual names of the arguments in the defined handler (other than the first, which is actually consumed by the macro). Lastly, the macros only define the function signature, so a pair of curly braces to begin the function body are required after the macro.

`SYSCALL_DEFINEn` macros only ever pass arguments that are the size needed to represent any pointeron the given system, that is on a 32-bit operating system like racoon each argument passed by the macros will be of the type `uint32_t`. This means that in the body of the function, the arguments passed by the define macro will have to be casted to types that the system call handler wishes to operate upon.

When writing system call handlers, you must always return a value. In the case that your handler does not actually _return_ a value, the system call must still set `0xC0BFFFF0` to either a 0 or -1. Returns are performed with the `SYSCALL_RETURN` macro, which writes the value to the afformentioned memory address that you provided. The return values 0 and -1 are also provided as the macros `SYSCALL_SUCCESS` and `SYSCALL_FAILURE` respectively.

An example system call handler is provided below:
```c
// example system call that performs a memory allocation and returns a pointer
// to the allocated memory (for more information on memory allocators, refer to
// the heap section of the documentation)
SYSCALL_DEFINE1(0, size) {
    // the first argument of SYSCALL_DEFINE1 states that this is system call
    // handler ID number 0

    // call kmalloc with the 1st argument of the system call (size)
    const uint32_t address = (uint32_t) kmalloc(size);

    // finally, return the allocated memory with SYSCALL_RETURN
    SYSCALL_RETURN(address);
}
```

You may also want to define your system call handler's function prototype in a header file, instead of just having it be a standalone C file. This can be done via the `SYSCALL_PROTO_DEFINE(n)` macro where _n_ is the syscall handler number of your prototype (in layman's terms, the same number as the first argument in the system call definition macro). You can then include your header file in the `syscall.h` source file. An example of this is provided below:
```c
// file: syscall_prototype.h
SYSCALL_PROTO_DEFINE(0);
```

After you have defined your system call handler, you then need to add it to the system call table, which put simply is a large array of function pointers to handlers that is indexed whenever a system call is initiated. To do this, you must use the `SYSCALL_SET(n)` macro in the `syscall_init` subroutine (found in `core/syscall.c`). An example of this can be found below which sets up system call handler 0:
```c
// file: syscall.c
SYSCALL_SET(0);
```

Now the system call should be usable across the operating system by triggering an `int 0x40`

_**Benefits and drawbacks of System Calls**_

The philosophy behind the use of system calls stems from simplicity - we can avoid mapping the kernel into a process and thus less code is needed when handling the loading of applications. Furthermore, it also means that there is a defined API for performing certain tasks, reducing the total number of ways to accomplish something and thereby simplifying the process of writing code for the operating system. This becomes a huge benefit when working on large codebases, as you spend less time thinking about how to do something and more time _actually_ implementing it.

System calls also have the benefit of being able to be used anywhere with little to no setup - all you really need is some operands and a number and you can perform whatever task you want, however one issue with this idea is that it becomes rather verbose to remember all of the the system call IDs, so you may have to spend some time searching through the system call list below to find the ID of the system call that does what you want.

### System Call List

[TBD]

### Style Guide

[TBD]

### Assembly Guide (Documentation Only)

This assembly guide is intended to explain the semantics of the assembly code listings that are provided throughout the documentation, as they differ from the assembly format used in the source code of racoon.

_**Basics**_

Every instruction listed in the assembly listings should always be in lowercase, and is always indented to 3 tab widths. Furthermore, the ordering of the operands is "from, to", so `mov %esp, %ebp` would mean move the contents of register `%esp` into register `%ebp`. There is also the dereference operator which allows for _indirect addressing_, which is represented by a pair of brackets aroun a register, for example: `movl (%ecx), %eax` would move the contents of the memory pointed to by register `%ecx` into register `%eax`.

Register formatting is also quite simple, as each register name is preceeded by a percent sign (%).

_**Instruction Sizes**_

Certain instructions may move different amounts of memory that they use, eg. 32 bit operands, 16 bit operands etc., so this documentation borrows from AT&T-like x86 syntax and appends a one letter suffix to the end of each instruction to indicate its size.

The table below illustrates instruction suffixes and the operand size they indicate:
| Suffix | Operand Size | Example |
|-|-|-|
| b | 8 Bits (1 Byte) | `movb $0xab, %ax` |
| w | 16 bits (2 Bytes) | `movw $0xabcd, %ax` |
| l | 32 bits (4 Bytes) | `movl $0xabcdef12, %eax` |
| q | 64 bits (8 Bytes) | `movq $0xabcdef1234567890, %rax` |

_Note that the 64 bit suffix will be almost never used since this is a 32 bit operating system_

_**Addresses, Literals, Indirects and Offsets**_

In x86, you can load a value from either RAM or literally load it into the register, and in order to distinguish this, two different types of declaration for numeric values are used: For literals (eg. 1234, 0xabcdef) a dollar sign (\$) followed by the base indicator (`0x` for hexadecimal, `#` for decimal and `&` for octal). For RAM addreses, only hexadecimal is used and an address is prefixed by one dollar sign (\$). Furthermore, x86 also supports indirect addressing modes (as discussed above), whereby a memory address or a register is surrounded by brackets to indicate the loading of a value that is being pointed to in RAM. If the value given is a RAM address, then a single dollar sign is used to begin the hexadecimal address. If it is a register then only the register name surrounded by brackets is used.

Finally, x86 also has something called on offset, which put simply is a value that is added to another register or multiplied with a register to give a new memory address. Rules for doing this in x86 are the following:
* Addition and subtraction are done last if other operations are present
* A value can only be added to or subtracted from once
* Division cannot be used as an offset
* Up to [CITATION NEEDED] registers can be added together
* A register can be added to another register which has been multiplied with another value
* Multiplications can only ever be a power of 2
* Additions must be a multiple of 2
* Only one multiplication can ever occur between 2 registers
* Operations can only be done when loading values indirectly
For the assembly used in this documentation, additions or subtractions to the register come outside of the brackets as either a positive or negative number. On the inside of the brackets, if an addition is taking place between two registers then will be written as `%xxx + %yyy`. If a multiplication on a register is taking place then it will be written as `%xxx * %yyy`. Some examples of how offsets are written can be found below:
```asm
            ; value at address %esp-8 is put in register eax
            movl -8(%esp), %eax 
            ; value at address (%ecx * %edx) - 8 is put into eax
            movl -8(%ecx * %edx), %eax
            ; value at address (%eax + (%edx * 8)) + 4 is put into eax
            movl 4(%eax + %edx * 8), %eax
```

### Glossary

Various definitions for terms used throughout the documentation.
| Word | Definition |
|-|-|
| Memory Map | A section of information that describes how memory is to be divided for different purposes, usually includes a memory address, a range and a specific function |
| Page | In x86* a page represents a 4096-byte region of memory |
| Aligned | Something is placed in a memory location that is divisible by its **alignment**, for example 256 is 16-byte aligned |
| File | An open stream of information that can either represent a physical piece of data stored on a hard disk or a virtual data structure |
| Virtual Memory | Memory addresses that do not represent physical locations in RAM |
| Physical Memory | An actual location in RAM that could be used by any system to address the same byte |
| File Descriptor | An unsigned 32-bit integer that represents an index into the file array, referencing a file |
| Pointer | A variable that stores the memory location of another variable. The other variable's data can be accessed by dereferencing the pointer |
| System Call | A number that corresponds to a **system call handler** that performs a certain task, initiated by an `int 0x40` |
| System Call Handler | A routine that is executed when its ID is passed to a **system call** |
| Interrupt | An event that causes the processor to pause execution and jump to an interrupt **vector** which performs a certain task |
| Vector | A program location or address of a function that is called from elsewhere |
| Address | A number that represents a location in memory |

\* = Specific to this implementation

</span>
