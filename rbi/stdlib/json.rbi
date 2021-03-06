# typed: __STDLIB_INTERNAL

module JSON
  FAST_STATE_PROTOTYPE = ::T.let(nil, ::T.untyped)
  Infinity = ::T.let(nil, ::T.untyped)
  JSON_LOADED = ::T.let(nil, ::T.untyped)
  MinusInfinity = ::T.let(nil, ::T.untyped)
  NaN = ::T.let(nil, ::T.untyped)
  PRETTY_STATE_PROTOTYPE = ::T.let(nil, ::T.untyped)
  SAFE_STATE_PROTOTYPE = ::T.let(nil, ::T.untyped)
  VERSION = ::T.let(nil, ::T.untyped)
  VERSION_ARRAY = ::T.let(nil, ::T.untyped)
  VERSION_BUILD = ::T.let(nil, ::T.untyped)
  VERSION_MAJOR = ::T.let(nil, ::T.untyped)
  VERSION_MINOR = ::T.let(nil, ::T.untyped)

  sig do
    params(
      object: ::T.untyped,
      opts: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.[](object, opts=T.unsafe(nil)); end

  sig do
    params(
      modul: ::T.untyped,
      constant: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.const_defined_in?(modul, constant); end

  sig {returns(::T.untyped)}
  def self.create_id(); end

  sig do
    params(
      create_id: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.create_id=(create_id); end

  sig do
    params(
      path: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.deep_const_get(path); end

  sig do
    params(
      obj: ::T.untyped,
      args: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.dump(obj, *args); end

  sig {returns(::T.untyped)}
  def self.dump_default_options(); end

  sig do
    params(
      dump_default_options: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.dump_default_options=(dump_default_options); end

  sig do
    params(
      obj: ::T.untyped,
      opts: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.fast_generate(obj, opts=T.unsafe(nil)); end

  sig do
    params(
      obj: ::T.untyped,
      opts: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.fast_unparse(obj, opts=T.unsafe(nil)); end

  sig do
    params(
      obj: ::T.untyped,
      args: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.generate(obj, *args); end

  sig {returns(::T.untyped)}
  def self.generator(); end

  sig do
    params(
      generator: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.generator=(generator); end

  sig do
    params(
      to: ::T.untyped,
      from: ::T.untyped,
      string: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.iconv(to, from, string); end

  sig do
    params(
      source: ::T.untyped,
      proc: ::T.untyped,
      options: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.load(source, proc=T.unsafe(nil), options=T.unsafe(nil)); end

  sig {returns(::T.untyped)}
  def self.load_default_options(); end

  sig do
    params(
      load_default_options: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.load_default_options=(load_default_options); end

  sig do
    params(
      json: ::T.untyped,
      args: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.parse(json, args=T.unsafe(nil)); end

  sig do
    params(
      source: ::T.untyped,
      opts: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.parse!(source, opts=T.unsafe(nil)); end

  sig {returns(::T.untyped)}
  def self.parser(); end

  sig do
    params(
      parser: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.parser=(parser); end

  sig do
    params(
      obj: ::T.untyped,
      args: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.pretty_generate(obj, *args); end

  sig do
    params(
      obj: ::T.untyped,
      opts: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.pretty_unparse(obj, opts=T.unsafe(nil)); end

  sig do
    params(
      result: ::T.untyped,
      proc: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.recurse_proc(result, &proc); end

  sig do
    params(
      source: ::T.untyped,
      proc: ::T.untyped,
      options: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.restore(source, proc=T.unsafe(nil), options=T.unsafe(nil)); end

  sig {returns(::T.untyped)}
  def self.state(); end

  sig do
    params(
      state: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.state=(state); end

  sig do
    params(
      string: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.swap!(string); end

  sig do
    params(
      obj: ::T.untyped,
      opts: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.unparse(obj, opts=T.unsafe(nil)); end

  sig do
    params(
      obj: ::T.untyped,
      anIO: ::T.untyped,
      limit: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.unsafe_dump(obj, anIO=T.unsafe(nil), limit=T.unsafe(nil)); end

  sig do
    params(
      obj: ::T.untyped,
      opts: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.unsafe_generate(obj, opts=T.unsafe(nil)); end

  sig do
    params(
      source: ::T.untyped,
      opts: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.unsafe_parse(source, opts=T.unsafe(nil)); end

  sig do
    params(
      obj: ::T.untyped,
      opts: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.unsafe_pretty_generate(obj, opts=T.unsafe(nil)); end
end

class JSON::CircularDatastructure < JSON::NestingError
end

class JSON::GeneratorError < JSON::JSONError
end

class JSON::GenericObject < OpenStruct
  sig do
    params(
      name: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def [](name); end

  sig do
    params(
      name: ::T.untyped,
      value: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def []=(name, value); end

  sig do
    params(
      _: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def as_json(*_); end

  sig {returns(::T.untyped)}
  def to_hash(); end

  sig do
    params(
      a: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def to_json(*a); end

  sig do
    params(
      other: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def |(other); end

  sig do
    params(
      _: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.[](*_); end

  sig do
    params(
      obj: ::T.untyped,
      args: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.dump(obj, *args); end

  sig do
    params(
      object: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.from_hash(object); end

  sig do
    params(
      json_creatable: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.json_creatable=(json_creatable); end

  sig {returns(::T.untyped)}
  def self.json_creatable?(); end

  sig do
    params(
      data: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.json_create(data); end

  sig do
    params(
      source: ::T.untyped,
      proc: ::T.untyped,
      opts: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.load(source, proc=T.unsafe(nil), opts=T.unsafe(nil)); end
end

class JSON::JSONError < StandardError
  sig do
    params(
      exception: ::T.untyped,
    )
    .returns(::T.untyped)
  end
  def self.wrap(exception); end
end

class JSON::MissingUnicodeSupport < JSON::JSONError
end

class JSON::NestingError < JSON::ParserError
end

class JSON::ParserError < JSON::JSONError
end
