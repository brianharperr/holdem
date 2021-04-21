# Builds static library [libtex.so] and console executable [texasholdem]
CC = gcc
CFLAGS = -fPIC -Os -g
LDFLAGS = -shared
RM = rm -f
TARGET_LIB = libtex.so
TARGET_EXE = texasholdem 

SRCS = game.c cards.c pots.c evaluator.c mtwister.c 
OBJS = $(SRCS:.c=.o)

.PHONY: all
all: ${TARGET_LIB} ${TARGET_EXE}

$(TARGET_LIB): $(OBJS)
	$(CC) ${LDFLAGS} -o $@ $^

$(SRCS:.c=.d):%.d:%.c
	$(CC) $(CFLAGS) -MM $< >$@

$(TARGET_EXE): $(OBJS)
	$(CC) -o holdem texasholdem.c $(SRCS)

include $(SRCS:.c=.d)

.PHONY: clean
clean:
	-${RM} ${OBJS} $(SRCS:.c=.d) 
