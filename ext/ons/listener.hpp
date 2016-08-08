#ifndef __RBEXT_LISTENER_HPP__
#define __RBEXT_LISTENER_HPP__

#include <rice/Object.hpp>
#include <ons/MessageListener.h>

class Listener: public ons::MessageListener {

public:
  Listener() {}
  virtual ~Listener() {}

  virtual void setHandler(Rice::Object handler);

  // implement the interface of consuming message.
  virtual Action consume(ons::Message& message, ons::ConsumeContext& context);

private:
  // a object which should be repsond to :call method.
  VALUE handler;
  Rice::Object riceHandler;

};

#endif  // __RBEXT_LISTENER_HPP__
