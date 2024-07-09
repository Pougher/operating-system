#include "../core/format.h"
#include "../core/gdt.h"
#include "../core/multiboot.h"
#include "../core/physical_memory_manager.h"
#include "../core/paging.h"
#include "../core/virtual_memory_manager.h"
#include "../core/allocator.h"

#include "../drivers/vga.h"

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

    // initialize memory managers
    pmm_find_usable_memory(mbi);
    paging_init(&pmm, pagetable);
    pmm_init();

    // initialize the global memory allocator
    //allocator_init();
}

void test_malloc() {
    char *mem = kmalloc(400);
    mem = kmalloc(5000);
    print_u32(allocator_get_allocated());
    printf("\n");
    kfree(mem);
    print_u32(allocator_get_allocated());
    printf("\n");

    for (uint32_t i = 0; i < 10; i++) {
        uint32_t x = (((uint32_t*)allocator_get_heap())[i]);
        if (x != 0) {
            printf("\x8e");
            print_u32(x);
            printf("\x8f");
        } else {
            print_u32(x);
        }
        printf(" ");
    }
    printf("\n");
}

void kernel_main(unsigned int boot_page_2, unsigned int ebx) {
    multiboot_info_t *mbinfo = (multiboot_info_t*)ebx;
    Pagetable *pagetable = (Pagetable*)boot_page_2;

    kernel_init(pagetable, mbinfo);
    //test_malloc();
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
    while(1){
    }
}
