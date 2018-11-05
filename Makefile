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
include $(PWD)/../../../$(OSTYPE).mak
