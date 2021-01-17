#include "stone/Driver/Activity.h"

using namespace stone;
using namespace stone::driver;

const char *Activity::GetName(Activity::Kind k) {
  switch (k) {
    case Activity::Kind::Input:
      return "input";
    case Activity::Kind::Compile:
      return "compile";
    case Activity::Kind::Backend:
      return "backend";
    case Activity::Kind::Assemble:
      return "assemble";
    case Activity::Kind::DynamicLink:
      return "dynamic-link";
    case Activity::Kind::StaticLink:
      return "static-link";
  }
  llvm_unreachable("invalid class");
}
