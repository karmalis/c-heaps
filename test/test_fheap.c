
#include "../src/fib_heap.h"

#include <stdio.h>
#include <time.h>
#include <errno.h>

/*
** Test graph
**
** 7----24----23----17----3
** |   / |              / | \
** 30 26 46           18 52 41
**     |               |     |
**    36              39    44
**
** Min is 3 and there are 3 marked nodes with keys: 26, 18, 39
**
** The initial heap will not have 17 node. Part of the test is insertion, but it should
** be at that position after insertion. This way it will be easier to track minimum
** extraction
**
 */
void test_simple_pop() {
    fibheap_node *n3_child_1 = create_node(
        18, 1, NULL, create_node(39, 1, NULL, NULL, NULL, NULL), NULL, NULL);

    fibheap_node *n3_child_2 = create_node(52, 0, NULL, NULL, NULL, n3_child_1);

    fibheap_node *n3_child_3 =
        create_node(41, 0, NULL, create_node(44, 0, NULL, NULL, NULL, NULL),
                    n3_child_1, n3_child_2);

    n3_child_1->next = n3_child_2;
    n3_child_1->prev = n3_child_3;
    n3_child_2->next = n3_child_3;

    fibheap_node *n3 = create_node(3, 0, NULL, n3_child_2, NULL, NULL);
    n3_child_1->parent = n3;
    n3_child_2->parent = n3;
    n3_child_3->parent = n3;

    fibheap_node *n23 = create_node(23, 0, NULL, NULL, n3, NULL);
    n3->prev = n23;

    fibheap_node *n24_child_1 = create_node(
        26, 1, NULL, create_node(35, 0, NULL, NULL, NULL, NULL), NULL, NULL);

    fibheap_node *n24_child_2 =
        create_node(46, 0, NULL, NULL, n24_child_1, n24_child_1);
    n24_child_1->prev = n24_child_2;
    n24_child_1->next = n24_child_2;

    fibheap_node *n24 = create_node(24, 0, NULL, n24_child_2, n23, NULL);
    n24_child_1->parent = n24;
    n23->prev = n24;

    fibheap_node *n7 = create_node(
        7, 0, NULL, create_node(30, 0, NULL, NULL, NULL, NULL), n24, n3);
    n3->next = n7;






}


int main(int argc, char** argv) {
    size_t n;
    char* endptr;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number>\n", argv[0]);
        return EXIT_FAILURE;
    }

    errno = 0;

    unsigned long ul = strtoul(argv[1], &endptr, 10);
    if (endptr == argv[1] || *endptr != '\0') {
        fprintf(stderr, "Invalid number format: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    n = (size_t)ul;

    srand(time(NULL));

    return EXIT_SUCCESS;
}
