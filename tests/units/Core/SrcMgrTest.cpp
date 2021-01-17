#include "stone/Core/SrcMgr.h"
#include "stone/Core/DiagnosticOptions.h"
#include "stone/Core/Diagnostics.h"
#include "stone/Core/FileMgr.h"
#include "stone/Core/FileSystemOptions.h"

#include "llvm/ADT/SmallString.h"
#include "llvm/Config/llvm-config.h"
#include "llvm/Support/Process.h"
#include "gtest/gtest.h"

#include <cstddef>

using namespace stone;

class SrcMgrTest : public ::testing::Test {
protected:
  DiagnosticOptions diagOpts;
  FileSystemOptions fmOpts;
  FileMgr fm;
  DiagnosticEngine de;
  SrcMgr sm;

protected:
  SrcMgrTest() : de(diagOpts, nullptr, false), fm(fmOpts), sm(de, fm) {}
};

TEST_F(SrcMgrTest, GetColNumber) {

  const char *Source = "int x;\n"
                       "int y;";

  auto memBuffer = llvm::MemoryBuffer::getMemBuffer(Source);

  auto MainSrcID = sm.CreateSrcID(std::move(memBuffer));
  sm.SetMainSrcID(MainSrcID);

  bool Invalid;

  Invalid = false;
  EXPECT_EQ(1U, sm.GetColNumber(MainSrcID, 0, &Invalid));
  EXPECT_TRUE(!Invalid);

  Invalid = false;
  EXPECT_EQ(5U, sm.GetColNumber(MainSrcID, 4, &Invalid));
  EXPECT_TRUE(!Invalid);

  Invalid = false;
  EXPECT_EQ(1U, sm.GetColNumber(MainSrcID, 7, &Invalid));
  EXPECT_TRUE(!Invalid);

  Invalid = false;
  EXPECT_EQ(5U, sm.GetColNumber(MainSrcID, 11, &Invalid));
  EXPECT_TRUE(!Invalid);

  Invalid = false;
  EXPECT_EQ(7U, sm.GetColNumber(MainSrcID, strlen(Source), &Invalid));
  EXPECT_TRUE(!Invalid);

  Invalid = false;
  sm.GetColNumber(MainSrcID, strlen(Source) + 1, &Invalid);
  EXPECT_TRUE(Invalid);

  // Test invalid files
  Invalid = false;
  sm.GetColNumber(SrcID(), 0, &Invalid);
  EXPECT_TRUE(Invalid);

  Invalid = false;
  sm.GetColNumber(SrcID(), 1, &Invalid);
  EXPECT_TRUE(Invalid);

  // Test with no invalid flag.
  EXPECT_EQ(1U, sm.GetColNumber(MainSrcID, 0, nullptr));
}
