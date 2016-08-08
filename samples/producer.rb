require 'bundler/setup'
require 'ons'

# register cleanup hooks, it will shutdown any Producer instances at program exit
Ons.register_cleanup_hooks

# create an ONS Producer instance
producer = Ons::Producer.new(ENV.fetch('ONS_ACCESS_KEY'), ENV.fetch('ONS_SECRET_KEY'), ENV.fetch('ONS_PRODUCER_ID'))

# start it before send any message
producer.start

# send messages
data = Array('a'..'z').join + Array('A'..'Z').join
loop do
  topic = ENV.fetch('ONS_TOPIC')
  tag = 'onsTag#samples'
  body = Range.new(1, 64).map(&->(_) { data[rand(data.size)] }).join
  key = 'samples-' + Range.new(1, 16).map(&->(_) { data[rand(data.size)] }).join

  begin
    print format("===== produce message =====\n")
    print format("        topic: %s\n", topic)
    print format("          tag: %s\n", tag)
    print format("         body: %s\n", body)
    id = producer.send_message(topic, tag, body, key)
    print format("           id: %s\n", id)
    print "\n"
  rescue => ex # such as Timeout, etc.
    print format("      failure: %s\n", ex.message)
    print "\n"
  end

  sleep rand(8)
end
