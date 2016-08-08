#ifndef __RBEXT_LMFAO_HPP__
#define __RBEXT_LMFAO_HPP__

/*
   Problem:

   You cannot call Ruby functions from a thread not created by Ruby. When it comes to MRI, you
   must be the owner of the GVL to call ruby functions, and since the thread is not a
   ruby-created thread you have no way of owning the GVL.

   Workaround:

   1. we have a special ruby thread, waiting to be notified
   2. when C callback is invoked, it stores its’ parameters somewhere, notifies ruby thread and waits
   3. ruby thread is notified, reads the callback parameters, and executes the callback handler
   4. ruby thread puts the return value of the handler in location where C callback can reach it, and notifies C callback
   5. C callback wakes up again, reads return value and returns it

   Reference:

   * http://www.ruby-forum.com/topic/3149838
   * http://www.burgestrand.se/articles/asynchronous-callbacks-in-ruby-c-extensions
*/

#include <rice/Module.hpp>
#include <ons/Message.h>
#include <ruby/thread.h>
#include <pthread.h>

typedef struct listener_consume_args_t listener_consume_args_t;
struct listener_consume_args_t {
  VALUE handler;
  ons::Message *message;
};

typedef struct callback_t callback_t;
struct callback_t {
  /*
     Each callback needs to store its’ data somewhere; this
     is how we later on access that data from our Ruby thread.

     We also use this for our return value from the Ruby handler.
  */
  void *data;

  /*
     Once we’ve dispatched our callback data to Ruby, we must
     wait for a reply before we continue. These two are used
     for that purpose.
  */
  pthread_mutex_t mutex;
  pthread_cond_t  cond;

  /*
     Even though we use the condition variable above to wait,
     we might still be woken up (spurious wakeups). This bool
     serves as a final check that tells us if we can continue.
  */
  bool handled;

  /*
     We use this to implement a linked list of callback data.
     This allows multiple callbacks to happen simultaneously
     without them having to wait for each other.
  */
  callback_t *next;
};

typedef struct callback_waiting_t callback_waiting_t;
struct callback_waiting_t {
  callback_t *callback;
  bool abort;
};

/* * * * * * * * * * * * * * * * * * * * * * * * *
 * Functions related to the global callback queue
 * * * * * * * * * * * * * * * * * * * * * * * * */
void g_callback_queue_push(callback_t *callback);
callback_t *g_callback_queue_pop();

/* * * * * * * * * * * * * * * * * * * * * * * * *
 * Functions related to LMFAO Ruby API
 * * * * * * * * * * * * * * * * * * * * * * * * */
bool mLMFAO_call(VALUE handler, ons::Message* message);
void *lmfao_callback(void *data);

/* * * * * * * * * * * * * * * * * * * * * * * * *
 * Our special Ruby event-listening thread functions
 * * * * * * * * * * * * * * * * * * * * * * * * */
VALUE LMFAO_handle_callback(void *cb);
void *wait_for_callback_signal(void *w);
void stop_waiting_for_callback_signal(void *w);
VALUE LMFAO_event_thread(void *unused);

/* * * * * * * * * * * * * * * * * * * * * * * * *
 * Ruby bindings for LMFAO C library
 * * * * * * * * * * * * * * * * * * * * * * * * */
Rice::Object start_event_thread();
void define_module_lmfao_under_module(Rice::Module module);

#endif  // __RBEXT_LMFAO_HPP__
