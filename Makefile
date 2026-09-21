CC = gcc
CFLAGS = -ansi -Wall -Wextra -Werror -pedantic-errors

symnmf: symnmf.o symnmf.h
	$(CC) -o symnmf symnmf.o -lm $(CFLAGS)
symnmf.o : symnmf.c
	$(CC) -c symnmf.c -lm $(CFLAGS)