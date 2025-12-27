#pragma once

namespace Constants {
typedef float *(*CompilerFunc)();
inline const char *anonymousExprName = "__anon_expr";
inline const char *heapAllocFuncName = "malloc";
inline const char *moduleName = "apl";
} // namespace Constants