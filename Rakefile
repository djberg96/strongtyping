require 'rake'
require 'rake/clean'
require 'rake/testtask'
require 'rbconfig'
include Config

CLEAN.include("**/*.gem", "**/*.rbc")

desc "Build the strongtyping source"
task :build => [:clean] do
  make = File::ALT_SEPARATOR ? "nmake" : "make"
  Dir.chdir('ext') do
    ruby 'extconf.rb'
    sh "#{make}"
  end
end

namespace 'gem' do
  desc 'Create the strongtyping gem'
  task :create => [:clean] do
    Dir["*.gem"].each{ |f| File.delete(f) } # Clean first
    spec = eval(IO.read('strongtyping.gemspec'))
    Gem::Builder.new(spec).build
  end

  desc 'Install the strongtyping gem'
  task :install => [:create] do
    file = Dir["*.gem"].first
    sh "gem install #{file}"
  end
end

Rake::TestTask.new do |t|
  task :test => :build
  t.libs << 'ext'
  t.verbose = true
  t.warning = true
end

task :default => :test
