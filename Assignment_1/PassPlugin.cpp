/**
 * PassPlugin.cpp
 *
 * Registers the three custom passes with LLVM's New Pass Manager so that
 * they can be loaded at runtime via:
 *
 *   opt --load-pass-plugin=./LLVMOptPasses.so \
 *       --passes="algebraic-identity,strength-reduction,multi-inst-opt" \
 *       input.ll -S -o output.ll
 *
 * Individual passes can also be combined with standard pipeline stages:
 *
 *   opt --load-pass-plugin=./LLVMOptPasses.so \
 *       --passes="function(algebraic-identity,strength-reduction)" \
 *       input.ll -S -o output.ll
 */

#include "Passes.h"

#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

// ── Plugin entry point ────────────────────────────────────────────────────────

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION,
        "LLVMOptPasses",
        LLVM_VERSION_STRING,
        [](PassBuilder &PB) {
            // ── Register each pass by name ────────────────────────────────

            // Pass 1 – Algebraic Identity
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                    if (Name == "algebraic-identity") {
                        FPM.addPass(AlgebraicIdentityPass());
                        return true;
                    }
                    return false;
                });

            // Pass 2 – Strength Reduction
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                    if (Name == "strength-reduction") {
                        FPM.addPass(StrengthReductionPass());
                        return true;
                    }
                    return false;
                });

            // Pass 3 – Multi-Instruction Optimisation
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                    if (Name == "multi-inst-opt") {
                        FPM.addPass(MultiInstructionOptPass());
                        return true;
                    }
                    return false;
                });

            // ── Optional: inject into the standard optimisation pipelines ──
            // Uncomment if you want the passes to run automatically when
            // -O1 / -O2 / -O3 is used after loading the plugin.
            //
            // PB.registerVectorizerStartEPCallback(
            //     [](FunctionPassManager &FPM, OptimizationLevel) {
            //         FPM.addPass(AlgebraicIdentityPass());
            //         FPM.addPass(StrengthReductionPass());
            //         FPM.addPass(MultiInstructionOptPass());
            //     });
        }};
}