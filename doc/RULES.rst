=====
RULES
=====

* Types end in _t
* Callbacks end in _cb
  - Callbacks are in past tense? (not sure)
* Callback types end in _cb_t
* Always unpack handles in callbacks till you can verify the chirp magic
* C4irp and C3irp are only used to disambiguate the python-only and the C based
  version of chirp, everywhere else chirp is used
* Embeddable: no memory allocation
* Every function returns ch_error_t
* Use pointers sparsely
  
  - Copy small structs 
  - Use pointers for large structs (with const)
  - Use pointer if it has to be modified (also out params)

* Embrace libuv styles and use it for c4irp API
* Literate programming
* Reader and write may not be linked by a lock. Since the send function waits
  for an ACK, this means a second send may start when the first message is sent
  out but the ACK has not be received yet (means batching is possible)
* Local messages are sent to scheduler directly
* Localhost connections bypass TLS and don't use cleanup

  - Therefore no retry etc is needed! -> speed speed speed

* Binding will send local messages to scheduler directly (not using c4irp)
* Use C99 plus the extension used by libuv
* PEP8 style in C is ok
* Sending messages my not allocate memory

  - Only bookkeeping may allocate memory when a connection is open/closed
  - Only things that happen seldom may allocate
  - Luckily chirp is already design that way

* The original chirp API may only be slightly changed
* It must be possible for original chirp to adapt the new wire protocol

  - So we have a pure-python and C implementation

* Static linking rules
* Always check coverage and scan-build
* The development build system may be hacky

  - BUT the release build system (make.release, setup.py) must be rock solid

* Provide wheels
* Provide distro packages

Performance
===========

* Adding buffering per connection would destroy some of the nice properties of
  chirp, mainly flow-control, simpleness and robustness.

   * Therefore we do not ever allow to remove the per connection send-lock,
     which means only one message can be sending and the next message can only
     be sent after the current message has been acknowledged. The first
     statement is important for simpleness and robustness and the second
     statement makes flow-control possible.

* Since chirp is meant for multiprocessing, our performance goals refer to this
  configuration

   * x must be able to send/receive 300'000+ message to/from a suitable N peers

.. graphviz::

   digraph FAST {
      concentrate=true;
      x -> a;
      x -> b;
      x -> c;
      x -> d;
      x -> e;
      x -> f;
      x -> N;
      a -> x;
      b -> x;
      c -> x;
      d -> x;
      e -> x;
      f -> x;
      N -> x;
   }

* For this configuration we just have to beat 10'000 messages of course the
  more the better

.. graphviz::

   digraph FAST {
      concentrate=true;
      x -> a;
      a -> x;
   }

* Of course 300'000 msg is our stretch goal, 30'000 msg is ok too

=========
Questions
=========

Not rules yet.

* Python binding: Should we ask python for memory?
