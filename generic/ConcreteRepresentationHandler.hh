//
// Authors:
//   Thorsten Brunklaus <brunklaus@ps.uni-sb.de>
//   Leif Kornstaedt <kornstae@ps.uni-sb.de>
//
// Copyright:
//   Thorsten Brunklaus, 2000
//   Leif Kornstaedt, 2000
//
// Last Change:
//   $Date$ by $Author$
//   $Revision$
//

#ifndef __GENERIC__CONCRETE_REPRESENTATION_HH__
#define __GENERIC__CONCRETE_REPRESENTATION_HH__

class Block;

class ConcreteRepresentationHandler {
public:
  // returns INVALID_POINTER if there is none
  virtual Block *GetAbstractRepresentation(Block *blockWithHandler) = 0;
};

#endif