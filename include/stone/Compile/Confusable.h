#ifndef STONE_COMPILE_CONFUSABLES_H
#define STONE_COMPILE_CONFUSABLES_H

#include <stdint.h>

namespace stone {
namespace confusable {
/// Given a UTF-8 codepoint, determines whether it appears on the Unicode
/// specification table of confusable characters and maps to punctuation,
/// and either returns either the expected ASCII character or 0.
char ConvertConfusableCharacterToASCII(uint32_t codepoint);
}  // namespace confusable
}  // namespace stone

#endif
