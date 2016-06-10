// ===============
// Protocol Header
// ===============
//
// .. code-block:: cpp

#ifndef ch_protocol_h
#define ch_protocol_h

#include "sglib.h"

#include <string.h>

// .. c:type:: ch_receipt_t
//
//    Protocl object.
//  
//    .. c:member:: TODO
//
// .. code-block:: cpp

typedef struct ch_receipt {
  unsigned char      receipt[16];
  char               color_field;
  struct ch_receipt* left;
  struct ch_receipt* right;
} ch_receipt_t;

#define CH_RECEIPT_CMP(x,y) \
    memcmp(x->receipt, y->receipt, 16)

SGLIB_DEFINE_RBTREE_PROTOTYPES(
    ch_receipt_t,
    left,
    right,
    color_field,
    CH_RECEIPT_CMP
);
SGLIB_DEFINE_RBTREE_FUNCTIONS(
    ch_receipt_t,
    left,
    right,
    color_field,
    CH_RECEIPT_CMP
);

// .. c:type:: ch_protocol_t
//
//    Protocol object.
//
//    .. c:member:: TODO
//
// .. code-block:: cpp

typedef struct {
} ch_protocol_t;

#endif //ch_protocol_h
