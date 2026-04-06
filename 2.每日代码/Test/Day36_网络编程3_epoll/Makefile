SRCS := $(wildcard *.c)
OUTS := $(patsubst %.c,%,$(SRCS))
CC := gcc
# 编译选项
COM_OP := -Wall -g
# 链接选项
LIBS := -pthread
.PHONY: clean rebuild all

all: $(OUTS)                      

% : %.c
	$(CC) $^ -o $@ $(COM_OP) $(LIBS) 
# 一定要是一个完整的Tab
clean:
	$(RM) $(OUTS) 
# 同理
rebuild: clean all
