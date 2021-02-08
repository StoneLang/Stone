#ifndef STONESYNTAXDIAGNOSTICS_H
#define STONESYNTAXDIAGNOSTICS_H

#include <memory>

#include "stone/Utils/DiagnosticEngine.h"

namespace stone {
namespace syn {

class QualTypeCustomDiagnosticArgument : public CustomDiagnosticArgument {};
class DeclNameCustomDiagnosticArgument : public CustomDiagnosticArgument {};
class IdentifierCustomDiagnosticArgument : public CustomDiagnosticArgument {};
class NamedDeclCustomDiagnosticArgument : public CustomDiagnosticArgument {};

class ASTDiagnostics final : public Diagnostics {
public:
};
} // namespace syn
} // namespace stone

#endif
