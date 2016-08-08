require 'spec_helper'

describe Ons::LMFAO do
  describe '.event_thread' do
    it 'returns the same thread' do
      expect(described_class.event_thread).to eq(described_class.event_thread)
    end
  end

  describe '.start_event_thread' do
    it 'returns the same thread' do
      expect(described_class.start_event_thread).to eq(described_class.start_event_thread)
    end
  end
end
