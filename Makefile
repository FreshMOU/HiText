# Hisilicon Hi35xx sample Makefile
include $(PWD)/../Makefile.param

CFLAGS += -I$(PWD)/sample
CFLAGS += -I$(PWD)/sample_nnie_software
CFLAGS += -I$(PWD)/conv
CFLAGS += -O3 -fopenmp

SRCS := $(wildcard *.c)
SRCS += $(wildcard ./sample/*.c)
SRCS += $(wildcard ./sample_nnie_software/*.c)
SRCS += $(wildcard $(PWD)/../common/*.c)
SRCS += $(wildcard $(PWD)/conv/*.c)
TARGET := libhitbpp.a

# compile linux or HuaweiLite
# target source
OBJS  := $(SRCS:%.c=%.o)

.PHONY : clean all

all: $(TARGET)

$(TARGET):$(COMM_OBJ) $(OBJS) 
	@$(AR) cru $@ $(OBJS) $(COMM_OBJ)


clean:
	@rm -f $(TARGET)
	@rm -f $(OBJS)
	@rm -f $(COMM_OBJ)
