require 'bundler/setup'
require 'ons'

# register cleanup hooks, it will shutdown any Consumer instances at program exit
Ons.register_cleanup_hooks

# create an ONS Consumer instance
consumer = Ons::Consumer.new(ENV.fetch('ONS_ACCESS_KEY'), ENV.fetch('ONS_SECRET_KEY'), ENV.fetch('ONS_CONSUMER_ID'))

# subscribe all tags under specify topic
consumer.subscribe(ENV.fetch('ONS_TOPIC'), '*') do |message|
  begin
    print format("===== consume message =====\n")
    print format("        topic: %s\n", message[:topic])
    print format("          tag: %s\n", message[:tag])
    print format("         body: %s\n", message[:body])
    print format("           id: %s\n", message[:id])
    print format("          key: %s\n", message[:key])
    print format("    reconsume: %s times\n", message[:reconsume_times])
    print format("     store at: %s (%s)\n", message[:store_timestamp], Time.at(message[:store_timestamp] / 1_000).localtime)
    print format("   deliver at: %s (%s)\n", message[:deliver_timestamp], Time.at(message[:deliver_timestamp] / 1_000).localtime)
    print "\n"
  rescue => ex # handle any exception here
    print format("      failure: %s\n", ex.message)
    print "\n"
  end

  true # commit message
end

# start it after subscribe topic
consumer.start && Ons.loop_forever
