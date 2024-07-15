<span style="font-family: Times">

# Racoon 0.0.1

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

_**Shell Code**_

[To be done]

_**The Heap**_

The Heap refers to a growing-upward section of memory that holds any data that has been allocated or deallocated by calling `malloc` or `free`. The Heap stores both Heap metadata and data contiguously in the same memory space, and so overwriting a metadata area via a heap overrun or arbitrary memory access can cause system failure. The Heap by default is allocated as a 4096 Byte block of memory, but when the Heap reaches maximum capacity, more memory may be allocated on top of this until no more physical memory is available to be used. For more information refer to the "The Heap" section of the documentation.

_**The Stack**_

The Stack is a data structure in x86 that refers to a section of memory and a pointer where the pointer moves downward whenever data is "pushed" onto the stack, and the pointer moves upward whenever data is "popped" from the stack.

This is done via the `push` and `pop` x86 instructions which both respectively modify the stack and also the contents of the `%esp` register (the stack pointer).
</span>