// =======
// Message
// =======
// 
// .. code-block:: cpp

#include "message.h"

#include <string.h>

// .. c:function::
void
ch_msg_init(ch_message_t* message)
//    :noindex:
//
//    see: :c:func:`ch_msg_init`
//
// .. code:: cpp

{
    memset(message, 0, sizeof(ch_message_t));
    message->host_order = 1;
}
