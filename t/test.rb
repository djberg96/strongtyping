require 'test/unit'
require 'strongtyping'
include StrongTyping

class TS_StrongTyping < Test::Unit::TestCase
  def test_expect
    assert_raises(ArgumentTypeError) {
      foo("hello", 2, 3)
    }

    assert_nothing_raised {
      optional(1)
      optional(1, 2)
    }

    assert_raises(ArgumentTypeError) {
      optional(1, "abc")
    }

    assert_raises(ArgumentTypeError) {
      optional2(nil, "abc")
    }
  end #m:test_expect

  def test_overload
    assert_nothing_raised {
      add
      add 1, 2
      add "a", "b"
    }

    assert_raises(RuntimeError) {
      add "a", 2
    }

    assert_raises(OverloadError) {
      add 2, "a"
    }
  end #m:test_overload


  def test_get_arg_types
    assert_equal([[]], get_arg_types(method(:blank)))

    assert_equal([[String, Integer, String]],
                 get_arg_types(method(:foo)))

    assert_equal([[Integer, [Integer, NilClass]]],
                 get_arg_types(method(:optional)))

    assert_equal([[], [Integer, Integer], [String, String]],
                 get_arg_types(method(:add)))
  end #m:test_get_arg_types


  def test_verify_args_for
    a1 = [1, 2]
    a2 = ["a", 2]

    assert(verify_args_for(method(:add), a1))
    assert(!verify_args_for(method(:add), a2))
  end #m:test_verify_args_for
end #c:TS_StrongTyping


##
## Helpers
##

def blank
end #m:blank

def foo(a, b, c)
  expect(a, String, b, Integer, c, String)
  print "#{a} found #{b} #{c}\n"
end

def optional(a, b = nil)
  expect(a, Integer, b, [Integer, NilClass])
  return a + b   if b
  return a
end

def optional2(a, b)
  expect(a, [Integer, NilClass], b, Integer)
end

# useful, but demonstrates it works
def add (*args)
  overload(args) { return }
  overload(args, Integer, Integer) { | a, b | return }
  overload(args, String, String)   { | a, b | return }

  overload_exception(args, String, Integer) {
    | a, b |
    raise "I'm not going there."
  }

  overload_default args
end
