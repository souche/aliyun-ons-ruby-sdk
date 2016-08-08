$LOAD_PATH.unshift File.expand_path('../../lib', __FILE__)
require 'ons'

# require all support files
Dir[File.expand_path('../support/*', __FILE__)].each { |f| require f }

# see http://rubydoc.info/gems/rspec-core/RSpec/Core/Configuration
RSpec.configure do |c|
  c.include Ons::RSpec::Env
  c.include Ons::RSpec::Global
  c.include Ons::RSpec::WaitHelper
  c.include Ons::RSpec::ProcessHelper

  c.before(:all) do
    Ons.register_cleanup_hooks

    g_producer.start
    g_consumer.start.shutdown
  end
end
