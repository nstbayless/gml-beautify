#include "catch.hpp"
#include "parser.h"
#include "test.h"
#include "util.h"
#include <sstream>
#include <fstream>
#include <istream>

// trims end of each line
std::string linertrim(std::string s)
{
  std::stringstream sstr(s);
  std::string out;
  std::string line;
  while (getline(sstr, line))
  {
    out += ext_rtrim(line) + "\n";
  }
  return out;
}

void verify(std::string filebasename, bool negate = false)
{
  BeautifulConfig config;
  config.egyptian = false;
  
  std::ifstream inFile;
  
  std::string file_src = "tests/examples/" + filebasename + "_src.gml";
  std::string file_cmp = "tests/examples/" + filebasename + "_cmp.gml";
  std::cout<<"verifying file " << file_src << "...\n";
 
  std::string in = read_file_contents(file_src);
  
  inFile.open(file_src);
  bool testResult = !perform_tests(inFile, config);
  CHECK(testResult);
  
  Parser parser(in);
  Production* root = parser.parse();
  std::string s = root->beautiful(config).to_string(config);
  delete(root);
    
  CHECK(s.length() > 0);
  bool stringCheck = (linertrim(ext_rtrim(read_file_contents(file_cmp))) == linertrim(ext_rtrim(s))) ^ negate;
  CHECK(stringCheck);
}

TEST_CASE("Beautifier correctly beautifies some standard files", "[beautifier]")
{
  verify("test1");
  verify("block_comments");
  verify("braces");
  verify("case-comment");
  verify("chained_else_if");
  verify("comment_blanks");
  verify("demical");
  verify("equals");
  verify("fn");
  verify("hello_world");
  verify("if-else");
  verify("line_split");
  verify("long_line");
  verify("loops");
  verify("loops2");
  verify("spacing");
  verify("stringTest");
  verify("switch");
  verify("unfinished_comment");
  verify("varspacing");
  verify("spacingA");
  verify("spacingB");
  verify("spacingC");
  verify("negate", true);
}