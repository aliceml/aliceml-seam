//
// Authors:
//   Thorsten Brunklaus <brunklaus@ps.uni-sb.de>
//   Leif Kornstaedt <kornstae@ps.uni-sb.de>
//
// Copyright:
//   Thorsten Brunklaus, 2002
//   Leif Kornstaedt, 2002
//
// Last Change:
//   $Date$ by $Author$
//   $Revision$
//

#ifndef __GENERIC__PRIMITIVE_HH__
#define __GENERIC__PRIMITIVE_HH__

#if defined(INTERFACE)
#pragma interface "generic/Primitive.hh"
#endif

#include "generic/Interpreter.hh"

class Primitive {
public:
  typedef Interpreter::Result (*function)(TaskStack *);

  static word MakeFunction(const char *name, function function,
			   u_int arity, bool sited = false);
  static word MakeClosure(const char *name, function function,
			  u_int arity, bool sited = false);
};

#endif
