#include <llvm/Support/LLVMDriver.h>

extern int lld_main(int argc, char **argv, const llvm::ToolContext &);

extern "C" {
__attribute__ ((visibility("default"))) int lld_hook(int argc, char **argv) {
    return lld_main(argc, (char**) argv, {argv[0], nullptr, false});
}
}
