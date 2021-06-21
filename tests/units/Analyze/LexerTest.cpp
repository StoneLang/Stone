#include "stone/Analyze/Lexer.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/Mem.h"
#include "stone/Basic/SrcMgr.h"

#include "gtest/gtest.h"

using namespace stone;
using namespace stone::syn; 


class LexerTest : public ::testing::Test {
protected:
  DiagnosticEngine de;
  DiagnosticOptions diagOpts;
  FileSystemOptions fmOpts;
  LangOptions langOpts;
  FileMgr fm;
  SrcMgr sm;
	Context ctx; 

protected:
  LexerTest() : de(diagOpts, &sm), fm(fmOpts), sm(de, fm) {}

protected:
  std::unique_ptr<Lexer> CreateLexer(llvm::StringRef srcBuffer) {

    auto memBuffer = llvm::MemoryBuffer::getMemBuffer(srcBuffer);
    auto mainSrcID = sm.CreateSrcID(std::move(memBuffer));

    sm.SetMainSrcID(mainSrcID);
    auto lexer = llvm::make_unique<Lexer>(mainSrcID, sm, basic);
    return lexer;
  }
  std::vector<syn::Token> Lex(llvm::StringRef srcBuffer) {

    auto lexer = CreateLexer(srcBuffer);
    std::vector<syn::Token> tokens;
    while (true) {
			syn::Token token;
      lexer->Lex(token);
      tokens.push_back(token);
      break;
      // if(token.GetKind() == tk::Type::eof) {
      //	break;
      //}
    }
    return tokens;
  }
};

TEST_F(LexerTest, GetNextToken) {

  llvm::StringRef srcBuffer = "fun\n";
  auto tokens = Lex(srcBuffer);

  ASSERT_EQ(tk::Type::kw_fun, tokens[0].GetType());
}
