
#include "../src/fib_heap.h"

#include <stdio.h>
#include <time.h>
#include <errno.h>

/*
** Test fib heap
**
** 17---24----23----7-----3
** |   /  |             / | \
** 30 26m 46          18m 52 41
**     |               |     |
**    36              39m    44
**
** Min is 3 and there are 3 marked nodes with keys: 26, 18, 39
**
** The initial heap will not have 17 node. Part of the test is insertion, but it should
** be at that position after insertion. This way it will be easier to track minimum
** extraction
**
 */

void basic_function_tests() {

    fibheap_node_t* n3 = create_node(3, 0);

    fibheap_node_t* n3_18m = create_node(18, 1);
    fibheap_node_t* n3_18m_39m = create_node(39, 1);
    add_node_child(n3_18m, n3_18m_39m);
    add_node_child(n3, n3_18m);

    fibheap_node_t* n3_52 = create_node(52, 0);
    add_node_child(n3, n3_52);

    fibheap_node_t* n3_41 = create_node(41, 0);
    fibheap_node_t* n3_41_44 = create_node(44, 0);
    add_node_child(n3_41, n3_41_44);
    add_node_child(n3, n3_41);

    fib_cb_t* root = create_circular_buffer(NULL);
    insert_cb_into(root, n3);

    fibheap_node_t* n7 = create_node(7, 0);
    insert_cb_into(root, n7);

    fibheap_node_t* n23 = create_node(23, 0);
    insert_cb_into(root, n23);

    fibheap_node_t* n24 = create_node(24, 0);
    fibheap_node_t* n24_26m = create_node(26, 1);
    add_node_child(n24, n24_26m);

    fibheap_node_t* n24_26m_36 = create_node(36, 0);
    add_node_child(n24_26m, n24_26m_36);

    fibheap_node_t* n46 = create_node(46, 0);
    add_node_child(n24, n46);

    insert_cb_into(root,n24);

    fibheap_node_t* n17 = create_node(17, 0);
    fibheap_node_t* n30 = create_node(30, 0);
    add_node_child(n17, n30);

    insert_cb_into(root, n17);

    fibheap_t* heap = create_heap_with_min(n3);

    insert_fib(heap, 21, 0);

    release_fib_heap(heap);
}
int main(int argc, char** argv) {
    basic_function_tests();

    return EXIT_SUCCESS;
}
