// ===============
// Testing array.h
// ===============
//
// .. code-block:: cpp

#include "array.h"

#include <stdlib.h>

// Create the prototype for a integer list.
//
// .. code-block:: cpp

CA_PROTOTYPE(int)

// .. c:function::
int
main(
    int argc,
    char *argv[]
) 
//
//    The test function will trigger the bound check if called with 3+.
//
//    :param int argc: Argument count
//    :param char** argv: List of arguments
//
// .. code-block:: cpp

{
    int index = strtol(argv[1], NULL, 10);
    ca_int_t a;
    a = ca_new_int(3);
    CA(int, a, index) = 0;
    return CA(int, a, index);
}
