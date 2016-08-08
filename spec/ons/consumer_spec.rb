require 'spec_helper'
require 'English'

describe Ons::Consumer do
  describe '.instances' do
    it 'returns an array' do
      expect(described_class.instances).to be_kind_of(Array)
    end

    it 'returns all the instances' do
      expect(described_class.instances).to eq([g_consumer])
    end
  end

  describe '#subscribe' do
    it 'subscribes a specify tag' do
      run_ruby_process <<-EOT
        tag = env.tag + '-none'

        g_consumer(tag: tag).start
        g_producer.start
        g_messages.clear

        id1 = g_producer.send_message(env.topic, tag, '1')
        id2 = g_producer.send_message(env.topic, tag, '2')

        condition = -> { g_messages.count(&->(m) { [id1, id2].include?(m[:id]) }) == 2 }
        exit wait_until(condition)
      EOT
      expect($CHILD_STATUS).to be_success
    end

    it 'subscribes two tags use combine symbol' do
      run_ruby_process <<-EOT
        tagA = env.tag + '-combineA'
        tagB = env.tag + '-combineB'

        g_consumer(tag: "\#{tagA} || \#{tagB}").start
        g_producer.start
        g_messages.clear

        id1 = g_producer.send_message(env.topic, tagA, '1')
        id2 = g_producer.send_message(env.topic, tagB, '2')

        condition = -> { g_messages.count(&->(m) { [id1, id2].include?(m[:id]) }) == 2 }
        exit wait_until(condition)
      EOT
      expect($CHILD_STATUS).to be_success
    end

    it 'subscribes all tags use star symbol' do
      run_ruby_process <<-EOT
        tagA = env.tag + '-starA'
        tagB = env.tag + '-starB'

        g_consumer(tag: '*').start
        g_producer.start
        g_messages.clear

        id1 = g_producer.send_message(env.topic, tagA, '1')
        id2 = g_producer.send_message(env.topic, tagB, '2')

        condition = -> { g_messages.count(&->(m) { [id1, id2].include?(m[:id]) }) == 2 }
        exit wait_until(condition)
      EOT
      expect($CHILD_STATUS).to be_success
    end
  end
end
