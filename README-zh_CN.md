# Ons

阿里云 ONS 消息队列服务 Ruby SDK（非官方版本）

## 项目依赖

* Linux/Unix 系统
* Ruby 2.1.5 或以上版本
* Boost C++ 程序库

## 安装步骤

### 安装 Ruby 程序

```sh
$ rvm install 2.1.5
```

### 安装 Boost C++ 程序库

```sh
$ apt-get install libboost-all-dev
```

### 安装 ons

```sh
$ git clone <git-repository>
$ cd ons
$ ./bin/setup
$ ./bin/rake install
```

## 使用方式

### 环境准备

* 安装 ons rubygems。
* 代码里涉及到的 TOPIC、PRODUCER_ID、CONSUMER_ID，需要在 MQ 控制台上创建。 Message Tag 可以完全由应用自定义，具体创建过程可参考 [申请MQ资源](https://help.aliyun.com/document_detail/29536.html)。
* 使用 MQ 服务的应用程序需要部署在阿里云 ECS 上。

部分资料来自 [消息队列 > TCP 接入(专业) > C/C++ SDK 环境准备](https://help.aliyun.com/document_detail/29555.html)

### 生产者

```ruby
producer = Ons::Producer.new('<ONS_ACCESS_KEY>', '<ONS_SECRET_KEY>', '<ONS_PRODUCER_ID>')

# 资源准备
producer.start

# 在 topic <ONS_TOPIC> 下发送消息
producer.send_message('<ONS_TOPIC>', 'tag', 'Hello, World!')

# 必须在程序退出前调用 shutdown 方法释放相关资源，否则可能出现栈错误
producer.shutdown
```

更多用法，请参考 {file:samples/producer.rb} 及 API 文档

### 消费者

```ruby
consumer = Ons::Consumer.new('<ONS_ACCESS_KEY>', '<ONS_SECRET_KEY>', '<ONS_CONSUMER_ID>')

# 订阅 topic <ONS_TOPIC> 下的所有消息
# 注意，当消息达到时，会调用下述函数，此函数会被调度到单独线程中执行
consumer.subscribe('<ONS_TOPIC>', '*') { |message| p message }

# 资源准备
consumer.start

# 等待消息到达
sleep 32

# 必须在程序退出前调用 shutdown 方法释放相关资源，否则可能出现栈错误
consumer.shutdown
```

更多用法，请参考 {file:samples/consumer.rb} 及 API 文档
