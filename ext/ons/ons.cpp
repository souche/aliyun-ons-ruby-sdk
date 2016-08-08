#include <rice/Module.hpp>
#include "lmfao.hpp"
#include "consumer.hpp"
#include "producer.hpp"
#include "ons.hpp"

extern "C"
void Init_ons()
{
  RUBY_TRY
  {
    Rice::Module rb_mOns = Rice::define_module("Ons");
    Rice::Module rb_mInternal = rb_mOns.define_module("Internal");
    define_class_consumer_under_module(rb_mInternal);
    define_class_producer_under_module(rb_mInternal);
    define_module_lmfao_under_module(rb_mInternal);
  }
  RUBY_CATCH
}
