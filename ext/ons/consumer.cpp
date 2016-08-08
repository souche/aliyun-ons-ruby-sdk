#include <rice/Constructor.hpp>
#include <rice/Data_Type.hpp>
#include <rice/Hash.hpp>
#include <rice/Module.hpp>
#include <rice/String.hpp>
#include <rice/Symbol.hpp>
#include <ons/ONSFactory.h>
#include "listener.hpp"
#include "consumer.hpp"

Consumer::Consumer(Rice::String accessKey, Rice::String secretKey, Rice::String consumerId, Rice::Hash options)
{
  ons::ONSFactoryProperty factoryInfo;
  factoryInfo.setFactoryProperty(ons::ONSFactoryProperty::AccessKey,   accessKey.str());
  factoryInfo.setFactoryProperty(ons::ONSFactoryProperty::SecretKey,   secretKey.str());
  factoryInfo.setFactoryProperty(ons::ONSFactoryProperty::ConsumerId, consumerId.str());

  Rice::Object namesrvAddr = options.call("[]", Rice::String("namesrv_addr"));
  if (!namesrvAddr) { namesrvAddr = options.call("[]", Rice::Symbol("namesrv_addr")); }
  if (namesrvAddr) { factoryInfo.setFactoryProperty(ons::ONSFactoryProperty::NAMESRV_ADDR, ((Rice::String)namesrvAddr).str()); }

  Rice::Object onsAddr = options.call("[]", Rice::String("ons_addr"));
  if (!onsAddr) { onsAddr = options.call("[]", Rice::Symbol("ons_addr")); }
  if (onsAddr) { factoryInfo.setFactoryProperty(ons::ONSFactoryProperty::ONSAddr, ((Rice::String)onsAddr).str()); }

  Rice::Object threadNum = options.call("[]", Rice::String("thread_num"));
  if (!threadNum) { threadNum = options.call("[]", Rice::Symbol("thread_num")); }
  if (threadNum.rb_type() == T_FIXNUM || threadNum.rb_type() == T_BIGNUM) { threadNum = threadNum.to_s(); }
  if (threadNum) { factoryInfo.setFactoryProperty(ons::ONSFactoryProperty::ConsumeThreadNums, ((Rice::String)threadNum).str()); }

  this->onsPushConsumer = ons::ONSFactory::getInstance()->createPushConsumer(factoryInfo);
}

Consumer::~Consumer()
{
  for (std::vector<Listener*>::iterator iter = this->listeners.begin(); iter != this->listeners.end(); ++iter) {
    delete *iter;
  }
}

void Consumer::subscribe(Rice::String topic, Rice::String subscribeExpression, Rice::Object handler)
{
  Listener *listener = new Listener;
  listener->setHandler(handler);

  this->listeners.push_back(listener);
  this->onsPushConsumer->subscribe(topic.str(), subscribeExpression.str(), listener);
}

void Consumer::start()
{
  this->onsPushConsumer->start();
}

void Consumer::shutdown()
{
  this->onsPushConsumer->shutdown();
}

void define_class_consumer_under_module(Rice::Module module)
{
  Rice::Data_Type<Consumer> rb_cConsumer = Rice::define_class_under<Consumer>(module, "Consumer");

  rb_cConsumer.define_constructor(Rice::Constructor<Consumer, Rice::String, Rice::String, Rice::String, Rice::Hash>());
  rb_cConsumer.define_method("subscribe", &Consumer::subscribe);
  rb_cConsumer.define_method("start", &Consumer::start);
  rb_cConsumer.define_method("shutdown", &Consumer::shutdown);
}
