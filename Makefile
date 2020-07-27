CC=gcc
CCOPTS=--std=gnu99 -Wall -D_LIST_DEBUG_ -g
AR=ar

OBJS=bit_map.o\
     buddy_allocator.o

HEADERS=bit_map.h buddy_allocator.h

LIBS=libbuddy.a

BINS=test1 test2 test3 test4 test5 test6 test7 test8 test9

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

test4: test4.o $(LIBS)
	$(CC) $(CCOPTS) -o $@ $^ -lm


test5: test5.o $(LIBS)
	$(CC) $(CCOPTS) -o $@ $^ -lm

test6: test6.o $(LIBS)
	$(CC) $(CCOPTS) -o $@ $^ -lm


test7: test7.o $(LIBS)
	$(CC) $(CCOPTS) -o $@ $^ -lm


test8: test8.o $(LIBS)
	$(CC) $(CCOPTS) -o $@ $^ -lm


test9: test9.o $(LIBS)
	$(CC) $(CCOPTS) -o $@ $^ -lm



clean:
	rm -rf *.o *~ $(LIBS) $(BINS)
