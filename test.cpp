#include <fstream>
#include <cstdlib>
#include <cstring>
#include <sstream>

#include "test.h"
#include "tokenstream.h"
#include "parser.h"
#include "util.h"

using namespace std;

void check_comments_identical(TokenStream&, TokenStream&);
void check_logic_identical(TokenStream&, TokenStream&);

void perform_tests(ifstream& is, BeautifulConfig& config) {
  std::stringstream fbuff;
  fbuff << is.rdbuf();
  fbuff.seekg(0, fbuff.beg);
  Parser parser(&fbuff);
  Production* root = parser.parse();
  string s = root->beautiful(config).to_string(config);
  delete(root);
  fbuff.seekg(0, fbuff.beg);
  istringstream ss(s);
  std::cout<<s<<endl;
  std::cout<<"^ for reference ^"<<endl;
  is.seekg(0, is.beg);
  ss.seekg(0, is.beg);
  
  // check if any comments were deleted or re-ordered
  TokenStream lex_logic_pre(&is);
  TokenStream lex_logic_post(&ss);
  
  check_comments_identical(lex_logic_pre,lex_logic_post);
  
  // reset for next test
  is.seekg(0, is.beg);
  ss.seekg(0, ss.beg);
  
  // check if any logic was changed
  TokenStream lex_com_pre(&is);
  TokenStream lex_com_post(&ss);
  
  check_logic_identical(lex_com_pre, lex_com_post);
}

void check_logic_identical(TokenStream& lex_com_pre, TokenStream& lex_com_post) {
  while (true) {
    Token pre, post;
    bool eof_pre = false, eof_post = false;
    while (true) {
      if (lex_com_pre.eof()) {
        eof_pre = true;
        break;
      }
        
      pre = lex_com_pre.read();
      if (pre.type == COMMENT || pre.type == WS)
        continue;
      break;
    }
    
    while (true) {
      if (lex_com_post.eof()) {
        eof_post = true;
        break;
      }
      
      post = lex_com_post.read();
      if (post.type == COMMENT|| post.type == WS)
        continue;
      break;
    }
    
    if (eof_pre ^ eof_post) {
      std::cout<< "Test failed!" <<endl;
      std::cout<< "Difference in logic tokens " <<endl;
      if (eof_pre)
        std::cout<< "pre-beautiful";
      else
        std::cout<< "post-beautiful";
      std::cout << " lexer ended while the other lexer saw ";
      if (eof_pre)
        std::cout<<post.value;
      else
        std::cout<<pre.value;
      std::cout << endl;
      return;
    }
    
    if (eof_pre && eof_post) {
      std::cout<< "Logic tokens are the same in post and pre"<<endl;
      break;
    }
    
    trim(post.value);
    trim(pre.value);
    
    if (post != pre) {
      std::cout<< "Test failed!" <<endl;
      std::cout<< "Difference in comments " <<endl;
      std::cout<< "Pre:  " << pre.value << endl;
      std::cout<< "Post: " << post.value << endl;
      return;
    }
  }
}

void check_comments_identical(TokenStream& lex_logic_pre, TokenStream& lex_logic_post) {
  while (true) {
    Token pre, post;
    bool eof_pre = false, eof_post = false;
    while (true) {
      if (lex_logic_pre.eof()) {
        eof_pre = true;
        break;
      }
        
      pre = lex_logic_pre.read();
      if (pre.type != COMMENT)
        continue;
      break;
    }
    
    while (true) {
      if (lex_logic_post.eof()) {
        eof_post = true;
        break;
      }
      
      post = lex_logic_post.read();
      if (post.type != COMMENT)
        continue;
      break;
    }
    
    if (eof_pre ^ eof_post) {
      std::cout<< "Test failed!" <<endl;
      std::cout<< "Difference in comments " <<endl;
      if (eof_pre)
        std::cout<< "pre-beautiful";
      else
        std::cout<< "post-beautiful";
      std::cout << " lexer ended while the other lexer saw ";
      if (eof_pre)
        std::cout<<post.value;
      else
        std::cout<<pre.value;
      std::cout << endl;
      return;
    }
    
    if (eof_pre && eof_post) {
      std::cout<< "Comments are the same in post and pre"<<endl;
      break;
    }
    
    trim(post.value);
    trim(pre.value);
    
    if (post != pre) {
      std::cout<< "Test failed!" <<endl;
      std::cout<< "Difference in comments " <<endl;
      std::cout<< "Pre:  " << pre.value << endl;
      std::cout<< "Post: " << post.value << endl;
      return;
    }
  }
}