#ifndef __RBEXT_PRODUCER_HPP__
#define __RBEXT_PRODUCER_HPP__

namespace Rice { class Hash; class Module; class Object; class String; }
namespace ons { class Producer; }

class Producer {

public:
  Producer(Rice::String accessKey, Rice::String secretKey, Rice::String producerId, Rice::Hash options);
  virtual ~Producer() {}

  virtual Rice::String sendMessage(Rice::String topic, Rice::String tag, Rice::String body, Rice::String key);
  virtual Rice::String sendTimerMessage(Rice::String topic, Rice::String tag, Rice::String body, Rice::Object deliverTimestamp, Rice::String key);

  // call this method once before send any messages.
  virtual void start();

  // call this method before program exit,
  // otherwise it would cause a memory leak and some other issues.
  virtual void shutdown();

private:
  ons::Producer* onsProducer;

};

void define_class_producer_under_module(Rice::Module module);

#endif  // __RBEXT_PRODUCER_HPP__
