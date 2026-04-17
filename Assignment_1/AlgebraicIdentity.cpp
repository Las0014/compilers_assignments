/**
 * AlgebraicIdentity.cpp
 *
 * Eliminates neutral-element operations:
 *   x + 0  =>  x        0 + x  =>  x
 *   x * 1  =>  x        1 * x  =>  x
 */

#include "Passes.h"

#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

// Return true if V is the integer constant 0.
static bool isZero(const Value *V)
{
    const auto *CI = dyn_cast<ConstantInt>(V);
    return CI && CI->isZero();
}

// Return true if V is the integer constant 1.
static bool isOne(const Value *V)
{
    const auto *CI = dyn_cast<ConstantInt>(V);
    return CI && CI->isOne();
}

PreservedAnalyses AlgebraicIdentityPass::run(Function &F,
                                             FunctionAnalysisManager &)
{
    bool Changed = false;

    // Collect instructions to erase after the iteration to avoid
    // invalidating the iterator while we walk the BB.
    SmallVector<Instruction *, 16> ToErase;

    for (BasicBlock &BB : F)
    {
        for (Instruction &I : BB)
        {
            auto *BinOp = dyn_cast<BinaryOperator>(&I);
            if (!BinOp)
                continue;

            // Only operate on integer arithmetic
            if (!BinOp->getType()->isIntegerTy())
                continue;

            Value *LHS = BinOp->getOperand(0);
            Value *RHS = BinOp->getOperand(1);
            Value *Replacement = nullptr;

            switch (BinOp->getOpcode())
            {
            //  x + 0  =>  x
            case Instruction::Add:
                if (isZero(RHS))
                    Replacement = LHS;
                //  0 + x  =>  x
                else if (isZero(LHS))
                    Replacement = RHS;
                break;

            //  x * 1  =>  x
            case Instruction::Mul:
                if (isOne(RHS))
                    Replacement = LHS;
                //  1 * x  =>  x
                else if (isOne(LHS))
                    Replacement = RHS;
                break;

            default:
                break;
            }

            if (Replacement)
            {
                errs() << "[AlgebraicIdentity] Replacing: " << *BinOp
                       << "\n               with: " << *Replacement << "\n";
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