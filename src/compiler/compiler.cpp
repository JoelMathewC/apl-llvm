#include "compiler.hpp"

#include "llvm/ADT/StringRef.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/Core.h"
#include "llvm/ExecutionEngine/Orc/ExecutionUtils.h"
#include "llvm/ExecutionEngine/Orc/ExecutorProcessControl.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h"
#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/Orc/SelfExecutorProcessControl.h"
#include "llvm/ExecutionEngine/Orc/Shared/ExecutorSymbolDef.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/LLVMContext.h"
#include <memory>

using namespace llvm;
using namespace llvm::orc;
using namespace std;

namespace AplCompiler {
JITCompiler::JITCompiler(unique_ptr<ExecutionSession> session,
                         JITTargetMachineBuilder jtmb, DataLayout dataLayout)
    : session(std::move(session)), dataLayout(std::move(dataLayout)),
      mangle(*this->session, this->dataLayout),
      objectLayer(*this->session,
                  [](const MemoryBuffer &) {
                    return make_unique<SectionMemoryManager>();
                  }),
      compileLayer(*this->session, objectLayer,
                   make_unique<ConcurrentIRCompiler>(std::move(jtmb))),
      mainJD(this->session->createBareJITDylib("<main>")) {
  this->mainJD.addGenerator(
      cantFail(DynamicLibrarySearchGenerator::GetForCurrentProcess(
          dataLayout.getGlobalPrefix())));
}

JITCompiler::~JITCompiler() {
  if (auto Err = this->session->endSession())
    session->reportError(std::move(Err));
}

Expected<unique_ptr<JITCompiler>> JITCompiler::create() {
  auto EPC = SelfExecutorProcessControl::Create();
  if (!EPC)
    return EPC.takeError();

  auto session = make_unique<ExecutionSession>(std::move(*EPC));

  JITTargetMachineBuilder jtmb(
      session->getExecutorProcessControl().getTargetTriple());

  auto dataLayout = jtmb.getDefaultDataLayoutForTarget();
  if (!dataLayout)
    return dataLayout.takeError();

  return make_unique<JITCompiler>(std::move(session), std::move(jtmb),
                                  std::move(*dataLayout));
}

const DataLayout &JITCompiler::getDataLayout() const {
  return this->dataLayout;
}

JITDylib &JITCompiler::getMainJITDylib() { return this->mainJD; }

Error JITCompiler::addModule(ThreadSafeModule tsm,
                             ResourceTrackerSP rt = nullptr) {
  if (!rt)
    rt = this->mainJD.getDefaultResourceTracker();
  return this->compileLayer.add(rt, std::move(tsm));
}

Expected<ExecutorSymbolDef> JITCompiler::lookup(StringRef name) {
  return session->lookup({&this->mainJD}, mangle(name.str()));
}
} // namespace AplCompiler