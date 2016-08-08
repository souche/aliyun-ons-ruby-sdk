module Ons
  # the ONS Consumer
  class Consumer
    # Get all the Consumer instances.
    #
    # @return [<Consumer>] all the Consumer instances
    def self.instances
      @instances ||= []
    end

    # Create a new aliyun ONS Consumer instance.
    #
    # @param access_key [String] the access key to aliyun ONS
    # @param secret_key [String] the secret key to aliyun ONS
    # @param consumer_id [String] the consumer ID
    # @param options [Hash{String, Symbol => String}]
    # @option options [String] :namesrv_addr the nameserver used to fetching ons_addr
    # @option options [String] :ons_addr the ONS server address
    # @option options [String, Fixnum, Bignum] :thread_num the consumer thread numbers
    def initialize(access_key, secret_key, consumer_id, options = {})
      @consumer = Internal::Consumer.new(access_key, secret_key, consumer_id, options)

      # register instance
      self.class.instances << self

      # start the global LMFAO event thread, so it could consume messages properly
      LMFAO.start_event_thread
    end

    # Subsribe a topic.
    #
    # @example subscribe tag :tagA under topic :TopicTestMQ
    #   consumer.subscribe('TopicTestMQ', 'tagA') {}
    #
    # @example subscribe tag :tagA and :tagB under topic :TopicTestMQ
    #   consumer.subscribe('TopicTestMQ', 'tagA || tagB') {}
    #
    # @example subscribe all tags under topic :TopicTestMQ
    #   consumer.subscribe('TopicTestMQ', '*') {}
    #
    # @param topic [String] the message topic
    # @param expression [String] the subsribe expression used to filter messages
    # @param handler [#call] the handler which will handle the message
    # @return [self] returns itself
    def subscribe(topic, expression, handler = nil)
      @consumer.subscribe(topic, expression, handler || Proc.new)
      self
    end

    # Start the Consumer instance.
    #
    # @return [self] returns itself
    #
    # @note this method should be called after subscribe.
    # @note thie method will not block the thread, please see also {.loop_forever}.
    def start
      @consumer.start
      self
    end

    # Shutdown the Consumer instance.
    #
    # @return [void]
    #
    # @note this method should be called before program exit, otherwise it would case a memory leak.
    # Please see also {.register_cleanup_hooks} if you want call it automatically.
    def shutdown
      @consumer.shutdown
    end
  end
end
