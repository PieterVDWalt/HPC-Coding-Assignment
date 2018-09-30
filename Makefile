all: serial.c
	gcc -o compiled serial.c -lm
clean:
	$(RM) compiled
