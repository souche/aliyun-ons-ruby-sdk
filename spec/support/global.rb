# rubocop:disable Style/GlobalVars

require 'concurrent'

module Ons
  module RSpec
    module Global
      def g_producer
        $g_producer ||=
          Ons::Producer.new(env.access_key, env.secret_key, env.producer_id)
      end

      def g_consumer(options = {}) # rubocop: disable Metrics/AbcSize
        $g_consumer ||=
          Ons::Consumer.new(env.access_key, env.secret_key, env.consumer_id)
                       .subscribe(env.topic, options.fetch(:tag, env.tag)) { |message| g_messages << message }
      end

      def g_messages
        $g_messages ||=
          Concurrent::Array.new
      end
    end
  end
end
