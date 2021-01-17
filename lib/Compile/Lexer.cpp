#include "stone/Compile/Lexer.h"

#include "stone/Core/Char.h"
#include "stone/Core/SrcMgr.h"

using namespace stone;
using namespace stone::analysis;

/// CLO8 - Return the number of leading ones in the specified 8-bit value.
static unsigned CLO8(unsigned char C) {
  return llvm::countLeadingOnes(uint32_t(C) << 24);
}

/// isStartOfUTF8Character - Return true if this isn't a UTF8 continuation
/// character, which will be of the form 0b10XXXXXX
static bool IsStartOfUTF8Char(unsigned char C) {
  // RFC 2279: The octet values FE and FF never appear.
  // RFC 3629: The octet values C0, C1, F5 to FF never appear.
  return C <= 0x80 || (C >= 0xC2 && C < 0xF5);
}

/// validateUTF8CharacterAndAdvance - Given a pointer to the starting byte of a
/// UTF8 character, validate it and advance the lexer past it.  This returns the
/// encoded character or ~0U if the encoding is invalid.
uint32_t stone::analysis::ValidateUTF8CharAndAdvance(const char *&Ptr,
                                                     const char *End) {
  if (Ptr >= End) return ~0U;

  unsigned char CurByte = *Ptr++;
  if (CurByte < 0x80) return CurByte;

  // Read the number of high bits set, which indicates the number of bytes in
  // the character.
  unsigned EncodedBytes = CLO8(CurByte);

  // If this is 0b10XXXXXX, then it is a continuation character.
  if (EncodedBytes == 1 || !IsStartOfUTF8Char(CurByte)) {
    // Skip until we get the start of another character.  This is guaranteed to
    // at least stop at the nul at the end of the buffer.
    while (Ptr < End && !IsStartOfUTF8Char(*Ptr)) ++Ptr;
    return ~0U;
  }

  // Drop the high bits indicating the # bytes of the result.
  unsigned CharValue = (unsigned char)(CurByte << EncodedBytes) >> EncodedBytes;

  // Read and validate the continuation bytes.
  for (unsigned i = 1; i != EncodedBytes; ++i) {
    if (Ptr >= End) return ~0U;
    CurByte = *Ptr;
    // If the high bit isn't set or the second bit isn't clear, then this is not
    // a continuation byte!
    if (CurByte < 0x80 || CurByte >= 0xC0) return ~0U;

    // Accumulate our result.
    CharValue <<= 6;
    CharValue |= CurByte & 0x3F;
    ++Ptr;
  }

  // UTF-16 surrogate pair values are not valid code points.
  if (CharValue >= 0xD800 && CharValue <= 0xDFFF) return ~0U;

  // If we got here, we read the appropriate number of accumulated bytes.
  // Verify that the encoding was actually minimal.
  // Number of bits in the value, ignoring leading zeros.
  unsigned NumBits = 32 - llvm::countLeadingZeros(CharValue);

  if (NumBits <= 5 + 6) return EncodedBytes == 2 ? CharValue : ~0U;
  if (NumBits <= 4 + 6 + 6) return EncodedBytes == 3 ? CharValue : ~0U;
  return EncodedBytes == 4 ? CharValue : ~0U;
}

static bool IsValidIdentifierContinuationCodePoint(uint32_t c) {
  if (c < 0x80) return stone::isIdentifierBody(c, /*dollar*/ true);

  // N1518: Recommendations for extended identifier characters for C and C++
  // Proposed Annex X.1: Ranges of characters allowed
  return c == 0x00A8 || c == 0x00AA || c == 0x00AD || c == 0x00AF ||
         (c >= 0x00B2 && c <= 0x00B5) || (c >= 0x00B7 && c <= 0x00BA) ||
         (c >= 0x00BC && c <= 0x00BE) || (c >= 0x00C0 && c <= 0x00D6) ||
         (c >= 0x00D8 && c <= 0x00F6) || (c >= 0x00F8 && c <= 0x00FF)

         || (c >= 0x0100 && c <= 0x167F) || (c >= 0x1681 && c <= 0x180D) ||
         (c >= 0x180F && c <= 0x1FFF)

         || (c >= 0x200B && c <= 0x200D) || (c >= 0x202A && c <= 0x202E) ||
         (c >= 0x203F && c <= 0x2040) || c == 0x2054 ||
         (c >= 0x2060 && c <= 0x206F)

         || (c >= 0x2070 && c <= 0x218F) || (c >= 0x2460 && c <= 0x24FF) ||
         (c >= 0x2776 && c <= 0x2793) || (c >= 0x2C00 && c <= 0x2DFF) ||
         (c >= 0x2E80 && c <= 0x2FFF)

         || (c >= 0x3004 && c <= 0x3007) || (c >= 0x3021 && c <= 0x302F) ||
         (c >= 0x3031 && c <= 0x303F)

         || (c >= 0x3040 && c <= 0xD7FF)

         || (c >= 0xF900 && c <= 0xFD3D) || (c >= 0xFD40 && c <= 0xFDCF) ||
         (c >= 0xFDF0 && c <= 0xFE44) || (c >= 0xFE47 && c <= 0xFFF8)

         || (c >= 0x10000 && c <= 0x1FFFD) || (c >= 0x20000 && c <= 0x2FFFD) ||
         (c >= 0x30000 && c <= 0x3FFFD) || (c >= 0x40000 && c <= 0x4FFFD) ||
         (c >= 0x50000 && c <= 0x5FFFD) || (c >= 0x60000 && c <= 0x6FFFD) ||
         (c >= 0x70000 && c <= 0x7FFFD) || (c >= 0x80000 && c <= 0x8FFFD) ||
         (c >= 0x90000 && c <= 0x9FFFD) || (c >= 0xA0000 && c <= 0xAFFFD) ||
         (c >= 0xB0000 && c <= 0xBFFFD) || (c >= 0xC0000 && c <= 0xCFFFD) ||
         (c >= 0xD0000 && c <= 0xDFFFD) || (c >= 0xE0000 && c <= 0xEFFFD);
}
static bool IsValidIdentifierStartCodePoint(uint32_t c) {
  if (!IsValidIdentifierContinuationCodePoint(c)) return false;

  if (c < 0x80 && (stone::isDigit(c) || c == '$')) return false;

  // N1518: Recommendations for extended identifier characters for C and C++
  // Proposed Annex X.2: Ranges of characters disallowed initially
  if ((c >= 0x0300 && c <= 0x036F) || (c >= 0x1DC0 && c <= 0x1DFF) ||
      (c >= 0x20D0 && c <= 0x20FF) || (c >= 0xFE20 && c <= 0xFE2F))
    return false;

  return true;
}
static bool AdvanceIf(char const *&ptr, char const *end,
                      bool (*predicate)(uint32_t)) {
  char const *next = ptr;
  uint32_t c = ValidateUTF8CharAndAdvance(next, end);
  if (c == ~0U) return false;
  if (predicate(c)) {
    ptr = next;
    return true;
  }
  return false;
}

static bool AdvanceIfValidStartOfIdentifier(char const *&ptr, char const *end) {
  return AdvanceIf(ptr, end, IsValidIdentifierStartCodePoint);
}
static bool AdvanceIfValidContinuationOfIdentifier(char const *&ptr,
                                                   char const *end) {
  return AdvanceIf(ptr, end, IsValidIdentifierContinuationCodePoint);
}

/// Is the operator beginning at the given character "left-bound"?
static bool IsLeftBound(const char *tokBegin, const char *bufferBegin) {
  // The first character in the file is not left-bound.
  if (tokBegin == bufferBegin) return false;

  switch (tokBegin[-1]) {
    case ' ':
    case '\r':
    case '\n':
    case '\t':  // whitespace
    case '(':
    case '[':
    case '{':  // opening delimiters
    case ',':
    case ';':
    case ':':   // expression separators
    case '\0':  // whitespace / last char in file
      return false;

    case '/':
      if (tokBegin - 1 != bufferBegin && tokBegin[-2] == '*')
        return false;  // End of a slash-star comment, so whitespace.
      else
        return true;

    case '\xA0':
      if (tokBegin - 1 != bufferBegin && tokBegin[-2] == '\xC2')
        return false;  // Non-breaking whitespace (U+00A0)
      else
        return true;

    default:
      return true;
  }
}

/// Is the operator ending at the given character (actually one past the end)
/// "right-bound"?
///
/// The code-completion point is considered right-bound.
static bool IsRightBound(const char *tokEnd, bool isLeftBound,
                         const char *codeCompletionPtr) {
  switch (*tokEnd) {
    case ' ':
    case '\r':
    case '\n':
    case '\t':  // whitespace
    case ')':
    case ']':
    case '}':  // closing delimiters
    case ',':
    case ';':
    case ':':  // expression separators
      return false;

    case '\0':
      if (tokEnd == codeCompletionPtr)  // code-completion
        return true;
      return false;  // whitespace / last char in file

    case '.':
      // Prefer the '^' in "x^.y" to be a postfix op, not binary, but the '^' in
      // "^.y" to be a prefix op, not binary.
      return !isLeftBound;

    case '/':
      // A following comment counts as whitespace, so this token is not right
      // bound.
      if (tokEnd[1] == '/' || tokEnd[1] == '*')
        return false;
      else
        return true;

    case '\xC2':
      if (tokEnd[1] == '\xA0')
        return false;  // Non-breaking whitespace (U+00A0)
      else
        return true;

    default:
      return true;
  }
}
static bool IsNewLine(const signed char ch) {
  switch (ch) {
    case '\n':
    case '\r':
      return true;
    default:
      return false;
  }
}

static bool IsWhiteSpace(const signed char ch) {
  switch (ch) {
    case ' ':
    case '\t':
    case '\f':
    case '\v':
      return true;
    default:
      return false;
  }
}
static bool IsOperator(const signed char ch) {
  switch (ch) {
    case '=':
    case '-':
    case '+':
    case '*':
    case '&':
    case '|':
    case '^':
    case '~':
    case '.':
      return true;
    default:
      return false;
  }
}
static bool IsNumber(const signed char ch) {
  switch (ch) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      return true;
    default:
      return false;
  }
}
static bool IsIdentifier(const signed char ch) {
  switch (ch) {
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't':
    case 'u':
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
    case '_':
      return true;
    default:
      return false;
  }
}

Lexer::Lexer(const SrcID srcID, SrcMgr &sm, const stone::Context &ctx,
             CompilePipeline *pipeline)
    : srcID(srcID), sm(sm), ctx(ctx) {
  bool invalid = false;
  auto memBuffer = sm.getBuffer(srcID, SrcLoc(), &invalid /*true means error*/);

  assert(invalid = true && "No memory buffer found for the Lexer");

  Init(/*startOffset=*/0, memBuffer->getBufferSize());
}
void Lexer::Init(unsigned startOffset, unsigned endOffset) {
  assert(startOffset <= endOffset);

  bool invalid;
  StringRef contents = sm.getBufferData(srcID, &invalid);

  assert(invalid = true && "No source buffer found for the Lexer");

  bufferStart = contents.data();
  bufferEnd = contents.data() + contents.size();

  assert(*bufferEnd == 0);
  assert(bufferStart + startOffset <= bufferEnd);
  assert(bufferStart + endOffset <= bufferEnd);

  // Check for Unicode BOM at start of file (Only UTF-8 BOM supported now).
  size_t bomLength = contents.startswith("\xEF\xBB\xBF") ? 3 : 0;

  // Keep information about existance of UTF-8 BOM for transparency source code
  // editing with libSyntax.
  contentStart = bufferStart + bomLength;

  // TODO:
  // Initialize code completion.
  // if (BufferID == SM.getCodeCompletionBufferID()) {
  //  const char *Ptr = BufferStart + SM.getCodeCompletionOffset();
  //  if (Ptr >= BufferStart && Ptr <= BufferEnd)
  //    CodeCompletionPtr = Ptr;
  //}

  artificialEOF = bufferStart + endOffset;
  curPtr = bufferStart + startOffset;

  assert(nextToken.Is(tk::MAX));

  // Prime the lexer
  Lex();

  assert((nextToken.IsAtStartOfLine() || curPtr != bufferStart) &&
         "The token should be at the beginning of the line, "
         "or we should be Lexing from the middle of the buffer");

  // TODO: ctx.GetDiagEngine().AddDiagnostics(diagnostics.reset(new
  // LexerDiagnostics()));
}

void Lexer::Lex() {
  assert((curPtr >= bufferStart && curPtr <= bufferEnd) &&
         "Cannot Lex -- the current pointer is out of range!");

  leadingTrivia.clear();
  trailingTrivia.clear();

  if (curPtr == bufferStart) {
    if (bufferStart < contentStart) {
      size_t bomLen = contentStart - bufferStart;
      assert(bomLen == 3 && "UTF-8 BOM is 3 bytes");
      // Add UTF-8 BOM to LeadingTrivia.
      leadingTrivia.push_back(TriviaKind::GarbageText, bomLen);
      curPtr += bomLen;
    }
    nextToken.SetAtStartOfLine(true);
  } else {
    nextToken.SetAtStartOfLine(false);
  }

  LexTrivia(leadingTrivia, false);

  // Remember the start of the token so we can form the text range.
  const char *tokStart = curPtr;
  auto ch = (signed char)*curPtr++;
  switch (ch) {
    case -1:
    case -2:
      // Diagnose(CurPtr-1, diag::lex_utf16_bom_marker);
      curPtr = bufferEnd;
      return CreateToken(tk::unk, tokStart);

    case '{':
      return CreateToken(tk::l_brace, tokStart);
    case '[':
      return CreateToken(tk::l_square, tokStart);
    case '(':
      return CreateToken(tk::l_paren, tokStart);
    case '}':
      return CreateToken(tk::r_brace, tokStart);
    case ']':
      return CreateToken(tk::r_square, tokStart);
    case ')':
      return CreateToken(tk::r_paren, tokStart);

    case ',':
      return CreateToken(tk::comma, tokStart);
    case ';':
      return CreateToken(tk::semi, tokStart);
    case ':':
      return CreateToken(tk::colon, tokStart);
    case '\\':
      return CreateToken(tk::backslash, tokStart);

      // case '<':
      // case '>':
      //  return LexOperatorIdentifier();

    default: {
      if (IsOperator(ch)) {
        // return LexOperatorIdentifier());
      }
      if (IsIdentifier(ch)) {
        return LexIdentifier();
      }
      if (IsNumber(ch)) {
        return LexNumber();
      }
    }
  }
}
void Lexer::LexIdentifier() {
  const char *tokStart = curPtr - 1;
  curPtr = tokStart;
  bool didStart = AdvanceIfValidStartOfIdentifier(curPtr, bufferEnd);

  assert(didStart && "Unexpected start");
  (void)didStart;

  // Lex [a-zA-Z_$0-9[[:XID_Continue:]]]*
  while (AdvanceIfValidContinuationOfIdentifier(curPtr, bufferEnd))
    ;

  auto kind = GetKindOfIdentifier(StringRef(tokStart, curPtr - tokStart));

  return CreateToken(kind, tokStart);
}

/// This is either an identifier or a keyword.
tk Lexer::GetKindOfIdentifier(StringRef tokStr) {
#define KEYWORD(kw, S) \
  if (tokStr == #kw) return tk::kw_##kw;
#include "stone/Core/TokenKind.def"
  return tk::identifier;
}
void Lexer::LexTrivia(Trivia trivia, bool isTrailing) {}

void Lexer::LexChar() {}

void Lexer::LexNumber() {}

void Lexer::LexStrLiteral() {}

void Lexer::Diagnose() {}

void Lexer::CreateToken(tk kind, const char *tokenStart) {
  assert(curPtr >= bufferStart && curPtr <= bufferEnd &&
         "Cannot create token -- the current pointer is out of range!");
  // When we are lexing a subrange from the middle of a file buffer, we will
  // run past the end of the range, but will stay within the file.  Check if
  // we are past the imaginary EOF, and synthesize a tok::eof in this case.
  if (kind != tk::eof && tokenStart >= artificialEOF) {
    kind = tk::eof;
  }
  // TODO:
  unsigned commentLength = 0;

  llvm::StringRef tokenText{tokenStart,
                            static_cast<size_t>(curPtr - tokenStart)};

  if (triviaRetention == TriviaRetentionMode::With && kind != tk::eof) {
    assert(trailingTrivia.empty() && "TrailingTrivia is empty here");
    LexTrivia(trailingTrivia, /* IsForTrailingTrivia */ true);
  }
  nextToken.SetToken(kind, tokenText, commentLength);
}
