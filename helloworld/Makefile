
PROJECT_NAME=hello_world
CC_PATH=/opt/riscv/bin/

SOURCES=main.c boot.s

all:
	${CC_PATH}/riscv32-unknown-elf-gcc ${SOURCES} -o ${PROJECT_NAME}.out -march=rv32i -Wl,-T littlerisc.ld
	${CC_PATH}/riscv32-unknown-elf-objcopy -O binary ${PROJECT_NAME}.out ${PROJECT_NAME}.bin
	#rm *.out
	
