/**
 * StrengthReduction.cpp
 *
 * Replaces expensive arithmetic with cheaper shift-based equivalents:
 *
 *   (A)  x * 15  =>  (x << 4) - x
 *        Generalised: x * (2^k - 1)  =>  (x << k) - x
 *
 *   (B)  x / C   =>  x >> log2(C)   (only for unsigned division)
 *        Handled for *unsigned* (udiv) exact powers of two.
 *        For sdiv the transformation would be incorrect for negative numbers,
 *        so it is not applied.
 *
 */

#include "Passes.h"

#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

// If V is a positive integer constant that equals 2^k - 1 for some k >= 1,
// return k; otherwise return 0.
static unsigned isPowerOfTwoMinusOne(const Value *V) {
  const auto *CI = dyn_cast<ConstantInt>(V);
  if (!CI || CI->isZero() || CI->isNegative())
    return 0;

  uint64_t N = CI->getZExtValue();
  // 2^k - 1 has exactly k trailing ones and no leading ones beyond bit k-1
  if ((N & (N + 1)) != 0)
    return 0; // not all-ones in the lower bits

  return static_cast<unsigned>(Log2_64(N + 1)); // k
}

// If V is a positive integer constant that is an exact power of two,
// return its log2; otherwise return -1.
static int exactLog2(const Value *V) {
  const auto *CI = dyn_cast<ConstantInt>(V);
  if (!CI || CI->isZero() || CI->isNegative())
    return -1;
  uint64_t N = CI->getZExtValue();
  if (!isPowerOf2_64(N))
    return -1;
  return static_cast<int>(Log2_64(N));
}

PreservedAnalyses StrengthReductionPass::run(Function &F,
                                             FunctionAnalysisManager &) {
  bool Changed = false;
  SmallVector<Instruction *, 16> ToErase;

  for (BasicBlock &BB : F) {
    for (Instruction &I : BB) {
      auto *BinOp = dyn_cast<BinaryOperator>(&I);
      if (!BinOp)
        continue;

      if (!BinOp->getType()->isIntegerTy())
        continue;

      Value *LHS = BinOp->getOperand(0);
      Value *RHS = BinOp->getOperand(1);

      IRBuilder<> Builder(BinOp);
      Value *Replacement = nullptr;

      // ── (A) Mul by (2^k - 1) ────────────────────────────────────────
      // x * (2^k - 1)  =>  (x << k) - x
      if (BinOp->getOpcode() == Instruction::Mul) {
        unsigned k = 0;
        Value *X = nullptr;

        if ((k = isPowerOfTwoMinusOne(RHS))) {
          X = LHS; // x * C
        } else if ((k = isPowerOfTwoMinusOne(LHS))) {
          X = RHS; // C * x (commutative)
        }

        if (k && X) {
          Value *ShiftAmt = ConstantInt::get(BinOp->getType(), k);
          Value *Shl = Builder.CreateShl(X, ShiftAmt, "shl_sr");
          Replacement = Builder.CreateSub(Shl, X, "mul_sr");

          errs() << "[StrengthReduction] Mul(" << (1ULL << k) - 1
                 << "): " << *BinOp << "\n"
                 << "               =>  (x << " << k << ") - x\n";
        }
      }

      // ── (B) UDiv by exact power of two ──────────────────────────────
      // udiv x / 2^k  =>  x >> k (logical shift)
      // (sdiv is not transformed because ashr changes rounding for negatives)
      else if (BinOp->getOpcode() == Instruction::UDiv) {
        int k = exactLog2(RHS);
        if (k >= 0) { // k == 0 for division by 1 is fine (shift by 0)
          Value *ShiftAmt =
              ConstantInt::get(BinOp->getType(), static_cast<uint64_t>(k));
          Replacement = Builder.CreateLShr(LHS, ShiftAmt, "udiv_sr");

          errs() << "[StrengthReduction] UDiv(2^" << k << "): " << *BinOp
                 << "\n"
                 << "               =>  x >> " << k << "\n";
        }
      }

      if (Replacement) {
        BinOp->replaceAllUsesWith(Replacement);
        ToErase.push_back(BinOp);
        Changed = true;
      }
    }
  }

  for (Instruction *I : ToErase)
    I->eraseFromParent();

  return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
}