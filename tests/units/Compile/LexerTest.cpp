#include "stone/Analysis/Lexer.h"
#include "stone/Core/LangOptions.h"
#include "stone/Core/Mem.h"
#include "stone/Core/SrcMgr.h"

#include "gtest/gtest.h"

using namespace stone;

class LexerTest : public ::testing::Test {
protected:
  DiagnosticEngine de;
  DiagnosticOptions diagOpts;
  FileSystemOptions fmOpts;
  LangOptions langOpts;
  FileMgr fm;
  SrcMgr sm;

protected:
  LexerTest() : de(diagOpts, nullptr, false), fm(fmOpts), sm(de, fm) {}

protected:
  std::unique_ptr<Lexer> CreateLexer(llvm::StringRef srcBuffer) {

    auto memBuffer = llvm::MemoryBuffer::getMemBuffer(srcBuffer);
    auto mainSrcID = sm.CreateSrcID(std::move(memBuffer));

    sm.SetMainSrcID(mainSrcID);
    auto lexer = llvm::make_unique<Lexer>(mainSrcID, sm, langOpts /*de*/);
    return lexer;
  }
  std::vector<Token> Lex(llvm::StringRef srcBuffer) {

    auto lexer = CreateLexer(srcBuffer);
    std::vector<Token> tokens;
    while (true) {
      Token token;
      lexer->Lex(token);
      tokens.push_back(token);
      break;
      // if(token.GetKind() == tk::eof) {
      //	break;
      //}
    }
    return tokens;
  }
};

TEST_F(LexerTest, GetNextToken) {

  llvm::StringRef srcBuffer = "fun\n";
  auto tokens = Lex(srcBuffer);

  ASSERT_EQ(tk::kw_fun, tokens[0].GetKind());
}
