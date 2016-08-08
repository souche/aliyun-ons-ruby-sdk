require 'spec_helper'

describe Ons::Producer do
  subject! { g_producer }

  describe '.instances' do
    it 'returns an array' do
      expect(described_class.instances).to be_kind_of(Array)
    end

    it 'returns all the instances' do
      expect(described_class.instances).to eq(Array(g_producer))
    end
  end

  describe '#send_message' do
    it 'sends a message' do
      expect { subject.send_message(env.topic, env.tag, 'Hello, World!') }.not_to raise_error
    end

    it 'sends a large message (size: 32KB)' do
      expect { subject.send_message(env.topic, env.tag, '01' * 1024 * 16) }.not_to raise_error
    end

    it 'sends a GBK encoded message' do
      expect { subject.send_message(env.topic, env.tag, '你好，世界！'.encode('GBK')) }.not_to raise_error
    end

    it 'fails when sent to a wrong topic' do
      expect { subject.send_message(env.topic + '-not-found', env.tag, '-') }.to raise_error(/Topic Route does not exist/)
    end
  end

  describe '#send_timer_message' do
    it 'sends a timer message' do
      expect { subject.send_timer_message(env.topic, env.tag, 'T', Time.now) }.not_to raise_error
      expect { subject.send_timer_message(env.topic, env.tag, 'S', Time.now.to_s) }.not_to raise_error
      expect { subject.send_timer_message(env.topic, env.tag, 'I', Time.now.to_i) }.not_to raise_error
    end
  end
end
