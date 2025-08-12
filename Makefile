RAYLIB   ?= /usr/lib/raylib-5.5_linux_amd64
COMPILER ?= gcc
CFLAGS   ?= -Wall -Wextra -std=c11 -I$(RAYLIB)/include -g -fsanitize=address,undefined
LDFLAGS  ?= -L$(RAYLIB)/lib -lraylib -lm -ldl -lpthread

all: program

program: main.c
	$(COMPILER) $(CFLAGS) main.c -o out $(LDFLAGS)

run: program
	@./out

clean:
	@rm -f out

.PHONY: all run clean
