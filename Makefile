C_SOURCES = $(wildcard kernel/*.c drivers/*.c font/*.c core/*.c shell/*.c \
	core/sys/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h font/*.h core/*.h shell/*.h \
	core/sys/*.h)
OBJ = ${C_SOURCES:.c=.o}
QEMU = qemu-system-x86_64

CC = i686-elf-gcc
GDB = i686-elf-gdb
GNUAS=i686-elf-as
LD=i686-elf-ld
AS=nasm
CFLAGS = -g -Wall -pedantic -Wextra -nostdlib -Wno-stringop-overflow

os.iso: os.bin shell/shell.elf filesystem
	mkdir -p isodir/boot/grub
	mkdir -p isodir/modules
	cp os.bin isodir/boot/os.bin
	cp grub/grub.cfg isodir/boot/grub/grub.cfg
	cp filesystem/filesystem isodir/modules
	grub-mkrescue -o os.iso isodir

os.bin: ${OBJ} boot/boots.o core/gdts.o core/idts.o core/isrs.o \
	core/syscalls.o core/registerss.o core/asm_utilss.o core/stacks.o
	${CC} -ffreestanding -nostdlib -T linker.ld -o $@ $^ -lgcc

boot/boots.o: boot/boot.s
	${GNUAS} $^ -o $@

core/gdts.o: core/gdt.s
	${AS} -felf32 $< -o $@

core/idts.o: core/idt.s
	${AS} -felf32 $< -o $@

core/isrs.o: core/isr.s
	${AS} -felf32 $< -o $@

core/syscalls.o: core/syscall.s
	${AS} -felf32 $< -o $@

core/registerss.o: core/registers.s
	${AS} -felf32 $< -o $@

core/asm_utilss.o: core/asm_utils.s
	${AS} -felf32 $< -o $@

core/stacks.o: core/stack.s
	${AS} -felf32 $< -o $@

run:
	${QEMU} -m 64M -cdrom os.iso -monitor stdio

filesystem:
	mkdir -p filesystem
	cp shell/*.elf filesystem
	python3 scripts/buildfilesystem.py

shell/shell.elf: shell/shell.s
	${AS} -felf32 $< -o shell/shell.o
	${LD} shell/shell.o -o shell/shell.elf -Ttext=0x100000

%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

clean:
	rm -rf *.bin *.dis *.o os-image.bin *.elf
	rm -rf filesystem
	rm -rf kernel/*.o boot/*.bin drivers/*.o boot/*.o core/*.o font/*.o \
	core/sys/*.o shell/*.o shell/*.elf
	rm os.iso
