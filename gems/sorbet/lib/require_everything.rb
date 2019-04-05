# frozen_string_literal: true
# typed: true

require_relative './gem_loader'

class ExitCalledError < RuntimeError
end

class Sorbet::Private::RequireEverything
  # Goes through the most common ways to require all your userland code
  def self.require_everything
    patch_kernel
    is_rails = load_rails
    load_bundler # this comes second since some rails projects fail `Bundler.require' before rails is loaded
    require_all_files if !is_rails
  end

  def self.load_rails
    return unless File.exist?('config/application.rb')
    begin
      require 'rails'
    rescue
      return false
    end
    require './config/application'
    rails = Object.const_get(:Rails)
    rails.application.require_environment!
    rails.application.eager_load!
    true
  end

  def self.load_bundler
    return unless File.exist?('Gemfile')
    begin
      require 'bundler'
    rescue
      return
    end
    Sorbet::Private::GemLoader.require_all_gems
  end

  def self.require_all_files
    abs_paths = Dir.glob("#{Dir.pwd}/**/*.rb")
    errors = []
    abs_paths.each do |abs_path|
      # Executable files are likely not meant to be required.
      # Some things we're trying to prevent against:
      # - misbehaving require-time side effects (removing files, reading from stdin, etc.)
      # - extra long runtime (making network requests, running a benchmark)
      # While this isn't a perfect heuristic for these things, it's pretty good.
      next if File.executable?(abs_path)

      begin
        watcher = Thread.new do
          sleep 5
          puts "Requiring #{abs_path} has taken more than 5 seconds."
        end
        require_relative abs_path
      rescue LoadError, NoMethodError, SyntaxError
        next
      rescue
        errors << abs_path
        next
      ensure
        watcher&.kill
      end
    end
    # one more chance for order dependent things
    errors.each do |abs_path|
      begin
        require_relative abs_path
      rescue
      end
    end
  end

  def self.patch_kernel
    Kernel.send(:define_method, :exit) do |*|
      puts 'Kernel#exit was called while requiring ruby source files'
      raise ExitCalledError.new
    end

    Kernel.send(:define_method, :at_exit) do |&block|
      if File.split($0).last == 'rake'
        # Let `rake test` work
        super
        return proc {}
      end
      puts "Ignoring at_exit: #{block}"
      proc {}
    end
  end
end
