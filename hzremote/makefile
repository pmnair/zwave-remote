
GCC=gcc
LD=ld

CXXFLAGS=$(CFLAGS) -Wall -O3 -fmessage-length=0 -rdynamic 
INCLUDES+=-I./inc -I../zwave_lib/inc -I/usr/include/libxml2
LIBS = -L../lib -Wl,--rpath -Wl,../ -lzwave -lpthread -lm -lrt -lxmlrpc_server_abyss -lxmlrpc_server -lxmlrpc_abyss -lxmlrpc -lxmlrpc_util -lxml2

SRCS = src/main.c \
	src/xmlrpc-methods.c \
	src/xmlrpc-utils.c \
	src/xmlconfig.c

%.o:%.c
	$(GCC) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

OBJS    := $(patsubst %.c, %.o, $(SRCS))

all: $(OBJS)
	$(GCC) -o ../bin/hzremote $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS)
