#ifndef STONE_ASTDIAGNOSTICS_H
#define STONE_ASTDIAGNOSTICS_H

#include <memory>

#include "stone/Utils/Diagnostics.h"

namespace stone {
namespace syn {

class QualTypeCustomDiagnosticArgument : public CustomDiagnosticArgument {};
class DeclNameCustomDiagnosticArgument : public CustomDiagnosticArgument {};
class IdentifierCustomDiagnosticArgument : public CustomDiagnosticArgument {};
class NamedDeclCustomDiagnosticArgument : public CustomDiagnosticArgument {};

class ASTDiagnostics final : public Diagnostics {
 public:
};
}  // namespace syn
}  // namespace stone

#endif
