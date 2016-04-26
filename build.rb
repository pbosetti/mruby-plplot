#!/usr/bin/env ruby

if __FILE__ == $PROGRAM_NAME
  require 'fileutils'
  FileUtils.mkdir_p 'tmp'
  unless File.exists?('tmp/mruby')
    system 'git clone --depth 1 https://github.com/mruby/mruby.git tmp/mruby'
  end
  system(%Q[cd tmp/mruby; MRUBY_CONFIG=#{File.expand_path __FILE__} ./minirake #{ARGV.join(' ')}])
  system %Q"ln -s tmp/mruby/bin/mirb ."
  exit system %Q"ln -s tmp/mruby/bin/mruby ."
end

MRuby::Build.new do |conf|
  toolchain :gcc
  conf.cc.flags += %w(-fexceptions -Wno-deprecated-declarations)
  conf.cc.defines += %w(ENABLE_READLINE)
  conf.gembox 'default'
  conf.gem File.dirname(__FILE__)
end