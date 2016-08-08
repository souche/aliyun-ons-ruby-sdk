module Ons
  module RSpec
    module WaitHelper
      def wait_until(condition, options = {})
        options.fetch(:timeout, 8).times do
          return true if condition.call
          sleep 1
        end; false
      end
    end
  end
end
