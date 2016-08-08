#include "lmfao.hpp"

/* * * * * * * * * * * * * * * * * * * * * * * * *
 * Functions related to the global callback queue
 * * * * * * * * * * * * * * * * * * * * * * * * */

/*
   Three globals to allow for Ruby/C-thread communication:

   - mutex & condition to synchronize access to callback_queue
   - callback_queue to store actual callback data in

   Be careful with the functions that manipulate the callback
   queue; they must do so in the protection of a mutex.
*/
pthread_mutex_t g_callback_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_callback_cond   = PTHREAD_COND_INITIALIZER;
callback_t *g_callback_queue     = NULL;

/*
   Use this function to add a callback node onto the global
   callback queue.

   Do note that we are adding items to the front of the linked
   list, and as such events will always be handled by most recent
   first. To remedy this, add to the end of the queue instead.
*/
void g_callback_queue_push(callback_t *callback)
{
  callback->next   = g_callback_queue;
  g_callback_queue = callback;
}

/*
   Use this function to pop off a callback node from the
   global callback queue. Returns NULL if queue is empty.
*/
callback_t *g_callback_queue_pop()
{
  callback_t *callback = g_callback_queue;
  if (callback)
  {
    g_callback_queue = callback->next;
  }
  return callback;
}

/* * * * * * * * * * * * * * * * * * * * * * * * *
 * Functions related to LMFAO Ruby API
 * * * * * * * * * * * * * * * * * * * * * * * * */

/*
   Call LMFAO with the given argument.
*/
bool mLMFAO_call(VALUE handler, ons::Message* message)
{
  listener_consume_args_t args = {
    .handler = handler,
    .message = message
  };
  return lmfao_callback((void *) &args);
}

/*
  This is our user-defined C callback, it gets called by the C library.

  We need to:

  1. Create a callback structure, put our parameters in it
  2. Push the callback node onto the global callback queue
  3. Wait for the callback to be handled
  4. Return the return value
*/
void *lmfao_callback(void *data)
{
  callback_t callback;
  pthread_mutex_init(&callback.mutex, NULL);
  pthread_cond_init(&callback.cond, NULL);
  callback.data = data;
  callback.handled = false;

  // Put callback data in global callback queue
  pthread_mutex_lock(&g_callback_mutex);
  g_callback_queue_push(&callback);
  pthread_mutex_unlock(&g_callback_mutex);

  // Notify waiting Ruby thread that we have callback data
  pthread_cond_signal(&g_callback_cond);

  // Wait for callback to be handled
  pthread_mutex_lock(&callback.mutex);
  while (callback.handled == false)
  {
    pthread_cond_wait(&callback.cond, &callback.mutex);
  }
  pthread_mutex_unlock(&callback.mutex);

  // Clean up
  pthread_mutex_destroy(&callback.mutex);
  pthread_cond_destroy(&callback.cond);

  return callback.data;
}

/* * * * * * * * * * * * * * * * * * * * * * * * *
 * Our special Ruby event-listening thread functions
 * * * * * * * * * * * * * * * * * * * * * * * * */

/*
   Executed for each callback notification; what we receive
   are the callback parameters. The job of this method is to:

   1. Convert callback parameters into Ruby values
   2. Call the appropriate callback with said parameters
   3. Convert the Ruby return value into a C value
   4. Hand over the C value to the C callback
*/
VALUE LMFAO_handle_callback(void *cb)
{
  callback_t *callback = (callback_t*) cb;

  // figure out the proper handler for this particular event, as
  // well as convert the callback data into proper ruby values!
  listener_consume_args_t *args = (listener_consume_args_t*) callback->data;
  ons::Message *message = args->message;
  VALUE proc = args->handler;
  VALUE hash = rb_hash_new();
  rb_hash_aset(hash, ID2SYM(rb_intern("id")),    rb_str_new2(message->getMsgID().c_str()));
  rb_hash_aset(hash, ID2SYM(rb_intern("topic")), rb_str_new2(message->getTopic().c_str()));
  rb_hash_aset(hash, ID2SYM(rb_intern("tag")),   rb_str_new2(message->getTag().c_str()));
  rb_hash_aset(hash, ID2SYM(rb_intern("body")),  rb_str_new2(message->getBody().c_str()));
  rb_hash_aset(hash, ID2SYM(rb_intern("key")),   rb_str_new2(message->getKey().c_str()));
  rb_hash_aset(hash, ID2SYM(rb_intern("reconsume_times")),   INT2NUM(message->getReconsumeTimes()));
  rb_hash_aset(hash, ID2SYM(rb_intern("store_timestamp")),   LL2NUM(message->getStoreTimestamp()));
  rb_hash_aset(hash, ID2SYM(rb_intern("deliver_timestamp")), LL2NUM(message->getStartDeliverTime()));
  VALUE result = rb_funcall(proc, rb_intern("call"), 1, hash);

  // convert it to a C value that our callback can return
  if (result == Qnil || result == Qfalse) {
    callback->data = (void *) false;
  } else {
    callback->data = (void *) true;
  }

  // tell the callback that it has been handled, we are done
  pthread_mutex_lock(&callback->mutex);
  callback->handled = true;
  pthread_cond_signal(&callback->cond);
  pthread_mutex_unlock(&callback->mutex);

  return Qnil;
}

/*
   Wait for global callback queue to contain something.

   This function is called while not holding the GVL, so it’s safe
   to do long waits in here; other threads will still run.

   The job of this function is merely to wait until the callback queue
   contains something. Once that happens, we remove the item from the
   queue and return it to our caller through waiting->callback.
*/
void *wait_for_callback_signal(void *w)
{
  callback_waiting_t *waiting = (callback_waiting_t*) w;

  pthread_mutex_lock(&g_callback_mutex);

  // abort signal is used when ruby wants us to stop waiting
  while (waiting->abort == false && (waiting->callback = g_callback_queue_pop()) == NULL)
  {
    pthread_cond_wait(&g_callback_cond, &g_callback_mutex);
  }

  pthread_mutex_unlock(&g_callback_mutex);

  return NULL;
}

/*
   Stop waiting for callback notification. This function
   is invoked by Ruby when she wants us to exit.

   As `wait_for_callback_signal` function is executed without holding
   the GVL, it can potentially take forever. However, when somebody wants
   to quit the program (for example if somebody presses CTRL-C to exit)
   we must tell Ruby how to wake up the `wait_for_callback_signal` function
   so Ruby can exit properly.
*/
void stop_waiting_for_callback_signal(void *w)
{
  callback_waiting_t *waiting = (callback_waiting_t*) w;

  pthread_mutex_lock(&g_callback_mutex);
  waiting->abort = true;
  pthread_cond_signal(&g_callback_cond);
  pthread_mutex_unlock(&g_callback_mutex);
}

/*
  This thread loops continously, waiting for callbacks to happen in C.
  Once they do, it will handle the callback in a new thread.

  The reason we use a thread for handling the callback is that the handler
  itself might fire off more callbacks, that might need to be handled before
  the handler returns. We can’t do that if the event thread is busy handling
  the first callback.

  Continously, we need to:

  1. Unlock the Ruby GVL (allowing other threads to run while we wait)
  2. Wait for a callback to fire
  3. Spawn a new ruby thread to handle the callback
*/
VALUE LMFAO_event_thread(void *unused)
{
  callback_waiting_t waiting = {
    .callback = NULL,
    .abort    = false
  };

  while (waiting.abort == false)
  {
    // release the GVL while waiting for a callback notification
    rb_thread_call_without_gvl(wait_for_callback_signal, &waiting, stop_waiting_for_callback_signal, &waiting);

    // if ruby wants us to abort, this will be NULL
    if (waiting.callback)
    {
      rb_thread_create((VALUE (*)(...)) LMFAO_handle_callback, (void *) waiting.callback);
    }
  }

  return Qnil;
}

/* * * * * * * * * * * * * * * * * * * * * * * * *
 * Ruby bindings for LMFAO C library
 * * * * * * * * * * * * * * * * * * * * * * * * */

Rice::Object start_event_thread()
{
  return rb_thread_create((VALUE (*)(...)) LMFAO_event_thread, NULL);
}

void define_module_lmfao_under_module(Rice::Module module)
{
  Rice::Module rb_mLMFAO = module.define_module("LMFAO");
  rb_mLMFAO.define_singleton_method("start_event_thread", &start_event_thread);
}
