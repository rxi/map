CC = gcc
CXX = g++
LINK = g++

CFLAGS_D = $(COMPILER_FLAGS) -c -g -O0 -Wall -Werror
CFLAGS_R = $(COMPILER_FLAGS) -c -g -O2  -Werror
CXXFLAGS_D = $(COMPILER_FLAGS) -c -g -O0  -Werror
CXXFLAGS_R = $(COMPILER_FLAGS) -c -g -O2  -Werror

INC+= -I. -I../../src
LIBS +=
RELEASE = 0

TARGET = c_test
CCFLAGS = $(CFLAGS_R)

ifeq ($(RELEASE),0)
    # debug
	CFLAGS = $(CFLAGS_D)
	CXXFLAGS = $(CXXFLAGS_D)
else
    # release
	CFLAGS = $(CFLAGS_R)
	CXXFLAGS = $(CXXFLAGS_R)
endif


CFILES = ../../src/map.c  main.c

OBJFILE = $(CFILES:.c=.o) $(CXXFILES:.cpp=.o)
all:$(TARGET)


$(TARGET): $(OBJFILE)
	$(LINK) $^ $(LIBS) -Wall -o $@

%.o: %.c
	$(CC) -o $@ $(CCFLAGS) $< $(INC)

%.o: %.cpp
	 $(CXX) -o $@ $(CXXFLAGS) $< $(INC)

clean:
	rm -f *.o *~ httpsniff
	rm -rf $(TARGET)
	rm -rf $(OBJFILE)
