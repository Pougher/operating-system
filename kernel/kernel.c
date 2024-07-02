#include "../core/format.h"
#include "../core/gdt.h"
#include "../core/multiboot.h"
#include "../core/physical_memory_manager.h"
#include "../core/paging.h"
#include "../core/virtual_memory_manager.h"

#include "../drivers/vga.h"

void kernel_init(Pagetable *pagetable, multiboot_info_t *mbi) {
    // integral functions to the operation of the system
    gdt_install_gdt();

    // functions for the operation of the kernel
    format_init();
    driver_vga_init();

    // clear the screen and set the background colour to grey
    driver_vga_clear(0x8);
    printf("\x98");

    // initialize memory managers
    pmm_find_usable_memory(mbi);
    paging_init(&pmm, pagetable);

    pmm_init();
}

void test_vmap() {
    printf("\x8aTesting memory allocation\x8f\n");
    uint32_t *arr = vmm_map_memory((void*)0x00010000, 32 * sizeof(uint32_t));
    uint32_t *arr2 = vmm_map_memory((void*)0x00000000, 32 * sizeof(uint32_t));
    arr[32] = 0x69;
    arr2[32] = 0x42;
    print_u32(arr[32]);
    printf("\n\n\n");
}

void kernel_main(unsigned int boot_page_2, unsigned int ebx) {
    multiboot_info_t *mbinfo = (multiboot_info_t*)ebx;
    Pagetable *pagetable = (Pagetable*)boot_page_2;

    kernel_init(pagetable, mbinfo);

    //printf("\x98\x87// program to output 'Hello, world!'\n");
    //printf("\x8e""int\x8a" " main\x8f() {\n");
    //printf("\x8e    char\x8f *\x83message\x8f = \x8a\"Hello, world!\"\x8f;\n\n");
    //printf("\x8a    printf\x8f(\x8a\"%s\"\x8f,\x83 message\x8f);\n");
    //printf("    \x8creturn\x8d 0\x8f;\n}");

    test_vmap();

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
printf("  +++++@,,,,   ,@@...@@   ,,,@++ \n");
printf("  +++++@,,,,   @@@@.@@@@++,,,@@+ \n");
printf("  +@+++@,,,,+@@@.@.@.@.@@@,,@@@+ \n");
printf("  #@@+@@@,,,@@@@@.@@@.@@@@@@@@++ \n");
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
