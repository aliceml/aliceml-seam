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
#ifndef __STORE__WEAKDICTIONARY_HH__
#define __STORE__WEAKDICTIONARY_HH__

#if defined(INTERFACE)
#pragma interface "store/WeakDictionary.hh"
#endif

#include "store/Store.hh"

class HashNode : private Block {
private:
  static const u_int SIZE    = 2;
  static const u_int KEY_POS = 1;
  static const u_int VAL_POS = 2;
public:
  using Block::ToWord;

  void MakeEmpty() {
    InitArg(KEY_POS, -1);
    InitArg(VAL_POS, 0);
  }
  int IsEmpty() {
    return (Store::WordToInt(GetArg(KEY_POS)) == - 1);
  }
  word GetKey() {
    return GetArg(KEY_POS);
  }
  int GetIntKey() {
    return Store::WordToInt(GetKey());
  }
  Block *GetBlockKey() {
    return Store::WordToBlock(GetKey());
  }
  void SetKey(word key) {
    ReplaceArg(KEY_POS, key);
  }
  word GetValue() {
    return GetArg(VAL_POS);
  }
  void SetValue(word value) {
    ReplaceArg(VAL_POS, value);
  }

  static HashNode *New() {
    Block *p = Store::AllocBlock(HASHNODE_LABEL, SIZE);

    p->InitArg(KEY_POS, -1);
    p->InitArg(VAL_POS, 0);

    return (HashNode *) p;
  }
  static HashNode *FromWord(word x) {
    Block *p = Store::DirectWordToBlock(x);

    Assert((p == INVALID_POINTER) || (p->GetLabel() == HASHNODE_LABEL));
    return (HashNode *) p;
  }
};

class WeakDictionary : private Block {
public:
  enum hashkeytype {
    INT_KEY,
    BLOCK_KEY
  };
  // this is to allow inlining
  u_int GetTableSize() {
    return (u_int) Store::DirectWordToBlock(GetArg(TABLE_POS))->GetSize();
  }
protected:
  static const u_int SIZE        = 5;
  static const u_int HANDLER_POS = 1;
  static const u_int COUNTER_POS = 2;
  static const u_int PERCENT_POS = 3;
  static const u_int TYPE_POS    = 4;
  static const u_int TABLE_POS   = 5;
  //
  // Adjust these two values to optimize runtime behaviour
  //
  static const u_int INC_STEP    = 5;
  static const double FILL_RATIO = 0.75;

  friend class Store;

  static u_int IncKey(u_int key, u_int size);
  static u_int NextPrime(u_int p);

  u_int FindKey(u_int i);
  u_int FindKey(Block *b);
  u_int FindKey(word key);
  void Resize();

  u_int GetCounter() {
    return (u_int) Store::DirectWordToInt(GetArg(COUNTER_POS));
  }
  void SetCounter(u_int counter) {
    InitArg(COUNTER_POS, counter);
  }
  u_int GetPercent() {
    return (u_int) Store::DirectWordToInt(GetArg(PERCENT_POS));
  }
  void SetPercent(u_int percent) {
    InitArg(PERCENT_POS, percent);
  }
  hashkeytype GetKeyType() {
    return (hashkeytype) Store::DirectWordToInt(GetArg(TYPE_POS));
  }
  Block *GetTable() {
    return Store::DirectWordToBlock(GetArg(TABLE_POS));
  }
  void SetTable(word t) {
    InitArg(TABLE_POS, t);
  }
  HashNode *GetEntry(u_int i) {
    Assert(i >= 1);
    Assert(i <= GetTableSize());
    return HashNode::FromWord(GetTable()->GetArg(i));
  }
  void RemoveEntry(HashNode *node) {
    node->MakeEmpty();
    SetCounter(GetCounter() - 1);
  }

  void InsertItem(word key, word value);
  void DeleteItem(word key);

  int IsMember(word key);
  word GetItem(word key); // must be member

  static WeakDictionary *New(hashkeytype type, BlockLabel l, u_int size, word handler);
public:
  using Block::ToWord;

  void InsertItem(int key, word value) {
    InsertItem(Store::IntToWord(key), value);
  }
  void DeleteItem(int key) {
    DeleteItem(Store::IntToWord(key));
  }
  int IsMember(int key) {
    return IsMember(Store::IntToWord(key));
  }
  word GetItem(int key) {
    return GetItem(Store::IntToWord(key));
  }

  u_int GetSize() {
    return (u_int) Store::WordToInt(GetArg(COUNTER_POS));
  }
  void Clear() {
    Block *arr = GetTable();
    u_int size = arr->GetSize();
    
    SetCounter(0);
    for (u_int i = 1; i <= size; i++) {
      HashNode::FromWord(arr->GetArg(i))->MakeEmpty();
    }
  }

  static WeakDictionary *New(u_int size, word handler) {
    WeakDictionary *d = WeakDictionary::New(INT_KEY, WEAK_DICT_LABEL, size, handler);

    Store::RegisterWeakDict(d);
    return d;
  } 
  static WeakDictionary *FromWord(word x) {
    Block *p = Store::WordToBlock(x);

    Assert((p == INVALID_POINTER) || (p->GetLabel() == WEAK_DICT_LABEL));
    return (WeakDictionary *) p;
  }
  static WeakDictionary *FromWordDirect(word x) {
    Block *p = Store::DirectWordToBlock(x);

    Assert((p == INVALID_POINTER) || (p->GetLabel() == WEAK_DICT_LABEL));
    return (WeakDictionary *) p;
  }
};

#endif __STORE__WEAKDICTIONARY_HH__