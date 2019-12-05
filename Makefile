CC         = gcc
CFLAGS     = -fPIC -Wall -Wextra
LDFLAGS    = -shared
RM         = rm -f
NAME_LIB   = map
TARGET_LIB = lib$(NAME_LIB).so

SRCS = src/map.c
TEST_SRCS = src/test.c
TEST_OUTPUT = test
OBJS = $(SRCS:.c=.o)

.PHONY: all
all: $(TARGET_LIB)

$(TARGET_LIB): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: clean
clean:
	-$(RM) $(TARGET_LIB) $(OBJS) $(TEST_OUTPUT)

test:
	-$(CC) $(TEST_SRCS) -l$(NAME_LIB) -Wl,-rpath,. -o $(TEST_OUTPUT) -L.
