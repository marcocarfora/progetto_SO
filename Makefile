CC=gcc
CCOPTS=--std=gnu99 -Wall -D_LIST_DEBUG_ 
AR=ar

OBJS=bit_map.o\
     buddy_allocator.o

HEADERS=bit_map.h buddy_allocator.h

LIBS=libbuddy.a

BINS=test1 test2 test3

.phony: clean all


all:	$(LIBS) $(BINS)

%.o:	%.c $(HEADERS)
	$(CC) $(CCOPTS) -c -o $@  $<

libbuddy.a: $(OBJS) 
	$(AR) -rcs $@ $^
	$(RM) $(OBJS)

test1: test1.o $(LIBS)
	$(CC) $(CCOPTS) -o $@ $^ -lm

test2: test2.o $(LIBS)
	$(CC) $(CCOPTS) -o $@ $^ -lm

test3: test3.o $(LIBS)
	$(CC) $(CCOPTS) -o $@ $^ -lm

clean:
	rm -rf *.o *~ $(LIBS) $(BINS)
