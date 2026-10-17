
CC	= m68k-xelf-gcc
AR	= m68k-xelf-ar

#CFLAGS = -Wall -g -Os -c
CFLAGS	= -Wall -Os -c

.c.o:
	$(CC) $(CFLAGS) $<

SRC	=	_spawn.c \
		spawnl.c spawnle.c spawnlp.c \
		spawnv.c spawnve.c spawnvp.c

OBJ     = $(SRC:.c=.o)

TARGET	= libspawn.a

all: $(TARGET) test1.x test2.x

$(TARGET): $(OBJ)
	$(AR) -r $(TARGET) $(OBJ)

test1.x: test1.c libspawn.a
	m68k-xelf-gcc -Wall -Os -o test1.x test1.c libspawn.a -specs=hupair.specs

test2.x: test2.c
	m68k-xelf-gcc -Wall -Os -o test2.x test2.c -specs=hupair.specs

clean:
	rm -f $(OBJ) $(TARGET) test1.x test2.x *.elf

.PHONY: all clean
