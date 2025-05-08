##
# QueueC
#
# @file
# @version 0.1
#

build_test_bin_heap: test/test_binary_heap.c
	gcc -O3 -DNDEBUG -std=gnu11 -flto -lrt -o bin/queuec_heap test/test_binary_heap.c src/heap.c -I.

build_test_fib_heap: test/test_fheap.c
	gcc -ggdb -std=gnu11 -flto -lrt -o bin/queuec_fibh test/test_fheap.c src/fib_heap.c -I.

clean:
	rm -rf ./bin/*

# end
