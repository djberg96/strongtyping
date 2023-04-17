module StrongTyping
  extend self

  class QueryParams
  end

  class ArgumentTypeError < ArgumentError; end
  class OverloadError < ArgumentError; end
  class ArgListError < ArgumentError; end

  def expect(*args)
    if args % 2 != 0
      raise ArgListError, "expect method requires argument pairs"
    end
  end

  def overload
  end

  def overload_exception
  end

  def overload_default
  end

  def overload_error
  end

  def get_arg_types
  end

  def verify_args_for
  end
end
