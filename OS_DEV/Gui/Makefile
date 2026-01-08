flags = -fno-stack-protector -Wall -O0 -std=gnu23 -m16 -ffreestanding -I include -fno-pic -fno-PIE
ldflags = --nmagic --script=gui.ld --oformat binary
objects = main.o shapes.o bmp.o omnistd.o
xobjects = xgfx.o
NASMENV = -i include/asm/
export NASMENV

.PHONY: clean

all: gui.com unclutter

gui.com: $(objects) $(xobjects)
	ld -m elf_i386 $(ldflags) $^ -o $@

main.o: main.c
	gcc $(flags) -c $^

omnistd.o: omnistd.c
	gcc $(flags) -c $^

shapes.o: shapes.c
	gcc $(flags) -c $^

bmp.o: bmp.c
	gcc $(flags) -c $^

xgfx.o: xgfx.asm
	nasm -f elf $^ -o $@
unclutter:
	rm -f *.o
clean:
	rm -f *.o *.com