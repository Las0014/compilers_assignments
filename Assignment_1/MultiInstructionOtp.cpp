/**
 * MultiInstructionOpt.cpp
 *
 * Peephole optimisation across pairs of instructions:
 *
 *   PATTERN:
 *     %a = add nsw %b, 1        
 *     %c = sub nsw %a, 1        
 *   RESULT:
 *     %c replaced by %b   
 * 
 * More precisely, the pass matches the general form:
 *     %t = <op1>  %x, C
 *     %u = <op2>  %t, C        where op1/op2 are inverse pairs
 *       =>  %u  replaced with  %x
 *
 * Supported inverse pairs:
 *   add / sub   (x + C - C  =>  x)
 *   sub / add   (x - C + C  =>  x)
 *
 * The pass performs a single-pass walk; it uses a map from Value* -> {src, const}
 * to remember "this value was produced by src OP C" so that when we see the
 * inverse we can fold immediately.
 *
 */

#include "Passes.h"

#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Support/raw_ostream.h"

#include <unordered_map>

using namespace llvm;

struct DefInfo {
    Value          *Src;     
    const ConstantInt *C;    
    unsigned        Opcode;  
};

static unsigned inverseOpcode(unsigned Op) {
    switch (Op) {
    case Instruction::Add:  return Instruction::Sub;
    case Instruction::Sub:  return Instruction::Add;
    default:                return 0;
    }
}

PreservedAnalyses MultiInstructionOptPass::run(Function &F,
                                                FunctionAnalysisManager &) {
    bool Changed = false;

    for (BasicBlock &BB : F) {
        // Map:  defined_value  ->  DefInfo
        // We rebuild this map for each basic block; cross-block peepholes are
        // intentionally left out to keep the analysis conservative.
        std::unordered_map<Value *, DefInfo> Defs;
        SmallVector<Instruction *, 16> ToErase;

        for (Instruction &I : BB) {
            auto *BinOp = dyn_cast<BinaryOperator>(&I);
            if (!BinOp || !BinOp->getType()->isIntegerTy()) {
                // Non-binary instructions don't participate but don't
                // invalidate the map (SSA: values are immutable).
                continue;
            }

            Value *LHS    = BinOp->getOperand(0);
            Value *RHS    = BinOp->getOperand(1);
            unsigned Op   = BinOp->getOpcode();

            // We look for:   current = LHS  inverse_op  C
            // where LHS was:  LHS    = Src  op          C
            // => current  is equivalent to Src.
            //
            // The constant is always the second operand.

            auto It = Defs.find(LHS);
            if (It != Defs.end()) {
                const DefInfo &D   = It->second;
                unsigned InvOp     = inverseOpcode(D.Opcode);
                const auto *RhsCI  = dyn_cast<ConstantInt>(RHS);

                if (InvOp && InvOp == Op && RhsCI && RhsCI == D.C) {
                    // Pattern matched – replace current instruction with Src.
                    errs() << "[MultiInstructionOpt] Folding: " << *BinOp
                           << "\n                  => " << *D.Src << "\n";
                    BinOp->replaceAllUsesWith(D.Src);
                    ToErase.push_back(BinOp);
                    Changed = true;
                    continue; 
                }
            }

            // ── Record this definition for potential future folding ────────
            // Only record add/sub with constant on the right-hand side.
            if (const auto *CI = dyn_cast<ConstantInt>(RHS)) {
                bool recordable = false;
                switch (Op) {
                case Instruction::Add:
                case Instruction::Sub:
                    recordable = true;
                    break;
                default:
                    break;
                }
                if (recordable)
                    Defs[BinOp] = DefInfo{LHS, CI, Op};
            }
        }

        for (Instruction *I : ToErase)
            I->eraseFromParent();
    }

    return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
}