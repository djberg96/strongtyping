require 'rubygems'

Gem::Specification.new do |spec|
  spec.name       = 'strongtyping'
  spec.version    = '2.0.7'
  spec.authors    = ['Ryan Pavlik', 'Daniel Berger']
  spec.email      = 'rpav@mephle.com'
  spec.license    = 'LGPL'
  spec.homepage   = 'http://mephle.org/StrongTyping/'
  spec.summary    = 'A Ruby library that provides type checking and method overloading'
  spec.test_files = Dir['test/*.rb']
  spec.files      = Dir['**/*'].reject{ |f| f.include?('git') }
  spec.extensions = ['ext/extconf.rb']

  spec.extra_rdoc_files  = ['LGPL', 'CHANGES.md', 'README.md', 'MANIFEST.md']

  spec.add_development_dependency('rake')
  spec.add_development_dependency('test-unit', '~> 3.4')

  spec.metadata = {
    'homepage_uri'          => 'https://github.com/djberg96/strongtyping',
    'bug_tracker_uri'       => 'https://github.com/djberg96/strongtyping/issues',
    'changelog_uri'         => 'https://github.com/djberg96/strongtyping/blob/main/CHANGES.md',
    'documentation_uri'     => 'https://github.com/djberg96/strongtyping/wiki',
    'source_code_uri'       => 'https://github.com/djberg96/strongtyping',
    'wiki_uri'              => 'https://github.com/djberg96/strongtyping/wiki',
    'rubygems_mfa_required' => 'true'
  }

  spec.description = %Q{
    The strongtyping gem is a Ruby library that provides type checking and
    method overloading.
  }
end
