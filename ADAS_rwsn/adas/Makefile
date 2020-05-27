#########################################################################
#usage:       make clean
#             make objs
#             make all
SHELL=/bin/sh

CC = $(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++
LD = $(CROSS_COMPILE)ld

#$(warning $(CC))

CFLAGS +=

LDFLAGS += -lpthread

MAKE_DIR=$(PWD)

CFLAGS += $(INCLS)
CXXFLAGS += $(INCLS)

VPATH=$(foreach dir,$(shell find . -type d),$(shell echo $(dir):))

INCLS=$(foreach dir,$(subst :, ,$(VPATH)),$(shell echo -I$(dir)))

C_SRC_PATH = $(foreach dir,$(subst :, ,$(VPATH)),$(wildcard $(dir)/*.c))
COBJS = $(subst .c,.o,$(C_SRC_PATH))

CPP_SRC_PATH = $(foreach dir,$(subst :, ,$(VPATH)),$(wildcard $(dir)/*.cc))
CPPOBJS = $(subst .cc,.o,$(CPP_SRC_PATH))

execobjs = $(COBJS) $(CPPOBJS)
###### generic rules #######
.PHONY: all

all: $(execobjs)
	$(CXX) -o ADAS.rwsn $(execobjs) $(LDFLAGS)

$(COBJS):%.o:%.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(CPPOBJS):%.o:%.cc
	$(CC) -c $< -o $@ $(CXXFLAGS) 

.PHONY: clean

clean:
	rm hwacc $(execobjs)

install:
	cp hwacc $(PREFIX)/bin/
