#include <rice/Constructor.hpp>
#include <rice/Data_Type.hpp>
#include <rice/Hash.hpp>
#include <rice/Module.hpp>
#include <rice/Object.hpp>
#include <rice/String.hpp>
#include <rice/Symbol.hpp>
#include <ons/ONSFactory.h>
#include "producer.hpp"

Producer::Producer(Rice::String accessKey, Rice::String secretKey, Rice::String producerId, Rice::Hash options)
{
  ons::ONSFactoryProperty factoryInfo;
  factoryInfo.setFactoryProperty(ons::ONSFactoryProperty::AccessKey,   accessKey.str());
  factoryInfo.setFactoryProperty(ons::ONSFactoryProperty::SecretKey,   secretKey.str());
  factoryInfo.setFactoryProperty(ons::ONSFactoryProperty::ProducerId, producerId.str());

  Rice::Object namesrvAddr = options.call("[]", Rice::String("namesrv_addr"));
  if (!namesrvAddr) { namesrvAddr = options.call("[]", Rice::Symbol("namesrv_addr")); }
  if (namesrvAddr) { factoryInfo.setFactoryProperty(ons::ONSFactoryProperty::NAMESRV_ADDR, ((Rice::String)namesrvAddr).str()); }

  Rice::Object onsAddr = options.call("[]", Rice::String("ons_addr"));
  if (!onsAddr) { onsAddr = options.call("[]", Rice::Symbol("ons_addr")); }
  if (onsAddr) { factoryInfo.setFactoryProperty(ons::ONSFactoryProperty::ONSAddr, ((Rice::String)onsAddr).str()); }

  Rice::Object sendTimeout = options.call("[]", Rice::String("send_timeout"));
  if (!sendTimeout) { sendTimeout = options.call("[]", Rice::Symbol("send_timeout")); }
  if (sendTimeout.rb_type() == T_FIXNUM || sendTimeout.rb_type() == T_BIGNUM) { sendTimeout = sendTimeout.to_s(); }
  if (sendTimeout) { factoryInfo.setFactoryProperty(ons::ONSFactoryProperty::SendMsgTimeoutMillis, ((Rice::String)sendTimeout).str()); }

  this->onsProducer = ons::ONSFactory::getInstance()->createProducer(factoryInfo);
}

Rice::String Producer::sendMessage(Rice::String topic, Rice::String tag, Rice::String body, Rice::String key)
{
  ons::Message message(topic.str(), tag.str(), body.str());
  if (key.length() != 0) { message.setKey(key.str()); }

  ons::SendResultONS sendResult = this->onsProducer->send(message);
  return sendResult.getMessageId();
}

Rice::String Producer::sendTimerMessage(Rice::String topic, Rice::String tag, Rice::String body, Rice::Object deliverTimestamp, Rice::String key)
{
  ons::Message message(topic.str(), tag.str(), body.str());
  if (key.length() != 0) { message.setKey(key.str()); }

  long long timestamp = NUM2LL(deliverTimestamp.value());
  if (timestamp != 0) { message.setStartDeliverTime(timestamp); }

  ons::SendResultONS sendResult = this->onsProducer->send(message);
  return sendResult.getMessageId();
}

void Producer::start()
{
  this->onsProducer->start();
}

void Producer::shutdown()
{
  this->onsProducer->shutdown();
}

void define_class_producer_under_module(Rice::Module module)
{
  Rice::Data_Type<Producer> rb_cProducer = Rice::define_class_under<Producer>(module, "Producer");

  rb_cProducer.define_constructor(Rice::Constructor<Producer, Rice::String, Rice::String, Rice::String, Rice::Hash>());
  rb_cProducer.define_method("send_message", &Producer::sendMessage);
  rb_cProducer.define_method("send_timer_message", &Producer::sendTimerMessage);
  rb_cProducer.define_method("start", &Producer::start);
  rb_cProducer.define_method("shutdown", &Producer::shutdown);
}
