# NAME 变量
# $(shell ) 执行shell命令并捕获输出
NAME := $(shell basename $(PWD))
# export 在当前的shell下设置环境变量
export MODULE := M2
# all 为make默认执行的目标
all: $(NAME)

include ../.shadow/oslabs.mk
