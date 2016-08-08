ENV['ONS_TAG'] = 'onsTag#rspec'
REQUIRED_ENV = %w(ONS_ACCESS_KEY ONS_SECRET_KEY ONS_PRODUCER_ID ONS_CONSUMER_ID ONS_TOPIC ONS_TAG).freeze
REQUIRED_ENV.each { |env| ENV.fetch(env) }

module Ons
  module RSpec
    module Env
      def env
        @env ||=
          Struct.new(:access_key, :secret_key, :producer_id, :consumer_id, :topic, :tag)
                .new(*REQUIRED_ENV.map(&->(env) { ENV.fetch(env) }))
      end
    end
  end
end
