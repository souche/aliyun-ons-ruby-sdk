# coding: utf-8

lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'ons/version'

Gem::Specification.new do |spec|
  spec.name          = 'ons'
  spec.version       = Ons::VERSION
  spec.authors       = ['caochaoping']
  spec.email         = ['caochaoping@souche.com']

  spec.summary       = 'unoffical aliyun ONS sdk (ruby version)'
  spec.description   = 'unoffical aliyun ONS sdk (ruby version)'
  spec.homepage      = ''

  # Prevent pushing this gem to RubyGems.org. To allow pushes either set the 'allowed_push_host'
  # to allow pushing to a single host or delete this section to allow pushing to any host.
  if spec.respond_to?(:metadata) # rubocop: disable Style/GuardClause
    spec.metadata['allowed_push_host'] = 'rubygems.org'
  else
    raise 'RubyGems 2.0 or newer is required to protect against public gem pushes.'
  end

  spec.files         = `git ls-files -z`.split("\x0").reject { |f| f.match(%r{^(test|spec|features)/}) }
  spec.bindir        = 'exe'
  spec.executables   = spec.files.grep(%r{^exe/}) { |f| File.basename(f) }
  spec.extensions    = ['ext/ons/extconf.rb']
  spec.require_paths = ['lib']

  spec.add_runtime_dependency 'rice', '~> 2.1.0'

  spec.add_development_dependency 'bundler', '~> 1.12'
  spec.add_development_dependency 'rake', '~> 10.0'
  spec.add_development_dependency 'rspec', '~> 3.0'
  spec.add_development_dependency 'rubocop', '~> 0.41.0'
  spec.add_development_dependency 'rubocop-rspec', '~> 1.5.0'
  spec.add_development_dependency 'rake-compiler', '~> 1.0.1'
  spec.add_development_dependency 'yard', '~> 0.9.0'
  spec.add_development_dependency 'concurrent-ruby', '~> 1.0.2'
end
