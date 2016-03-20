=======
Message
=======

.. code-block:: cpp

   #include "message.h"

   #include <string.h>

   void
   ch_msg_init(ch_message_t* message) {
       memset(message, 0, sizeof(ch_message_t));
       message->host_order = 1;
   }
