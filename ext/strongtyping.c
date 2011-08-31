/*
  StrongTyping - Method parameter checking for Ruby
  Copyright (C) 2003  Ryan Pavlik

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
*/

#include "ruby.h"
#include "strongtyping.h"

/* Ruby 1.9.x */
#ifndef RSTRING_PTR
#define RSTRING_PTR(s) (RSTRING(s)->ptr)
#endif
#ifndef RSTRING_LEN
#define RSTRING_LEN(s) (RSTRING(s)->len)
#endif

#ifndef RARRAY_PTR
#define RARRAY_PTR(a) (RARRAY(a)->ptr)
#endif
#ifndef RARRAY_LEN
#define RARRAY_LEN(a) (RARRAY(a)->len)
#endif

static int check_args(int argc, VALUE *obj, VALUE *mod);

static VALUE strongtyping_expect(int argc, VALUE *argv, VALUE self UNUSED) {
  int i = 0;
  VALUE obj[MAXARGS], mod[MAXARGS];
  VALUE typestr;

  if(!argc)
    return Qnil;

  if(argc % 2)
    rb_raise(rb_eSyntaxError, "expect() requires argument pairs");

#ifndef __GNUC__
  if(argc*2 > MAXARGS*2)
    rb_raise(rb_eSyntaxError, "too many arguments to expect()");
#endif

  for(i = 0; i < argc; i += 2) {
    obj[i/2]     = argv[i];
    mod[(i+1)/2] = argv[i+1];
  }

  if(rb_funcall(obj[0], id_isa, 1, cQueryParams)) {
    rb_funcall(obj[0], rb_intern("<<"), 1, rb_ary_new4(argc/2, mod));
    rb_raise(eArgList, ""); // TODO: Why an empty string? Causes a warning in 1.9.x.
  }
    
  i = check_args(argc / 2, obj, mod);

  if(i < 0)
    return Qnil;

  typestr = rb_funcall(mod[i], id_inspect, 0);

  rb_raise(
    eArgumentTypeError,
    "Expecting %s as argument %d, got %s",
    RSTRING_PTR(typestr), i + 1,
    rb_class2name(rb_funcall(obj[i], id_class, 0))
  );
}

static VALUE strongtyping_overload(int argc, VALUE *argv, VALUE self UNUSED) {
  struct RArray *q;
    
  if(argc < 1)
    rb_raise(rb_eArgError, "At least one parameter required");

  Check_Type(argv[0], T_ARRAY);
  q = RARRAY(argv[0]);

  if(RARRAY_LEN(q) && rb_funcall(RARRAY_PTR(q)[0], id_isa, 1, cQueryParams)) {
    rb_funcall(RARRAY_PTR(q)[0], rb_intern("<<"), 1, rb_ary_new4(argc - 1, argv + 1));
    return Qnil;
  }

  if(RARRAY_LEN(q) != (argc - 1))
    return Qnil;

  if(check_args(argc - 1, RARRAY_PTR(q), argv + 1) < 0){
    if(argc == 2)
      rb_yield(*RARRAY_PTR(*argv));
    else
      rb_yield(*argv);
  }

  return Qnil;
}

static VALUE strongtyping_overload_exception(int argc, VALUE *argv, VALUE self UNUSED) {
  struct RArray *q;
    
  if(argc < 1)
    rb_raise(rb_eArgError, "At least one parameters required");

  Check_Type(argv[0], T_ARRAY);
  q = RARRAY(argv[0]);

  if(RARRAY_LEN(q) && (argc - 1) == 0)
    return Qnil;

  if(check_args(argc - 1, RARRAY_PTR(q), argv + 1) < 0)
    rb_yield(argv[0]);

  return Qnil;
}

static VALUE strongtyping_overload_error(VALUE self UNUSED, VALUE args) {
  struct  RArray *q;
  VALUE           classlist;
  const char      *name = 0;
  int             i    = 0;
    
  Check_Type(args, T_ARRAY);
  q = RARRAY(args);
    
  if(RARRAY_LEN(q) && rb_funcall(RARRAY_PTR(q)[0], id_isa, 1, cQueryParams))
    rb_raise(eArgList, "");

  classlist = rb_str_new2("");

  for(i = 0; i < RARRAY_LEN(q); i++) {
    if(i > 0)
      rb_str_cat(classlist, ", ", 2);

    name = rb_class2name(rb_funcall(RARRAY_PTR(q)[i], id_class, 0));
    rb_str_cat(classlist, name, strlen(name));
  }

  rb_raise(
    eOverloadError,
    "No matching template for arguments: [%s]",
    RSTRING_PTR(classlist)
  );
}

static int check_args(int argc, VALUE *obj, VALUE *mod) {
  int i = 0;
  VALUE ret;
    
  for(i = 0; i < argc; i++){
    if(TYPE(mod[i]) == T_ARRAY){
      int j = 0, ok = 0;

      for(j = 0; j < RARRAY_LEN(mod[i]); j++){
        if(rb_funcall(obj[i], id_isa, 1, RARRAY_PTR(mod[i])[j]) == Qtrue)
          ok = 1;
      }

      if(ok)
        continue;
      else
        return i;
    }
    else{
      ret = rb_funcall(obj[i], id_isa, 1, mod[i]);
      if(ret == Qfalse) return i;
    }
  }

  return -1;
}

static VALUE call_method(VALUE ary) {
  VALUE  method = RARRAY_PTR(ary)[0],
         query  = RARRAY_PTR(ary)[1];
  VALUE *argv   =  NULL;
  VALUE  ret;
  int    argc   =  0,
         i      =  0;

  argc = FIX2INT(rb_funcall(method, rb_intern("arity"), 0));

  if(argc == 0) {
    rb_funcall(query, rb_intern("<<"), 1, rb_ary_new());
    rb_raise(eArgList, "");
  }
  else if(argc < 0)
    argc = -argc;

  argv    = malloc(sizeof(VALUE) * argc);
  argv[0] = query;

  for(i = 1; i < argc - 1; i++)
    argv[i] = Qnil;

  ret = rb_funcall2(method, rb_intern("call"), argc, argv);
  free(argv);

  return ret;
}

static VALUE grab_types(VALUE query) {
  return query;
}

static VALUE strongtyping_get_arg_types(VALUE obj UNUSED, VALUE method) {
  VALUE query, ary;
  query = rb_funcall(cQueryParams, rb_intern("new"), 0);
  ary   = rb_ary_new3(2, method, query);
    
  return rb_rescue2(call_method, ary, grab_types, query, eArgList, 0);
}

static VALUE strongtyping_verify_args_for(VALUE self, VALUE method, VALUE args) {
  struct RArray *list = NULL;
  struct RArray *t    = NULL;
  struct RArray *a    = NULL;
  int i    = 0;
  VALUE template = strongtyping_get_arg_types(self, method);

  list = RARRAY(template);
  a    = RARRAY(args);

  for(i = 0; i < RARRAY_LEN(list); i++){
    t = RARRAY_PTR(list)[i];

    if(RARRAY_LEN(a) != RARRAY_LEN(t))
      continue;

    if(check_args(RARRAY_LEN(a), RARRAY_PTR(a), RARRAY_PTR(t)) < 0)
      return Qtrue;
  }
    
  return Qfalse;
}

void Init_strongtyping() {
    mStrongTyping = rb_define_module("StrongTyping");
    id_isa        = rb_intern("is_a?");
    id_class      = rb_intern("class");
    id_inspect    = rb_intern("inspect");

    cQueryParams       = rb_define_class_under(mStrongTyping,
                                               "%QueryParams",
                                               rb_cArray);

    eArgumentTypeError = rb_define_class_under(mStrongTyping,
                                               "ArgumentTypeError",
                                               rb_eArgError);
    eOverloadError     = rb_define_class_under(mStrongTyping,
                                               "OverloadError",
                                               eArgumentTypeError);
    eArgList           = rb_define_class_under(mStrongTyping,
                                               "%ArgList",
                                               rb_eException);

    rb_define_module_function(mStrongTyping, "expect",
                              strongtyping_expect, -1);
    rb_define_module_function(mStrongTyping, "overload",
                              strongtyping_overload, -1);
    rb_define_module_function(mStrongTyping, "overload_exception",
                              strongtyping_overload_exception, -1);
    rb_define_module_function(mStrongTyping, "overload_default",
                              strongtyping_overload_error, 1);
    rb_define_module_function(mStrongTyping, "overload_error",
                              strongtyping_overload_error, 1);
    rb_define_module_function(mStrongTyping, "get_arg_types",
                              strongtyping_get_arg_types, 1);
    rb_define_module_function(mStrongTyping, "verify_args_for",
                              strongtyping_verify_args_for, 2);
}
