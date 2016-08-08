require 'bundler/gem_tasks'
require 'rake/extensiontask'
require 'rspec/core/rake_task'

Rake::ExtensionTask.new(:ons) do |ext|
  ext.lib_dir = './lib/ons'
end

RSpec::Core::RakeTask.new spec: %w(clobber compile)
task default: :spec
