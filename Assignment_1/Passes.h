#include "llvm/IR/PassManager.h"

namespace llvm {

// ────────────────────────────────────────────────────────────────────────────
// Pass 1 – Algebraic Identity
//   x + 0  =>  x        0 + x  =>  x
//   x * 1  =>  x        1 * x  =>  x
// ────────────────────────────────────────────────────────────────────────────
struct AlgebraicIdentityPass : PassInfoMixin<AlgebraicIdentityPass> {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);
    static bool isRequired() { return false; }
};

// ────────────────────────────────────────────────────────────────────────────
// Pass 2 – Strength Reduction
//   x * 15  =>  (x << 4) - x
//   x / 8   =>  x >> 3      
// ────────────────────────────────────────────────────────────────────────────
struct StrengthReductionPass : PassInfoMixin<StrengthReductionPass> {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);
    static bool isRequired() { return false; }
};

// ────────────────────────────────────────────────────────────────────────────
// Pass 3 – Multi-Instruction Optimization
//   a = b + 1
//   c = a - 1   =>  c = b
// ────────────────────────────────────────────────────────────────────────────
struct MultiInstructionOptPass : PassInfoMixin<MultiInstructionOptPass> {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);
    static bool isRequired() { return false; }
};

} // namespace llvm