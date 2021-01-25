#ifndef STONE_DRIVER_COMPILATIONUNIT_H
#define STONE_DRIVER_COMPILATIONUNIT_H

#include <cassert>
#include <iterator>
#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/Option.h"
#include "stone/Core/LLVM.h"
namespace stone {
namespace driver {

class CompilationUnit final {
 public:
  struct Profile final {
    llvm::StringRef fileName;
    file::FileType fileType;
  };
  Profile input;
  Profile output;
};

}  // namespace driver
}  // namespace stone
