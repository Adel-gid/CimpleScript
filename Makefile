
CC = gcc

SRC_FOLDERS = parser interpreter

SRC = $(foreach dir,$(SRC_FOLDERS),$(wildcard $(dir)/*.c))

OBJS = $(patsubst %.c, obj/%.o, $(SRC))

all: csi.exe

obj/%.o:%.c
	$(CC) -c $< -o $@ \
	-Wall -Wextra -g \
	-Wpedantic -mno-red-zone

csi.exe: $(OBJS)
	$(CC) -g $^ -o $@
