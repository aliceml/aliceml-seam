//
// Author:
//   Thorsten Brunklaus <brunklaus@ps.uni-sb.de>
//
// Copyright:
//   Thorsten Brunklaus, 2000
//
// Last Change:
//   $Date$ by $Author$
//   $Revision$
//

#if defined(INTERFACE)
#pragma implementation "store/WeakDictionary.hh"
#endif
#include "store/WeakDictionary.hh"

#include <cstring>

//
// Internal Helper Functions
//

static inline int IsPrime(u_int p) {
  if ((p % 2) == 0) {
    return 0;
  }
  
  for (u_int i = 3; (i * i) <= p; i += 2) {
    if ((p % i) == 0) {
      return 0;
    }
  }

  return 1;
}

// String hashing function is taken from 'Aho, Sethi, Ullman: Compilers..., page 436
static inline u_int HashString(const char *s, u_int len, u_int size) {
  const char *sm = (s + len);
  unsigned h = 0, g;

  for (const char *p = s; p < sm; p++) {
    h = (h << 4) + (*p);
    if ((g = h & 0xf0000000)) {
      h = h ^ (g >> 24);
      h = h ^ g;
    }
  }

  return (h % size);
}

static inline u_int HashBlock(Block *b, u_int size) {
  return HashString((const char *) b->GetBase(), b->GetSize() * sizeof(word), size);
}

static inline int CompareBlocks(Block *a, Block *b) {
  u_int al = (a->GetSize() * sizeof(word));
  u_int bl = (b->GetSize() * sizeof(word));

  if (al == bl) {
    return std::memcmp((char *) a->GetBase(), (char *) b->GetBase(), al);
  }

  return 1;
}

//
// Internal Helper Methods
//

inline u_int WeakDictionary::IncKey(u_int key, u_int size) {
  key += INC_STEP;
  if (key > size) {
    key -= size;
  }

  return key;
}

inline u_int WeakDictionary::NextPrime(u_int p) {
  if (p <= INC_STEP) {
    p = (INC_STEP + 2);
  }
  if ((p % 2) == 0) {
    p++;
  }
  
  while (!IsPrime(p)) {
    p += 2;
  }

  return p;
}

inline u_int WeakDictionary::FindKey(u_int i) {
  u_int size = GetTableSize();
  u_int key  = (1 + (i % size));

 loop:
  HashNode *entry = GetEntry(key);

  if ((!entry->IsEmpty()) && ((u_int) entry->GetIntKey() != i)) {
    key = IncKey(key, size);
    goto loop;
  }

  return key;
}

inline u_int WeakDictionary::FindKey(Block *b) {
  u_int size = GetTableSize();
  u_int key  = (1 + HashBlock(b, size));

 loop:
  HashNode *entry = GetEntry(key);
    
  if ((!entry->IsEmpty()) && (CompareBlocks(entry->GetBlockKey(), b) != 0)) {
    key = IncKey(key, size);
    goto loop;
  }

  return key;
}

inline u_int WeakDictionary::FindKey(word key) {
  switch (GetKeyType()) {
  case INT_KEY:
    return FindKey((u_int) Store::WordToInt(key));
  case BLOCK_KEY:
    return FindKey(Store::WordToBlock(key));
  }

  // this is unsafe -- to be determined
  return 0;
}

void WeakDictionary::Resize() {
  u_int oldsize = GetTableSize();
  u_int newsize = NextPrime(oldsize << 1);
  u_int percent = (u_int) (1 + (newsize * FILL_RATIO));
  Block *oldp   = GetTable();
  Block *newp   = Store::AllocBlock((BlockLabel) HASHNODEARRAY_LABEL, newsize);

  SetCounter(0);
  SetPercent(percent);
  ReplaceArg(TABLE_POS, newp->ToWord());

  for (u_int i = 1; i <= newsize; i++) {
    newp->InitArg(i, HashNode::New()->ToWord());
  }
  // this should be optimized later
  for (u_int i = 1; i <= oldsize; i++) {
    HashNode *entry = HashNode::FromWord(oldp->GetArg(i));
    
    if (!entry->IsEmpty()) {
      InsertItem(entry->GetKey(), entry->GetValue());
    }
  }
}

//
// Public Interface Methods
//

WeakDictionary *WeakDictionary::New(hashkeytype type, BlockLabel l, u_int size, word handler) {
  size = NextPrime(size);

  Block *p      = Store::AllocBlock(l, SIZE);
  Block *arr    = Store::AllocBlock(HASHNODEARRAY_LABEL, size);
  u_int percent = (u_int) (1 + (size * FILL_RATIO));

  if (handler != Store::IntToWord(0)) {
    HeaderOp::SetHandlerMark(p);
  }

  p->InitArg(HANDLER_POS, handler);
  p->InitArg(COUNTER_POS, Store::IntToWord(0));
  p->InitArg(PERCENT_POS, Store::IntToWord(percent));
  p->InitArg(TYPE_POS, Store::IntToWord(type));
  p->InitArg(TABLE_POS, arr->ToWord());
  
  for (u_int i = 1; i <= size; i++) {
    arr->InitArg(i, HashNode::New()->ToWord());
  }
  
  return (WeakDictionary *) p;
}

void WeakDictionary::InsertItem(word key, word value) {
  u_int counter = GetCounter();
  u_int percent = GetPercent();

  if (counter > percent) {
    Resize();
  }

  u_int keyval    = FindKey(key);
  HashNode *entry = GetEntry(keyval);

  if (entry->IsEmpty()) {
    SetCounter((counter + 1));
  }
  
  entry->SetKey(key);
  entry->SetValue(value);
}

void WeakDictionary::DeleteItem(word key) {
  u_int keyval    = FindKey(key);
  HashNode *entry = GetEntry(keyval);

  if (!entry->IsEmpty()) {
    entry->MakeEmpty();
  }
}

int WeakDictionary::IsMember(word key) {
  u_int keyval    = FindKey(key);
  HashNode *entry = GetEntry(keyval);

  return !(entry->IsEmpty());
}

word WeakDictionary::GetItem(word key) {
  u_int keyval    = FindKey(key);
  HashNode *entry = GetEntry(keyval);
  
  return entry->GetValue(); // should raise invalid_key something
}