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

extern "C" {
#include <sys/types.h>
#include <sys/socket.h>
}

#if defined(INTERFACE)
#pragma implementation "emulator/IOHandler.hh"
#endif

#include "emulator/IOHandler.hh"

void IOHandler::Init() {
}

void IOHandler::Poll() {
}

void IOHandler::Block() {
}