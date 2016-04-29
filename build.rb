#!/usr/bin/env ruby
BUILD_DIR = 'mruby_build'
if __FILE__ == $PROGRAM_NAME
  require 'fileutils'
  unless File.exists?(BUILD_DIR)
    system "git clone --depth 1 https://github.com/mruby/mruby.git #{BUILD_DIR}"
  end
  system(%Q[cd #{BUILD_DIR}; MRUBY_CONFIG=#{File.expand_path __FILE__} ./minirake #{ARGV.join(' ')}])
  system %Q"ln -fs #{BUILD_DIR}/bin ."
  exit 0
end

MRuby::Build.new do |conf|
  toolchain :gcc
  conf.cc.flags += %w(-fexceptions -Wno-deprecated-declarations)
  conf.cc.defines += %w(ENABLE_READLINE)
  conf.gembox 'default'
  conf.gem File.dirname(__FILE__)
end