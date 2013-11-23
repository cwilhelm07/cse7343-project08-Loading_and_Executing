# Compile and link kernel
rm -f kernel.o kernel_asm.o kernel bochsout.txt
as86 kernel.asm -o kernel_asm.o
bcc -ansi -c -o kernel.o kernel.c
ld86 -o kernel -d kernel.o kernel_asm.o
if [ ! -f kernel ]; then
	exit 1
fi

# Compile and link shell
rm -f shell.o lib_asm.o shell
as86 lib.asm -o lib.o
bcc -ansi -c -o shell.o shell.c
ld86 -o shell -d shell.o lib.o
if [ ! -f shell ]; then
	exit 1
fi

dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3
dd if=map.img of=floppya.img bs=512 count=1 seek=1 conv=notrunc
dd if=dir.img of=floppya.img bs=512 count=1 seek=2 conv=notrunc
./loadFile shell
./loadFile message.txt
./loadFile tstpr2
bochs -f opsys.bxrc