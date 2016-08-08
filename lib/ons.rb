require 'ons/version'

# require dynamic library
require 'ons/ons'

# require self
require 'ons/lmfao'
require 'ons/consumer'
require 'ons/producer'

# .
module Ons
  # Perform an automatic cleanup at program exit.
  #
  # It handles:
  #
  # * shutdown all the Consumer instances, see also {Consumer#shutdown}
  # * shutdown all the Producer instances, see also {Producer#shutdown}
  # * kill the global LMFAO event thread if it's running
  def self.register_cleanup_hooks
    return if @registered
    @registered = true

    at_exit do
      Consumer.instances.each(&:shutdown)
      Producer.instances.each(&:shutdown)
      LMFAO.event_thread && LMFAO.event_thread.terminate
    end
  end

  # Block the current thread.
  def self.loop_forever
    raise 'please start an Ons::Consumer instance before loop forever' if Consumer.instances.empty?
    LMFAO.event_thread.join
  end
end
