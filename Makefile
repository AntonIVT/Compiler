NASMFLAGS = -f elf64
CFLAGS    = -O0
CDEBUGFLAGS = -g  -fsanitize=address -fsanitize=alignment -fsanitize=bool -fsanitize=bounds -fsanitize=enum -fsanitize=float-cast-overflow -fsanitize=float-divide-by-zero -fsanitize=integer-divide-by-zero -fsanitize=leak -fsanitize=nonnull-attribute -fsanitize=null -fsanitize=object-size -fsanitize=return -fsanitize=returns-nonnull-attribute -fsanitize=shift -fsanitize=signed-integer-overflow -fsanitize=undefined -fsanitize=unreachable -fsanitize=vla-bound -fsanitize=vptr

lib:
	nasm $(NASMFLAGS) stdoors/stdoors.asm -o stdoors/stdoors.o
	ld stdoors/stdoors.o -o stdoors/stdoors
	rm stdoors/stdoors.o

compiler: lib
	g++ -o udc main.cpp

debug:
	g++ $(CDEBUGFLAGS) -o udc_debug main.cpp
	mv udc_debug bin
