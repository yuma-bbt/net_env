CPPFLAGS= -I/usr/include/
CC = gcc

forward: forward.c

test: forward
	./test.sh
run: forward
	$(CC) -o forward  $(CPPFLAGS)  ./forward.c
	chmod +x forward
	sudo ip netns exec router ./forward
clean:
	rm -f forward *.o *~ tmp*

.PHONY: test clean
