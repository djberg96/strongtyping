# Disclaimer
Most of this document was written by Ryan Pavlik, including the Q&A below.
It does not necessarily reflect my own opinions. I am just the maintainer.

## Description
The strongtyping library is a provides a convenient way for Ruby methods
to check parameter types, and also dynamically query them. In addition to
merely checking a single set of types, it allows easy overloading based on
a number of different templates.

## Installation
`gem install strongtyping`

## Synopsis
Let's say you have the following function:

```ruby
def foo(a, b)
  ...
end
```

Now let's say this function wants 'a' to always be a String, and 'b'
should be Numeric:

```ruby
require 'strongtyping'
include StrongTyping

def foo(a, b)
  expect(a, String, b, Numeric)
  ...
end
```

If 'a' or 'b' is of the wrong type, an ArgumentTypeError will be raised.

Overloading is just as easy:

```ruby
require 'strongtyping'
include StrongTyping

def bar(*args)
  overload(args, String, String){ |s1,s2|
    ...
    return
  }

  overload(args, String, Integer){ |s,i|
    ...
    return
  }

  overload_default args
end
```

If someone calls 'bar' with two Strings, or a String and an Integer,
the appropriate block will be called.  Otherwise, an OverloadError
is raised.

How about default parameters? Say we have the following function:

```ruby
def baz(a, b = nil)
  action  = "You baz #{a}";
  action += " with a #{b}"  if b

  print action, "\n"
end
```

Now, b can either be nil or a String. We don't want to have two
full overload cases... that would duplicate code. So, expect()
allows an array of types:

`expect(a, String, b, [String, NilClass])`

This takes care of the above case nicely.

What if your code is curious about which types are allowed? The
get_arg_types function is provided for just this purpose.  Given the
 above definitions for 'foo' and 'bar', consider the following code:

```ruby
p get_arg_types(method(:foo))  # => [[String, Numeric]]
p get_arg_types(method(:bar))  # => [[String, String], [String, Integer]]
p get_arg_types(method(:baz))  # => [[String, [String, NilClass]]]
```
      
This is useful if you're converting user input into a form that the
method expects.  (If you get "1234", should you convert it to an
integer, or is it best left a string? Now you know.)

What if you have an array of arguments, 'arr', and you're worried
that the method 'bar' won't accept them?  You can check ahead of
time:

```ruby
if not verify_args_for(method(:bar), arr)
  print "I can't let you do that, Dave\n"
end
```

## Reference
Module: StrongTyping

Methods:
  
`expect(obj0, Module0[, obj1, Module1[,...objN, ModuleN]])`

Verify the parameters obj0..objN are of the given class (or
module) Module0..ModuleN

`overload(args, [Module0[, Module1[,...ModuleN]]]) { | o0, o1,..oN | }`

Call the block with 'args' if they match the pattern
Module0..ModuleN.  The block should _always_ call return at the
end.

`overload_exception(args, [Module0[,...ModuleN]]]) { | o0, o1,..oN | }`

This acts identically to overload(), except the case specified
is considered invalid, and thus not returned by get_arg_types().
It is expected that the specified block will throw an exception.

```
overload_default(args)       
overload_error(args)
```
    
Raise OverloadError. This should _always_ be called after the
last overload() block. In addition to raising the exception,
it aids in checking parameters. As of 2.0, the overload_error
name is deprecated; use overload_default.

`get_arg_types(Method)`

Return an array of parameter templates.  This is an array of
arrays, and will have multiple indices for functions using
multiple overload() blocks.

`verify_args_for(method, args)`

Verify the method 'method' will accept the arguments in array
'args', returning a boolean result.

Exceptions:

`ArgumentTypeError < ArgumentError`

This exception is raised by expect() if the arguments do not
match the expected types.

`OverloadError < ArgumentTypeError`

This exception is raised by overload_default() if no overload()
template matches the given arguments.
       
## Q & A
This section written by Ryan Pavlik.

Q: Why?
A: Because it was originally needed for the Mephle library.

Q: No really, why bother with static typing? Isn't ruby dynamic?
A: This is not 'static typing'. This is 'strong typing'. Static
   typing is what you get when a variable can only be of a certain
   type, as in C or C++. Strong typing is enforcing types. These may
   seem similar, but they are actually not directly related.

   Some other languages, such as Common Lisp, allow for dynamic,
   strong typing. Strong typing and dynamic typing are not mutually
   exclusive.

Q: Yeah, but really, why bother?  Why not just let ruby sort out the
   errors as they occur?  
A: This is incorrect thinking. Allowing errors to just occur when
   they happen is naive programming. Consider the following:

```ruby
   # Wait N seconds, then open the bridge for M seconds
   def sendMsg(bridge, n, m)
     sleep(n)
     bridge.open
     sleep(m)
     bridge.close
   end
```

   Now say 'm' is pased in as a string. Oops! A TypeError is
   raised. Now the bridge is open, and somewhere (hopefully!) someone
   caught the exception so the program didn't crash, but the bridge
   opening wasn't reversed, so it's going to stay open and back up
   traffic until someone fixes the problem.

   This is an academic example, but there are many cases when just
   letting an error happen will lead to an inconsistent system state.
   Ruby (and most systems) are not transactional, and inconsistent
   states are unacceptable.

   In addition, it is desireable to know _programmatically_ why
   something failed, as specific action can be taken if desired.

   "Wait," someone in the audience says, "you could just check to see
   if 'm' and 'n' are of the correct type!"

   Yes, yes you could.

   That's what this module is for. ;-)

Q: Isn't it up to the caller to call my function correctly?
A: The caller cannot know and deal with errors that may occur in your
   code. That's your job. Checking for errors ahead of time and
   informing the caller about problems is also your job. This module
   just makes it easy.

   In addition, it's nice for the caller to be able to ask and check
   what your method expects ahead of time to guard against error.
   The strongtyping module also provides functionality for this.

Q: OK, but strong typing is baaad.  What if I want to pass something
   that acts like something else, or responds to a given symbol?
   Doesn't ruby have "duck" typing?
A: First, what you're suggesting is evil. If you want that, go
   write C++. :-)

   Second, you should never depend on a function's implementation.  If
   the documentation says "pass me a hash" and you pass it anything
   that responds to :[], your code may break when the next version
   comes out.

   Third, if you pass something that responds accurately to the
   _interface_ (methods provided by class or module) specified, then
   that should be _of_ that class or module.  This may not be the case
   with all ruby objects yet; for instance, anything responding to :[]
   being something like a Mappable.  You can make this the case in
   your code, or urge developers to create a standard set of interface
   mixins for just this purpose.

   "Duck" typing just a term for this sort of "maybe" behavior, much
   like what C++ STL templates use.  However, the problem is that even
   if an object responds to a method, there is no guarantee that the
   method acts in an expected manner---and the interface may still
   change without notice.  "Duck" typing sounds much like 'duct
   taping' depending on your accent, and I think duct-taping is a good
   description of this is in practice. :-)

   Another argument is that ruby allows one to change the behavior of
   methods at any time:

```ruby
   a = String.new;
   def a.split
     print "hello world\n"
   end
```

   For this, I have two responses:  first, if a method is deprecated
   or changed dramatically, strongtyping can aid in letting the code
   know:

```ruby
   a = String.new;
   def a.split(*args)
     overload(args) { print "hello world\n" }
     overload_default args
   end
```

   This case will drop any normal calls through to overload_default,
   raising an exception, which can be caught and analyzed.  You can
   even provide another case that calls the superclass.

   Second, either you're changing the method in a subtle manner (it
   does what it used to, with added effect), or an outrageous manner
   (it acts nothing like it did before).  In the former case, code
   should work fine anyway.  In the latter case, as in the above
   example, you should ask yourself why you're changing it.  The
   function no longer splits, why is it called split?  This is not
   good design; the strongtyping module is here to aid in good design,
   not prevent poor design.

   A more realistic example would be the academic "Shape" class
   example of inheritance, with "Ellipse" and "Circle".  Ruby properly
   allows one to make Circle a subclass of Ellipse, and redefine
   "setSize" to the constrained definition of a circle.  This change
   is visible to code---an ArgumentError will be raised (2 arguments
   for 1), or setSize can throw a ConstraintError.  strongtyping
   provides a useful function, overload_exception, for just this case:

```ruby
   class Circle < Ellipse
     def setSize(*args)
       overload(args, Integer) { |r|
         @radius = r
           return
       }

       overload_exception(args, Integer, Integer) { |a,b|
         raise ConstraintError
       }
        
       overload_default args
     end
   end
```

   Of course, there are a number of good choices for handling
   this... you may still allow `#setSize(a, b) if a == b`. The
   important part is that the change in behavior can now be determined
   by code.

Q: But I always write perfect code. I know what my functions do, and
   what they take, and what I'm passing them.
A: No one writes perfect code. Additionally, not all environments are
   as controlled as yours may be. Especially in a networked
   environment when someone may be invoking a method remotely, you
   can't depend on calling code not to be malicious.

Q: OK, OK. But, uh... what is Mephle?
A: Mephle is a soon-to-be-released network-transparent persistant
   object system written in ruby. It uses many of the Unity concepts
   (http://unity-project.sf.net/). It will be on the RAA when
   released.

DJB: The RAA is long dead of course, and the SF link looks like a blank project.
   As for Mephle, I don't think it was ever released and I'm not sure
   what Unity is, unless it's a reference to what is now known as Unity
   MLAPI, the networking library behind Unity3d.

## License
strongtyping - Method parameter checking for Ruby Copyright (C) 2003-2011 Ryan Pavlik

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA

## Future Plans (obsolete)
There's some odd C code that is causing a couple of warnings, but it
does not appear to be harmful. I want to clean this code up eventually.

## See Also
The Crystal programming language.

## Authors
* Ryan Pavlik (original author)
* Daniel Berger (maintenance)
