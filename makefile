# Began implementing a build mechanism for making fat binaries on
# the Macintosh supporting both  x86 and ARM architectures. But I
# gave up.

UNAME_S := $(shell uname -s)
MACFLAGS = 

ifeq ($(UNAME_S),Darwin)
	MACFLAGS += -D OSX
	CC = clang++
else
	CC = g++
endif

CFLAGS	= -Wall -Werror --pedantic -std=c++11 -g $(MACFLAGS)
COBJS	= client.o
srcs	= $(wildcard *.cpp)
deps	= $(srcs:.cpp=.d)

all		: client

client	: 	$(COBJS)
		$(CC) -lncurses -o $@ $(COBJS)

%.o: %.cpp
		$(CC) -MMD -MP $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
		$(RM) $(COBJS) $(SOBJS) $(deps) a.out core client
		rm -rf *.dSYM

-include $(deps)