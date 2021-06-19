#ifndef STONE_ANALYZE_PARSERRAII_H
#define STONE_ANALYZE_PARSERRAII_H

#include "stone/Analyze/DeclSpecifier.h"
#include "stone/Analyze/Parser.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {
namespace syn {

/// A class for parsing a declarator.
class ParsingDeclarator final : public Declarator {

  // ParsingDeclRAIIObject ParsingRAII;

  // public:
  //   ParsingDeclarator(Parser &P, const ParsingDeclSpecifier &ds,
  //   DeclaratorContext dc)
  //       : Declarator(DS, C), ParsingRAII(P, &DS.getDelayedDiagnosticPool())
  //       {}

  //   const ParsingDeclSpec &GetDeclSpecifier() const {
  //     return static_cast<const ParsingDeclSpec &>(Declarator::getDeclSpec());
  //   }

  //   ParsingDeclSpec &getMutableDeclSpecifier() const {
  //     return const_cast<ParsingDeclSpec &>(getDeclSpec());
  //   }

  //   void Clear() {
  //     //Declarator::Clear();
  //     //ParsingRAII.Reset();
  //   }

  // void Complete(Decl *D) { ParsingRAII.complete(D); }
};
/// RAII object that makes sure paren/bracket/brace count is correct
/// after declaration/statement parsing, even when there's a parsing error.
class ParsingBalancer final {
  Parser &parser;
  unsigned short parenCount, bracketCount, braceCount;

public:
  ParsingBalancer(Parser &other)
      : parser(other), parenCount(other.parenCount),
        bracketCount(other.bracketCount), braceCount(other.braceCount) {}

  ~ParsingBalancer() {
    // parser.AngleBrackets.clear(parser);
    parser.parenCount = parenCount;
    parser.bracketCount = bracketCount;
    parser.braceCount = braceCount;
  }
};
} // namespace syn
} // namespace stone
#endif
