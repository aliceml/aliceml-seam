//
// Author:
//   Leif Kornstaedt <kornstae@ps.uni-sb.de>
//
// Copyright:
//   Leif Kornstaedt, 2002
//
// Last Change:
//   $Date$ by $Author$
//   $Revision$
//

#ifndef __GENERIC__CONCRETE_REPRESENTATION_HH__
#define __GENERIC__CONCRETE_REPRESENTATION_HH__

#if defined(INTERFACE)
#pragma interface "generic/ConcreteRepresentation.hh"
#endif

#include "store/Store.hh"
#include "generic/ConcreteRepresentationHandler.hh"

class ConcreteRepresentation: private Block {
protected:
  static const u_int HANDLER_POS = 0;
  static const u_int BASE_SIZE   = 1;
public:
  using Block::ToWord;

  static ConcreteRepresentation *New(ConcreteRepresentationHandler *handler,
				     u_int size) {
    Block *b = Store::AllocBlock(CONCRETE_LABEL, BASE_SIZE + size);
    b->InitArg(HANDLER_POS, Store::UnmanagedPointerToWord(handler));
    return static_cast<ConcreteRepresentation *>(b);
  }
  static ConcreteRepresentation *FromWord(word x) {
    Block *b = Store::WordToBlock(x);
    Assert(b == INVALID_POINTER || b->GetLabel() == CONCRETE_LABEL);
    return static_cast<ConcreteRepresentation *>(b);
  }
  static ConcreteRepresentation *FromWordDirect(word x) {
    Block *b = Store::DirectWordToBlock(x);
    Assert(b->GetLabel() == CONCRETE_LABEL);
    return static_cast<ConcreteRepresentation *>(b);
  }

  ConcreteRepresentationHandler *GetHandler() {
    return static_cast<ConcreteRepresentationHandler *>
      (Store::DirectWordToUnmanagedPointer(GetArg(HANDLER_POS)));
  }
  void Init(u_int index, word value) {
    InitArg(BASE_SIZE + index, value);
  }
  word Get(u_int index) {
    return GetArg(BASE_SIZE + index);
  }
  void Replace(u_int index, word value) {
    return ReplaceArg(BASE_SIZE + index, value);
  }
  u_int GetSize() {
    return Block::GetSize() - BASE_SIZE;
  }
};

#endif
