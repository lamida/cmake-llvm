#include <llvm/ADT/Twine.h>
#include <llvm/Support/raw_ostream.h>

int main() {
    llvm::Twine t = "Hello LLVM";
    llvm::outs() << t << "\n";
    return 0;
}
