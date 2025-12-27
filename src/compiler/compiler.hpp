#pragma once

#include "../codegen/codegen.hpp"
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
#include <memory>

using namespace std;
using namespace llvm;
using namespace llvm::orc;

namespace AplCompiler {

class JITCompiler {
private:
  std::unique_ptr<ExecutionSession> session;
  DataLayout dataLayout;
  MangleAndInterner mangle;
  RTDyldObjectLinkingLayer objectLayer;
  ResourceTrackerSP rt;
  IRCompileLayer compileLayer;
  JITDylib &mainJD;

public:
  JITCompiler(std::unique_ptr<ExecutionSession> session,
              JITTargetMachineBuilder jtmb, DataLayout dataLayout);
  ~JITCompiler();
  static unique_ptr<JITCompiler> create();
  const DataLayout &getDataLayout() const;
  JITDylib &getMainJITDylib();
  Constants::CompilerFunc compile(AplCodegen::LlvmCodegen *codegenManager);
};
} // namespace AplCompiler