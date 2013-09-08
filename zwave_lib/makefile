
GCC=gcc
LD=ld

CXXFLAGS=$(CFLAGS) -fPIC -Wall -O3 -fmessage-length=0 -rdynamic 
INCLUDES+=-I./inc
LIBS = -lsqlite3 -lpthread -lm -lrt

CMD_CLASSES = src/classes/bin_sw_cmd_class.c \
		src/classes/bin_sensor_cmd_class.c \
		src/classes/battery_cmd_class.c \
		src/classes/toggle_sw_cmd_class.c \
		src/classes/wake_up_cmd_class.c

LIB_SRCS = src/cmd_class.c \
		src/zw_node.c \
		src/zw_api.c \
		src/db_utils.c \
		src/log.c

LIB_SRCS += $(CMD_CLASSES) 
MAIN_SRC = src/main.c

%.o:%.c
	$(GCC) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

LIB_OBJS    := $(patsubst %.c, %.o, $(LIB_SRCS))
MAIN_OBJ    := $(patsubst %.c, %.o, $(MAIN_SRC))

all: lib exe

exe: $(LIB_OBJS) $(MAIN_OBJ)
	$(GCC) -o ../bin/zwave $(MAIN_OBJ) $(LIB_OBJS) $(LIBS)

lib: $(LIB_OBJS)
	$(GCC) -shared -fPIC -o ../lib/libzwave.so $(LIB_OBJS) $(LIBS)

clean:
	rm -f $(LIB_OBJS) $(MAIN_OBJ) ../lib/libzwave.so
