# Declare constants for the multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# virtual base of the kernel to be added to any value passed to the kernel
.set KERNEL_VIRTUAL_BASE, 0xC0000000

# Declare a multiboot header that marks the program as a kernel.
.section .multiboot.data, "aw"
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Allocate the initial stack:sparkles: .
.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

# Preallocate pages used for paging. Don't hard-code addresses and assume they
# are available, as the bootloader might have loaded its multiboot structures or
# modules there. This lets the bootloader know it must avoid the addresses.

.section .bss, "aw", @nobits
	.align 4096
boot_page_directory:
	.skip 4096
boot_page_table1:
	.skip 4096

# page table 2 maps all of the page tables into RAM from 0xC0400000-0xC0800000
boot_page_table2:
    .skip 4096

# Further page tables may be required if the kernel grows beyond 4 MiB.

# The kernel entry point.
.section .multiboot.text, "a"
.global _start
.type _start, @function
_start:
	# Physical address of boot_page_table1.
	# TODO: I recall seeing some assembly that used a macro to do the
	#       conversions to and from physical. Maybe this should be done in this
	#       code as well?
	#movl $(boot_page_table1 - 0xC0000000), %edi
	# First address to map is address 0.
	# TODO: Start at the first kernel page instead. Alternatively map the first
	#       1 MiB as it can be generally useful, and there's no need to
	#       specially map the VGA buffer.
#	movl $0, %esi
	# Map 1006 pages. The 1008-1024th will be the VGA pixel buffer, and the
    # 1007th will be the multiboot header
#	movl $1007, %ecx

	# Only map the kernel.
#	cmpl $_kernel_start, %esi
#	jl 2f
#	cmpl $(_kernel_end - 0xC0000000), %esi
#	jge 3f
#
#	# Map physical address as "present, writable". Note that this maps
#	# .text and .rodata as writable. Mind security and map them as non-writable.
#	movl %esi, %edx
#	orl $0x003, %edx
#	movl %edx, (%edi)
#
    # Size of page is 4096 bytes.
#	addl $4096, %esi
	# Size of entries in boot_page_table1 is 4 bytes.
#	addl $4, %edt
	# Loop to the next entry if we haven't finished.
#	loop 1b

    # Map all adresses from 0-4MiB to virtual address 0xC0000000-0xC0400000
    movl $(boot_page_table1 - 0xC0000000), %edi
    movl $0, %esi
    movl $1023, %ecx
page_set_loop:
    movl %esi, %edx
    orl $3, %edx
    movl %edx, (%edi)
    addl $4, %edi
    addl $4096, %esi
    loop page_set_loop

    #movl $0xdeadbeef, (boot_page_table2 - 0xC0000000)

	# The page table is used at both page directory entry 0 (virtually from 0x0
	# to 0x3FFFFF) (thus identity mapping the kernel) and page directory entry
	# 768 (virtually from 0xC0000000 to 0xC03FFFFF) (thus mapping it in the
	# higher half). The kernel is identity mapped because enabling paging does
	# not change the next instruction, which continues to be physical. The CPU
	# would instead page fault if there was no identity mapping.

	# Map the page table to both virtual addresses 0x00000000 and 0xC0000000.
	;movl $(boot_page_table1 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 0
	movl $(boot_page_table1 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 768 * 4

	# Set cr3 to the address of the boot_page_directory.
	movl $(boot_page_directory - 0xC0000000), %ecx
	movl %ecx, %cr3

	# Enable paging and the write-protect bit.
	movl %cr0, %ecx
	orl $0x80010000, %ecx
	movl %ecx, %cr0

	# Jump to higher half with an absolute jump.
	lea 4f, %ecx
	jmp *%ecx

.section .text

4:
	# At this point, paging is fully set up and enabled.

	# Unmap the identity mapping as it is now unnecessary. 
	movl $0, boot_page_directory + 0

	# Reload crc3 to force a TLB flush so the changes to take effect.
	movl %cr3, %ecx
	movl %ecx, %cr3

    cli
	# Set up the stack.
	mov $stack_top, %esp
    movl $0x0, %ebp

	# Enter the high-level kernel, with the multiboot header passed as an
    # argument in the EBX register
    addl $0xC0000000, %ebx
    pushl %ebx
    pushl $(boot_page_table2)
	call kernel_main

	# Infinite loop if the system has nothing more to do.
	cli
1:	hlt
	jmp 1b
