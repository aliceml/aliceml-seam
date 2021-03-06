//
// Author:
//   Thorsten Brunklaus <brunklaus@ps.uni-sb.de>
//
// Copyright:
//   Thorsten Brunklaus, 2000-2001
//
// Last Change:
//   $Date$ by $Author$
//   $Revision$
//
#ifndef __STORE__GCHELPER_HH__
#define __STORE__GCHELPER_HH__

#if defined(INTERFACE)
#pragma interface "store/GCHelper.hh"
#endif

class GCHelper {
public:
  // Moving Helper
  static u_int AlreadyMoved(Block *p) {
    return (!((reinterpret_cast<u_int *>(p)[0] & GEN_GC_MASK) >> GEN_GC_SHIFT));
  }
  static Block *GetForwardPtr(Block *p) {
    return reinterpret_cast<Block **>(p)[0];
  }
  static void MarkMoved(Block *p, Block *np) {
    Assert((reinterpret_cast<u_int>(np) & GEN_GC_MASK) >> GEN_GC_SHIFT == 0);
    reinterpret_cast<Block **>(p)[0] = np;
  }
  // Generation Encoding
  static void EncodeGen(Block *p, u_int gen) {
    reinterpret_cast<u_int *>(p)[0] = (reinterpret_cast<u_int *>(p)[0] & ~GEN_GC_MASK) | ((gen + 1) << GEN_GC_SHIFT);
  }
  static u_int DecodeGen(Block *p) {
    return ((reinterpret_cast<u_int *>(p)[0] & GEN_GC_MASK) >> GEN_GC_SHIFT);
  }
  // like PointerOp::Deref, but be careful about forwards
  static word Deref(word v) {
  loop:
    u_int vi = reinterpret_cast<u_int>(v);

    if (!((vi ^ TRTAG) & TAGMASK)) {
      vi ^= static_cast<u_int>(TRTAG);
      
      if (!AlreadyMoved(reinterpret_cast<Block *>(vi)) &&
	    HeaderOp::DecodeLabel(reinterpret_cast<Block *>(vi)) == REF_LABEL) {
        v = reinterpret_cast<word *>(vi)[1];
        goto loop;
      }
    }

    return v;
  }
};

#endif
