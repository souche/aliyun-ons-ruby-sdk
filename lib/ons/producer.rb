module Ons
  # the ONS Producer
  class Producer
    # Get all the Producer instances.
    #
    # @return [<Producer>] all the Producer instances
    def self.instances
      @instances ||= []
    end

    # Create a new aliyun ONS Producer instance.
    #
    # @param access_key [String] the access key to aliyun ONS
    # @param secret_key [String] the secret key to aliyun ONS
    # @param producer_id [String] the producer ID
    # @param options [Hash{String, Symbol => String}]
    # @option options [String] :namesrv_addr the nameserver used to fetching ons_addr
    # @option options [String] :ons_addr the ONS server address
    # @option options [String, Fixnum, Bignum] :send_timeout (3000) send message timeout
    def initialize(access_key, secret_key, producer_id, options = {})
      @producer = Internal::Producer.new(access_key, secret_key, producer_id, options)

      # register instance
      self.class.instances << self
    end

    # Send a message.
    #
    # @example send 'Hello, World!' message with tag :tagA which is under topic :TopicTestMQ
    #   producer.send_message('TopicTestMQ', 'tagA', 'Hello, World!')
    #
    # @see #send_timer_message
    #
    # @param topic [String] the message topic
    # @param tag [String] the message tag
    # @param body [String] the message body
    # @param key [String] the message key
    # @return [String] the message id
    def send_message(topic, tag, body, key = '')
      @producer.send_message(topic, tag, body, key)
    end

    # Send a timer message.
    #
    # @example send 'Hello, World!' message at 30 seconds later
    #   producer.send_timer_message('TopicTestMQ', 'tagA', 'Hello, World!', Time.now + 30)
    #
    # @see #send_message
    #
    # @param topic [String] the message topic
    # @param tag [String] the message tag
    # @param body [String] the message body
    # @param timer [#to_i] when deliver the message
    # @param key [String] the message key
    # @return [String] the message id
    def send_timer_message(topic, tag, body, timer, key = '')
      @producer.send_timer_message(topic, tag, body, timer.to_i * 1000, key)
    end

    # Start the Producer instance.
    #
    # @return [self] returns itself
    #
    # @note this method should be called before send any message.
    def start
      @producer.start
      self
    end

    # Shutdown the Producer instance
    #
    # @return [void]
    #
    # @note this method should be called before program exit, otherwise it would case a memory leak.
    # Please see also {.register_cleanup_hooks} if you want call it automatically.
    def shutdown
      @producer.shutdown
    end
  end
end
