# $ 执行 shell 命令
# basename - strip directory and suffix from filenames
NAME := $(shell basename $(PWD))
# export使 MODULE 可以被子make进程使用
export MODULE := M1
# make 默认执行 all，依赖$(NAME)变量
all: $(NAME)
# include 包含另一个Makefile， .mk 是辅助 Makefile 文件
include ../.shadow/oslabs.mk

# 自定义目标 my
my:
	gcc -g -I../testkit/ -o labyrinth ./labyrinth.c
