// ==============
// Encryption API
// ==============
// 
// .. code-block:: cpp

#ifndef ch_inc_encryption_h
#define ch_inc_encryption_h

#include "encryption.h"

// .. c:function::
extern
ch_error_t
ch_en_openssl_init(void);
//
//    Initialize openssl according to config.
//

// .. c:function::
extern
ch_error_t
ch_en_openssl_uninit(void);
//
//    Uninitialize openssl.
//

// .. c:function::
extern
void
ch_en_set_manual_openssl_init(void);
//
//    By default chirp will initialize openssl on the first instance of chirp
//    and uninitialize openssl on the last instance of chirp. If for some
//    reason the count of chirp instances can drop to zero sometimes and you
//    do not want openssl to get uninitialized you can manually call
//    :c:func:`ch_en_openssl_init` before creating the first chirp instance and
//    :c:func:`ch_en_openssl_uninit` after closing the last chirp instance.
//
//    You can also initialize openssl yourself if you have to.
//


#endif //ch_inc_encryption_h
