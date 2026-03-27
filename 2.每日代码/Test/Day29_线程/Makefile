SRCS := $(wildcard *.c)
OUTS := $(patsubst %.c,%,$(SRCS))
CC := gcc
COM_OP := -Wall -g 
.PHONY: clean rebuild all

all: $(OUTS)                      

% : %.c
	$(CC) $^ -o $@ $(COM_OP) 
# 一定要是一个完整的Tab
clean:
	$(RM) $(OUTS) 
# 同理
rebuild: clean all
