#include "array.h"

// Testcode

CA_PROTOTYPE(int)

int
main(
    int argc,
    char *argv[]
) {
    ca_int_t a;
    a = ca_new_int(3);
    CA(int, a, 3) = 2;
    return CA(int, a, 3);
}

