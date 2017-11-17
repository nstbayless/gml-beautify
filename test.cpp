#include <fstream>
#include <cstdlib>
#include <cstring>
#include <sstream>

#include "test.h"
#include "tokenstream.h"
#include "parser.h"
#include "util.h"

using namespace std;

bool check_comments_identical(TokenStream&, TokenStream&);
bool check_logic_identical(TokenStream&, TokenStream&);
bool check_idempotent(std::string, BeautifulConfig& );

bool perform_tests(istream& is, BeautifulConfig config) {
  BeautifulConfig c2(config);
  std::string s_is;
  std::string line;
  while (getline(is, line)) {
    s_is += line + "\n";
  }
  std::stringstream fbuff(s_is);
  fbuff.seekg(0, fbuff.beg);
  Parser parser(&fbuff);
  Production* root = parser.parse();
  config.force_double_equals_comparison = false;
  config.compare_style = 0;
  config.comment_space = false;
  config.cond_parens = 0;
  string s = root->beautiful(config).to_string(config);
  delete(root);
  fbuff.seekg(0, fbuff.beg);
  istringstream ss(s);
  //std::cout<<s<<endl;
  //std::cout<<"^ for reference ^"<<endl;

  std::stringstream log_ss_pre(s_is);
  std::stringstream log_ss_post(s);
  
  // check if any comments were deleted or re-ordered
  TokenStream lex_logic_pre(&log_ss_pre);
  TokenStream lex_logic_post(&log_ss_post);
  
  if (check_comments_identical(lex_logic_pre,lex_logic_post))
    return true;
  
  // check if any logic was changed
  std::stringstream log_ssl_pre(s_is);
  std::stringstream log_ssl_post(s);
  
  TokenStream lex_com_pre(&log_ssl_pre);
  TokenStream lex_com_post(&log_ssl_post);
  
  if (check_logic_identical(lex_com_pre, lex_com_post))
    return true;
  
  if (check_idempotent(s_is, c2)) {
    std::cout<<"Idempotence failed."<<std::endl;
    return true;
  } else std::cout<<"Idempotence test passed."<<std::endl;
  
  return false;
}

bool check_logic_identical(TokenStream& lex_com_pre, TokenStream& lex_com_post) {
  while (true) {
    Token pre, post;
    bool eof_pre = false, eof_post = false;
    while (true) {
      if (lex_com_pre.eof()) {
        eof_pre = true;
        break;
      }
        
      pre = lex_com_pre.read();
      if (pre.type == COMMENT || pre.type == WS || pre.type == ENX)
        continue;
      break;
    }
    
    while (true) {
      if (lex_com_post.eof()) {
        eof_post = true;
        break;
      }
      
      post = lex_com_post.read();
      if (post.type == COMMENT|| post.type == WS || post.type == ENX)
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
      return true;
    }
    
    if (eof_pre && eof_post) {
      std::cout<< "Logic tokens are the same in post and pre"<<endl;
      return false;
    }
    
    trim(post.value);
    trim(pre.value);
    
    if (post != pre) {
      std::cout<< "Test failed!" <<endl;
      std::cout<< "Difference in logic " <<endl;
      std::cout<< "Pre:  " << pre.value << endl;
      std::cout<< "Post: " << post.value << endl;
      return true;
    }
  }
  
  return false;
}

bool check_comments_identical(TokenStream& lex_logic_pre, TokenStream& lex_logic_post) {
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
      return true;
    }
    
    if (eof_pre && eof_post) {
      std::cout<< "Comments are the same in post and pre"<<endl;
      return false;
    }
    
    trim(post.value);
    trim(pre.value);
    
    if (post != pre) {
      std::cout<< "Test failed!" <<endl;
      std::cout<< "Difference in comments " <<endl;
      std::cout<< " Pre: " << pre.value << endl;
      std::cout<< "Post: " << post.value << endl;
      return true;
    }
  }
  
  return false;
}

bool check_idempotent(std::string s, BeautifulConfig& bc) {
  std::stringstream ss1(s);
  Parser parser1(&ss1);
  Production* root1 = parser1.parse();
  std::string beautiful1 = root1->beautiful(bc).to_string(bc);
  std::stringstream ss2(beautiful1);
  Parser parser2(&ss2);
  Production* root2 = parser2.parse();
  std::string beautiful2 = root2->beautiful(bc).to_string(bc);
  if (beautiful1 != beautiful2) {
    
    auto p = first_difference(beautiful1,beautiful2);
    std::cout<<"Idempotence failed." <<endl;
    
    int radius = 80;
    
    std::cout<<beautiful1.substr(max(0,p.first - radius), 2*radius)<<endl;
    
    std::cout<<"*=====================*" <<endl;
    
    std::cout<<beautiful2.substr(max(0,p.first - radius), 2*radius)<<endl;
    
    std::cout<<"Idempotence failed." <<endl;
    std::cout<<"First difference at " <<p.first<<" (line "<<p.second<<")"<<endl;
  }
  return beautiful1 != beautiful2;
}