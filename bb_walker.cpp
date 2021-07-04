#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/IR/CFG.h>
#include <llvm/ADT/BreadthFirstIterator.h>
#include <llvm/ADT/DepthFirstIterator.h>
#include <llvm/ADT/SCCIterator.h>
#include <llvm/ADT/PostOrderIterator.h>
#include <iostream>
#include <vector>
#include <array>
#include <utility>

using namespace llvm;


using MeasurementMap = std::map<std::pair<BasicBlock *, BasicBlock *>, std::vector<BasicBlock *>>;

// For each BasicBlock find checkpoint and collect LoA that direct
// control flow from previous Checkpoint to the next one.
void handle(BasicBlock *basicBlock,
            MeasurementMap &measurements,
            std::set<BasicBlock *> &checkpoints,
            std::vector<BasicBlock *> &LoA,
            std::array<BasicBlock *, 2> &checkpointPair) {

    if (checkpointPair[0] == nullptr) {
        if (checkpoints.contains(basicBlock)) {
            checkpointPair[0] = basicBlock;
            LoA = std::vector<BasicBlock *>();
            LoA.push_back(basicBlock);
        }
    } else if (checkpointPair[1] == nullptr) {
        if (checkpoints.contains(basicBlock)) {
            checkpointPair[1] = basicBlock;
            if (LoA.size() < 2) {
                LoA.push_back(basicBlock);
            }
            std::pair<BasicBlock *, BasicBlock *> key = {checkpointPair[0],
                                                         checkpointPair[1]};
            measurements[key] = LoA;
            // Current checkpoint will be the next checkpoint
            checkpointPair = {nullptr, nullptr};
            if (checkpoints.contains(basicBlock)) {
                checkpointPair[0] = basicBlock;
                LoA = std::vector<BasicBlock *>();
                LoA.push_back(basicBlock);
            }
        } else {
            if (LoA.size() < 2) {
                LoA.push_back(basicBlock);
            }
        }
    } else {
        checkpointPair = {nullptr, nullptr};
        if (checkpoints.contains(basicBlock)) {
            checkpointPair[0] = basicBlock;
            LoA = std::vector<BasicBlock *>();
            LoA.push_back(basicBlock);
        }
    }
    if (basicBlock->getTerminator()->getNumSuccessors() > 1) {
        for (auto succ : successors(basicBlock)) {
            std::array<BasicBlock*, 2> bbPairNested = {checkpointPair[0], nullptr};
            std::vector<BasicBlock*> nestedLoas = LoA;
            handle(succ, measurements, checkpoints, nestedLoas, bbPairNested);
        }
    }
}


void collectListOfActions(const std::unique_ptr<Module> &module) {
    // Put in vector for easy reference
    std::vector<BasicBlock *> basicBlocks;
    for (auto &function : *module) {
        if (function.getName() == "main") {
            for (auto it : depth_first(&function.getEntryBlock())) {
                basicBlocks.push_back(it);
            }
        }
    }
    // only for neted-loop.ll
    std::vector<bool> cp = {true, true, false, true, false, true, true, false, false, false, false, false,
                            true};

    // Tell whether BB is a checkpoint
    std::set<BasicBlock *> checkpoints;
    for (auto i = 0; i < basicBlocks.size(); i++) {
        if (cp[i]) {
            checkpoints.insert(basicBlocks[i]);
        }
    }

    std::array<BasicBlock *, 2> checkpointPair = {nullptr, nullptr};
    std::vector<BasicBlock *> LoA;
    MeasurementMap measurements;
    for (auto bb : depth_first(basicBlocks[0])) {
        handle(bb, measurements, checkpoints, LoA, checkpointPair);
    }


    // Printing the result
    std::cout << "The size of the measurement: " << measurements.size() << std::endl;
    for (auto iter: measurements) {
        auto key = iter.first;
        auto value = iter.second;
        outs() << "Checkpoint_A: " << *key.first << "\n";
        outs() << "Checkpoint_B: " << *key.second << "\n";

        int idx = 0;
        for (auto loa : value) {
            outs() << "LOA_" << idx << ": " << *loa << "\n";
            idx++;
        }
        outs() << "=====================================\n";
    }
}

int getNumPredecassor(BasicBlock &BB) {
    int num = 0;
    for (auto it = pred_begin(&BB), e = pred_end(&BB); it != e; ++it) {
        num++;
    }
    return num;
}

void iterator(const std::unique_ptr<Module> &module) {
    std::vector<BasicBlock *> basicBlocks;
    for (auto &function : *module) {
        if (function.getName() == "main") {
            for (auto it : depth_first(&function.getEntryBlock())) {
                basicBlocks.push_back(it);
            }
        }
    }

    DenseMap<BasicBlock*, int> visited;
    int all = 0;
    for (auto it : depth_first(basicBlocks[0])) {
        BasicBlock &b = *it;
        visited[&b] += b.getTerminator()->getNumSuccessors();
        if (getNumPredecassor(b) > 1) {
            visited[&b] += getNumPredecassor(b);
        }
        all += visited[&b];
    }
    df_iterator_default_set<BasicBlock*> st;
//    all = 2;
    while (all != 0) {
        BasicBlock *previous = nullptr;
        for (auto it = df_ext_begin(basicBlocks[0], st), e = df_ext_end(basicBlocks[0], st); it != e; ++it) {
            if (it->getTerminator()->getNumSuccessors() == 0) {
                break;
            }
            if (previous != nullptr ) {
                if (--visited[previous] != 0) {
                    st.erase(previous);
                }
            }
            all--;
            outs() << "visiting: " << **it << "\n";
            previous = *it;
        }

    }
}

int main() {
    LLVMContext ctx;
    SMDiagnostic Err;
    auto module = parseIRFile("/home/lamida/github/lamida/hello-llvm/hello-dot/branches2.ll", Err, ctx);
    iterator(module);
}