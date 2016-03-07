#include "array.h"

#include <stdlib.h>

// Testcode

CA_PROTOTYPE(int)

int
main(
    int argc,
    char *argv[]
) {
    int index = strtol(argv[1], NULL, 10);
    ca_int_t a;
    a = ca_new_int(3);
    CA(int, a, index) = 0;
    return CA(int, a, index);
}

