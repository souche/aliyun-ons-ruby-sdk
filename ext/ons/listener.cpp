#include <rice/Hash.hpp>
#include "listener.hpp"
#include "lmfao.hpp"

void Listener::setHandler(Rice::Object handler)
{
  this->handler = handler.value();
  this->riceHandler = handler;
}

Action Listener::consume(ons::Message& message, ons::ConsumeContext& context)
{
  // Listener::consume will called in a separate thread which is not created by Ruby,
  // so you cannot call any Ruby functions here. Use the LMFAO library workaround.
  bool result = mLMFAO_call(this->handler, &message);

  if (result) { return CommitMessage; }
  return ReconsumeLater;
}
