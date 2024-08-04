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

#include "../drivers/vga.h"
#include "../drivers/pic.h"

void kernel_init(Pagetable *pagetable, multiboot_info_t *mbi) {
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

    // initializes the file module and initializes stdio
    file_init();

    // initialize the system call interface
    syscall_init();
}

void dump_mem(char *mem, size_t length) {
    for (uint32_t i = 0; i < length; i++) {
        print_u32(*((uint32_t*)mem));
        printf(" ");
        mem += 4;
    }
}

void kernel_main(unsigned int boot_page_2, unsigned int ebx) {
    multiboot_info_t *mbinfo = (multiboot_info_t*)ebx;
    Pagetable *pagetable = (Pagetable*)boot_page_2;

    kernel_init(pagetable, mbinfo);

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
