#include "compiler.hpp"
#include "../constants.hpp"

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

#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include <iostream>
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
  if (jtmb.getTargetTriple().isOSBinFormatCOFF()) {
    this->objectLayer.setOverrideObjectFlagsWithResponsibilityFlags(true);
    this->objectLayer.setAutoClaimResponsibilityForObjectSymbols(true);
  }
  this->rt = nullptr;
}

JITCompiler::~JITCompiler() {
  if (auto Err = this->session->endSession())
    session->reportError(std::move(Err));
}

unique_ptr<JITCompiler> JITCompiler::create() {
  // Prepare the environment to generate code for the machine
  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();
  InitializeNativeTargetAsmParser();

  auto EPC = SelfExecutorProcessControl::Create();
  if (!EPC)
    return nullptr;

  auto session = make_unique<ExecutionSession>(std::move(*EPC));

  JITTargetMachineBuilder jtmb(
      session->getExecutorProcessControl().getTargetTriple());

  auto dataLayout = jtmb.getDefaultDataLayoutForTarget();
  if (!dataLayout)
    return nullptr;

  return make_unique<JITCompiler>(std::move(session), std::move(jtmb),
                                  std::move(*dataLayout));
}

const DataLayout &JITCompiler::getDataLayout() const {
  return this->dataLayout;
}

JITDylib &JITCompiler::getMainJITDylib() { return this->mainJD; }

Constants::CompilerFunc
JITCompiler::compile(AplCodegen::LlvmCodegen *codegenManager,
                     AplCodegen::RValue returnExpr) {

  if (this->rt != nullptr)
    Error error1 = this->rt->remove();
  this->rt = this->mainJD.getDefaultResourceTracker();

  codegenManager->returnCodegen(returnExpr.getResultPtr());
  auto [context, module] = codegenManager->getAndReinitializeContextAndModule();
  module->print(errs(), nullptr);

  auto tsm = llvm::orc::ThreadSafeModule(std::move(module), std::move(context));
  Error error = this->compileLayer.add(this->rt, std::move(tsm));
  auto Sym = this->session
                 ->lookup({&this->mainJD}, mangle(Constants::anonymousExprName))
                 .get();
  auto *fp = Sym.toPtr<Constants::CompilerFunc>();
  return fp;
}
} // namespace AplCompiler