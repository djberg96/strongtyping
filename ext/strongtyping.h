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

#ifndef __GNUC__
     /* We can use this extension to get rid of some superfluous warnings */
#    define __attribute__(x)
#    define MAXARGS (128)
#else
#    define MAXARGS (argc/2)
#endif

#define UNUSED __attribute__ ((unused))

VALUE mStrongTyping;
VALUE cQueryParams;
VALUE eArgumentTypeError, eOverloadError, eArgList;
ID    id_isa, id_class, id_inspect;

