#include "../core/format.h"
#include "../core/gdt.h"
#include "../core/multiboot.h"
#include "../core/physical_memory_manager.h"
#include "../core/paging.h"
#include "../core/virtual_memory_manager.h"
#include "../core/allocator.h"
#include "../core/idt.h"
#include "../core/isr.h"
#include "../core/syscall.h"
#include "../core/file.h"
#include "../core/mb_module.h"
#include "../core/kernfs.h"
#include "../core/process.h"
#include "../core/stack.h"
#include "../core/elf.h"

#include "../drivers/vga.h"
#include "../drivers/pic.h"

static Pagetable *pagetable;
static multiboot_info_t *mbi;

// initialization of lower level kernel functions (gdt, drivers, etc.)
void kernel_init_low_level() {
    // updates the global descriptor table to set up the permissions for
    // different segments of memory
    gdt_install_gdt();

    // initializes the formatter used for outputting to the console
    format_init();

    // initializes the VGA driver by setting the VGA registers, palettes etc.
    driver_vga_init();

    // clear the screen and set the background colour to grey
    driver_vga_clear(0x8);
    printf("\x98");

    // initialize the PIC
    pic_init();

    // initialize memory managers
    pmm_find_usable_memory(mbi);
    paging_init(&pmm, pagetable);
    pmm_init();

    // initialize the global memory allocator
    allocator_init();

    // initialize interrupts
    idt_init();
    isr_init();
    idt_begin();
}

// the stack allocation must be its own function since we dont want to clobber
// any variables we may need
void kernel_stack_relocate() {
    // allocate the new stack
    stack_relocate();
}

// initialization for higher level kernel functions
void kernel_init_high_level() {
    // initializes the file module and initializes stdio
    file_init();

    // initialize the system call interface
    syscall_init();

    // initialize the kernel-level filesystem
    multiboot_module_t *filesystem_root = module_get(mbi, KERNFS_ROOT_INDEX);
    kernfs_init(filesystem_root);

    // initialize process execution module
    process_init();
}

// checks the minimum requirements of the operating system versus the hardware
// present in the system that it is being run on
void kernel_check_minimum_requirements() {
    if (pmm_get_total_memory() < KERNEL_MINIMUM_RAM) {
        kpanic("Minimum requirements not met: Not enough RAM (<32MiB)");
    }
}

void dump_mem(char *mem, size_t length) {
    for (uint32_t i = 0; i < length; i++) {
        print_u32(*((uint32_t*)mem));
        printf(" ");
        mem += 4;
    }
}

void shell_load() {
    File *f = kernfs_open("shell.elf");
    ELF32 shell;
    elf_read(file_get_buffer(f), &shell);
    //print_u32(shell.section_headers[1].sh_size);
    process_spawn(&shell, file_get_buffer(f));
    elf_close(&shell);
}

void kernel_main(unsigned int boot_page_2, unsigned int ebx) {
    mbi = (multiboot_info_t*)ebx;
    pagetable = (Pagetable*)boot_page_2;

    kernel_init_low_level();
    kernel_stack_relocate();
    kernel_init_high_level();

    kernel_check_minimum_requirements();

    shell_load();
    //while (1) {}

    //process_spawn(f);
    //printf("Loaded process!\n");

    //printf("\x88\x9c""FATAL EXCEPTION: Page fault (0x0000000D)");
    //printf("\x88\x9c""\n%esp = 0xC00D198E");

    //file_write(stdout, "ABC", 8);
    //kpanic("Not sigma");
/*
    printf("    @@@                 @@@@     \n");
    printf("   @++#@@@            @@+##@     \n");
    printf("   @+#,,++@@@     @@@@++,,,#@    \n");
    printf("   @#,,,+++++@@@@@+++++,,,,#@    \n");
    printf("   @#,,,,++++++++++++++#,,,+@    \n");
    printf("   @+,,,,,++++++++++++++,,#,@    \n");
    printf("   @+,,,#+++++++++++++++###,@    \n");
    printf("    +,,#+++++++++++++++++++,     \n");
    printf("    @++++++@@@@++++++@@@++++     \n");
    printf("     @++++++@@@@++++@@@++++++    \n");
    printf("    @+++++@@,,,,++++,,,,,@@++    \n");
    printf("    +++++@,,,,,,,,,,,,,,,,,@++   \n");
    printf("    ++++@,,,,,,,,,,,,,,,,,,,@+   \n");
    printf("   ++++@,,,,,,,,,,,,,,,   ,,,@+  \n");
    printf("  +++++@,,,,   ,,,,,@@,   ,,,@++ \n");
    printf("  +++++@,,,,   ,@@   @@   ,,,@++ \n");
    printf("  +++++@,,,,   @@@@ @@@@++,,,@@+ \n");
    printf("  +@+++@,,,,+@@@ @ @ @ @@@,,@@@+ \n");
    printf("  #@@+@@@,,,@@@@@ @@@ @@@@@@@@++ \n");
    printf("   @@@@@@@@@@@@@@@@@@@@@@@@@@@#  \n");
    printf("    @@@@@@@@@@@@@@@@@@@@@@@@@@   \n");
    printf("    @@@@@@@@@@@@@@@@@@@@@@@@@@   \n");
    printf("     @@@@@@@@@@@@@@@@@@@@@@@@    \n");
    printf("      @@@@@@@@@@@@@@@@@@@@@@     \n");
    printf("       ++++@@@@@@@@@@@@@@        \n");
    printf("        +++++@@@@@@@@@@@  +++    \n");
    printf("        ++++++++@@@@@@+++++++    \n");
    printf("       ++++#+++++@@@+++++#++#    \n");
    printf("       #+##++++++++++++##+++#    \n");
    printf("       #+++    ++++++++  +++#    \n");
    printf("       #++++++ +++@@@++::::::    \n");
    printf("       ##++++++ +   :::::::::#   \n");
    printf("      # ##++++++  :::::::::::##  \n");
    printf("      ##  ##++++  :::::::::::##  \n");
    printf("    , ####  ##++  ::::,,:::::+#  \n");
    printf("   ,, ###++#  , + ::::,,:::::++# \n");
    printf("  #,, ###+++#  + :::::::::::+++# \n");
    printf("  +#, ##+++  ::: :::::::::::++## \n");
    printf(" +++# ###+++  :: :::::::::::++#  \n");
    printf(" +,,, ,#+++++  : :::::      ++#  \n");
    printf(" ,,,, ,+++++++        +, ++++#,  \n");
    printf(" ,,+## ,,++++,,,@ +@@+  ,@,+##   \n");
    printf("  ++##,  ,,,+,@,,, ++, @,,,##,   \n");
    printf("  +++,,,#  ,,,,@@, ,,, ,@@,,,    \n");
    printf("   ,,,,,##    ,@@       @@,      \n");
    printf("    ,,,#+                        \n");
    printf("      +++                        \n");
    printf("                                 \n\n");*/
    klock();
}
