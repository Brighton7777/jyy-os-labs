NAME := $(shell basename $(PWD))
export MODULE := M4
CFLAGS += -ldl

all: $(NAME)

test: all
	TK_VERBOSE=1 ./$(NAME)

include ../.shadow/oslabs.mk

.PHONY: all test