#include <llvm/ADT/ArrayRef.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/ADT/SmallSet.h>
#include <llvm/ADT/DenseMap.h>
#include <iostream>
#include <ostream>

using namespace llvm;

struct BBasicBlock {
    std::string name;
};

std::ostream& operator<<(std::ostream &o, BBasicBlock &bb) {
    o << "{'name':" << bb.name << "}";
    return o;
}

void arrayRef() {
    ArrayRef<int> arr = {1,3,5};
    for (auto e : arr) {
        std::cout << e << std::endl;
    }
    ArrayRef<BBasicBlock> arr2 = {BBasicBlock{"test"}};
    for (auto e : arr2) {
        std::cout << e << std::endl;
    }
}

void smallVector() {
    SmallVector<int, 1> v;
    for (auto i = 0; i < 10; i++) {
        v.push_back(i);
    }
    for (auto e : v) {
        std::cout << e << std::endl;
    }
}

void smallSet() {
    SmallSet<int, 3> s;
    s.insert(1);
    s.insert(1);
    s.insert(2);
    std::cout << s.size() << std::endl;
}

void denseMap() {
    // Need specialization for the key
//    DenseMap<std::string, int> m;

}

int main() {
    smallSet();
}
