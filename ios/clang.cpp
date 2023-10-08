#include <llvm/Support/LLVMDriver.h>
#include <llvm/Support/CommandLine.h>

extern int clang_main(int argc, char **argv, const llvm::ToolContext &);

extern "C" {
extern void LLVMInitializeWebAssemblyTargetInfo();
extern void LLVMInitializeWebAssemblyTarget();
extern void LLVMInitializeWebAssemblyTargetMC();

__attribute__ ((visibility("default"))) int clang_hook(int argc, char **argv) {
    LLVMInitializeWebAssemblyTargetInfo();
    LLVMInitializeWebAssemblyTarget();
    LLVMInitializeWebAssemblyTargetMC();
    llvm::cl::ResetAllOptionOccurrences();
    return clang_main(argc, (char**) argv, {argv[0], nullptr, false});
}
}
