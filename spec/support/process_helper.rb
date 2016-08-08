require 'tempfile'

module Ons
  module RSpec
    module ProcessHelper
      def run_ruby_process(inner_code)
        file = Tempfile.open { |f| f.write(_code(inner_code)) && f }
        Process.wait(spawn(RbConfig.ruby, file.path))
      end

      def _code(inner_code)
        <<-EOT
          $LOAD_PATH.unshift File.expand_path('../../../lib', '#{__FILE__}')
          require 'ons'

          # require all support files
          Dir[File.expand_path('../*', '#{__FILE__}')].each { |f| require f }

          # include helpers
          include Ons::RSpec::Env
          include Ons::RSpec::Global
          include Ons::RSpec::WaitHelper

          # cleanup hooks
          Ons.register_cleanup_hooks

          # main logic
          #{inner_code}
        EOT
      end
    end
  end
end
