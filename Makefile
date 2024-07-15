C_SOURCES = $(wildcard kernel/*.c drivers/*.c font/*.c core/*.c shell/*.c \
	core/sys/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h font/*.h core/*.h shell/*.h \
	core/sys/*.h)
OBJ = ${C_SOURCES:.c=.o}
QEMU = qemu-system-x86_64

CC = i686-elf-gcc
GDB = i686-elf-gdb
CFLAGS = -g -O3 -Wall -pedantic -Wextra -nostdlib -Wno-stringop-overflow


os.bin: ${OBJ} boot/boots.o core/gdts.o core/utils.o core/idts.o core/isrs.o \
	core/syscalls.o
	${CC} -ffreestanding -nostdlib -T linker.ld -o $@ $^ -lgcc
	mkdir -p isodir/boot/grub
	cp os.bin isodir/boot/os.bin
	cp grub/grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o os.iso isodir

boot/boots.o: boot/boot.s
	i686-elf-as $^ -o $@

core/gdts.o: core/gdt.s
	nasm -felf32 $< -o $@

core/utils.o: core/util.s
	nasm -felf32 $< -o $@

core/idts.o: core/idt.s
	nasm -felf32 $< -o $@

core/isrs.o: core/isr.s
	nasm -felf32 $< -o $@

core/syscalls.o: core/syscall.s
	nasm -felf32 $< -o $@

run:
	${QEMU} -m 988M -cdrom os.iso -monitor stdio

%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

clean:
	rm -rf *.bin *.dis *.o os-image.bin *.elf
	rm -rf kernel/*.o boot/*.bin drivers/*.o boot/*.o core/*.o font/*.o \
	core/sys/*.o
	rm os.iso
