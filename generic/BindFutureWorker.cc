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

#if defined(INTERFACE)
#pragma implementation "generic/ByneedInterpreter.hh"
#endif

#include <cstdio>
#include "generic/Backtrace.hh"
#include "generic/TaskStack.hh"
#include "generic/Scheduler.hh"
#include "generic/Transients.hh"
#include "generic/ByneedInterpreter.hh"

// ByneedFrame
class ByneedFrame: private StackFrame {
private:
  static const u_int FUTURE_POS = 0;
  static const u_int SIZE       = 1;
public:
  using Block::ToWord;

  // ByneedFrame Constructor
  static ByneedFrame *New(Interpreter *interpreter, Transient *future) {
    StackFrame *frame = StackFrame::New(BYNEED_FRAME, interpreter, SIZE);
    frame->InitArg(FUTURE_POS, future->ToWord());
    return static_cast<ByneedFrame *>(frame);
  }
  // ByneedFrame Untagging
  static ByneedFrame *FromWordDirect(word frame) {
    StackFrame *p = StackFrame::FromWordDirect(frame);
    Assert(p->GetLabel() == BYNEED_FRAME);
    return static_cast<ByneedFrame *>(p);
  }

  // ByneedFrame Accessors
  Future *GetFuture() {
    Transient *transient =
      Store::WordToTransient(StackFrame::GetArg(FUTURE_POS));
    Assert(transient != INVALID_POINTER &&
	   transient->GetLabel() == FUTURE_LABEL);
    return static_cast<Future *>(transient);
  }
};

//
// ByneedInterpreter Functions
//
ByneedInterpreter *ByneedInterpreter::self;

void ByneedInterpreter::PushFrame(TaskStack *taskStack, Transient *future) {
  taskStack->PushFrame(ByneedFrame::New(self, future)->ToWord());
}

static inline int IsCyclic(word x, Future *future) {
  return static_cast<Future *>(Store::WordToTransient(x)) == future;
}

Interpreter::Result ByneedInterpreter::Run(TaskStack *taskStack) {
  ByneedFrame *frame = ByneedFrame::FromWordDirect(taskStack->GetFrame());
  Future *future = frame->GetFuture();
  future->ScheduleWaitingThreads();
  Interpreter::Construct();
  word arg = Scheduler::currentArgs[0];
  if (IsCyclic(arg, future)) { // cancel future with Cyclic exception
    future->Become(CANCELLED_LABEL, Hole::cyclicExn);
    Scheduler::currentData = Hole::cyclicExn;
    Scheduler::currentBacktrace = Backtrace::New(frame->ToWord());
    return Interpreter::RAISE;
  } else { // actually bind the future
    taskStack->PopFrame();
    future->Become(REF_LABEL, arg);
    Scheduler::nArgs = Scheduler::ONE_ARG;
    Scheduler::currentArgs[0] = arg;
    return Interpreter::CONTINUE;
  }
}

Interpreter::Result
ByneedInterpreter::Handle(word exn, Backtrace *, TaskStack *taskStack) {
  Future *future =
    ByneedFrame::FromWordDirect(taskStack->GetFrame())->GetFuture();
  taskStack->PopFrame();
  future->ScheduleWaitingThreads();
  future->Become(CANCELLED_LABEL, exn);
  Scheduler::nArgs = Scheduler::ONE_ARG;
  Scheduler::currentArgs[0] = future->ToWord();
  return Interpreter::CONTINUE;
}

const char *ByneedInterpreter::Identify() {
  return "ByneedInterpreter";
}

void ByneedInterpreter::DumpFrame(word) {
  std::fprintf(stderr, "Byneed\n");
}
