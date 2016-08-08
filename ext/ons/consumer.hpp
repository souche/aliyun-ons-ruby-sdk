#ifndef __RBEXT_CONSUMER_HPP__
#define __RBEXT_CONSUMER_HPP__

#include <vector>

namespace Rice { class Hash; class Module; class Object; class String; }
namespace ons { class PushConsumer; }
class Listener;

class Consumer {

public:
  Consumer(Rice::String accessKey, Rice::String secretKey, Rice::String consumerId, Rice::Hash options);
  virtual ~Consumer();

  virtual void subscribe(Rice::String topic, Rice::String subscribeExpression, Rice::Object handler);

  // call this method once after subscribe topic.
  virtual void start();

  // call this method before program exit,
  // otherwise it would cause a memory leak and some other issues.
  virtual void shutdown();

private:
  ons::PushConsumer* onsPushConsumer;
  std::vector<Listener*> listeners;

};

void define_class_consumer_under_module(Rice::Module module);

#endif  // __RBEXT_CONSUMER_HPP__
