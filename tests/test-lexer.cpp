# include "catch.hpp"
# include "tokenstream.h"
# include <sstream>

TEST_CASE("Lexer reading empty string", "[lexer]")
{
  TokenStream ts("");
  CHECK(ts.eof());
  CHECK(ts.peek().type == END);
  CHECK(ts.read().type == END);
}

TEST_CASE("Lexer can parse a variety of standard tokens", "[lexer]")
{
  std::stringstream ss;
  TokenStream* tsp = nullptr;
  
  for (int cat = 1; cat >= 0; cat--)
  {
    if (!cat)
    {
      tsp = new TokenStream(&ss);
    }
    
    TokenStream& ts = *tsp;
     
    //  token
    if (cat)
    {
      ss << "test;";
    }
    else
    {
      REQUIRE(ts.read() == Token(ID, "test"));
      REQUIRE(ts.read() == Token(ENX, ";"));
    }
    
    // arithmetic
    if (cat)
    {
      ss << "{x.z = 4.4\ny--/*com\nment*/}";
    }
    else
    {
      REQUIRE(ts.read() == Token(PUNC, "{"));
      REQUIRE(ts.read() == Token(ID, "x"));
      REQUIRE(ts.read() == Token(PUNC, "."));
      REQUIRE(ts.read() == Token(ID, "z"));
      REQUIRE(ts.read() == Token(OP, "="));
      REQUIRE(ts.read() == Token(NUM, "4.4"));
      REQUIRE(ts.read() == Token(ENX, "\n"));
      REQUIRE(ts.read() == Token(ID, "y"));
      REQUIRE(ts.read() == Token(OPR, "--"));
      REQUIRE(ts.read() == Token(COMMENT, "/*com\nment*/"));
      REQUIRE(ts.read() == Token(PUNC, "}"));
    }
    
    // numbers
    if (cat)
    {
      ss << "4 4.3 0.2 026.242 $fe34Fd;";
    }
    else
    {
      REQUIRE(ts.read() == Token(NUM, "4"));
      REQUIRE(ts.read() == Token(NUM, "4.3"));
      REQUIRE(ts.read() == Token(NUM, "0.2"));
      REQUIRE(ts.read() == Token(NUM, "026.242"));
      REQUIRE(ts.read() == Token(NUM, "$fe34Fd"));
      REQUIRE(ts.read() == Token(ENX, ";"));
    }
  }
}