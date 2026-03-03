
CC = gcc

SRC_FOLDERS = parser compiler

SRC = $(foreach dir,$(SRC_FOLDERS),$(wildcard $(dir)/*.c))

OBJS = $(patsubst %.c, obj/%.o, $(SRC))

all: csc.exe

setup:
	mkdir ..\obj
	mkdir ..\obj\compiler
	mkdir ..\obj\parser

obj/%.o:%.c
	$(CC) -c $< -o $@ \
	-Wall -Wextra -g \
	-Wpedantic -mno-red-zone

csc.exe: $(OBJS)
	$(CC) -g $^ -o $@
