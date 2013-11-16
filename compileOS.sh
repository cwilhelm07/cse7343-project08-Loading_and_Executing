rm -f kernel.o kernel_asm.o
as86 kernel.asm -o kernel_asm.o
bcc -ansi -c -o kernel.o kernel.c
ld86 -o kernel -d kernel.o kernel_asm.o
if [ ! -f kernel ]; then
	exit 1
fi
dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3
dd if=message.txt of=floppya.img bs=512 count=1 seek=30 conv=notrunc
bochs -f opsys.bxrc