//
// Author:
//   Thorsten Brunklaus <brunklaus@ps.uni-sb.de>
//
// Copyright:
//   Thorsten Brunklaus, 2002
//
// Last Change:
//   $Date$ by $Author$
//   $Revision$
//

#if defined(INTERFACE)
#pragma implementation "store/Memory.hh"
#endif

#include "store/Store.hh"
#include "store/Memory.hh"

#if defined(__MINGW32__) || defined(_MSC_VER)
#include <windows.h>

char *MemChunk::Alloc(u_int size) {
  return (char *) VirtualAlloc(NULL, size,
			       (MEM_RESERVE | MEM_COMMIT),
			       PAGE_READWRITE);
}

void MemChunk::Free(char *p) {
  if (p && VirtualFree(p, 0, MEM_RELEASE) != TRUE) {
    // to be done
  }
}
#else
#include <cstdlib>
#include <unistd.h>
#include <sys/mman.h>
char *MemChunk::Alloc(u_int size) {
#if defined(STORE_ALLOC_MMAP)
  return (char *) mmap(NULL, size,
		       (PROT_READ | PROT_WRITE), MAP_PRIVATE,
		       -1, (off_t) 0);
#else
  return (char *) std::malloc(size);
#endif
}

void MemChunk::Free(char *p) {
#if defined(STORE_ALLOC_MMAP)
  munmap((char *) p, (u_int) (max - base));
#else
  std::free(p);
#endif
}
#endif