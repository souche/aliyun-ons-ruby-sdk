module Ons
  # the LMFAO library
  module LMFAO
    # Get the global LMFAO event thread.
    #
    # @return [Thread, nil] returns the event thread
    def self.event_thread
      @event_thread
    end

    # Start the global LMFAO event thread manually.
    #
    # @return [Thread] returns a created/existed event thread
    def self.start_event_thread
      @event_thread ||= Internal::LMFAO.start_event_thread
    end
  end
end
