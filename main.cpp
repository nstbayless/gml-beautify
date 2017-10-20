#include <fstream>
#include <cstdlib>
#include <cstring>
#include <sstream>

#include "tokenstream.h"
#include "parser.h"

using namespace std;
void perform_tests(ifstream& is, BeautifulConfig& config) ;

int main (int argn, char** argv) {
  enum PrintStyle {
    BEAUTIFUL,
    PRODUCTIONS,
    PRODUCTIONS_PF,
  } print_style = BEAUTIFUL;
  bool test_suite = false;
  const char* filename = "in.gml";
  for (int i=1;i<argn;i++) {
    if (strncmp(argv[i],"--",2) == 0) {
      char* arg = (argv[i]+2);
      if (strcmp(arg, "beautiful") == 0)
        print_style = BEAUTIFUL;
      if (strcmp(arg, "productions") == 0)
        print_style = PRODUCTIONS;
      if (strcmp(arg, "postfixes") == 0 || strcmp(arg, "pf") == 0)
        print_style = PRODUCTIONS_PF;
      if (strcmp(arg, "test") == 0) {
        test_suite = true;
      }
    } else {
      filename = argv[i];
    }
  }
  
  BeautifulConfig config;
  
  if (argn >= 2) {
    filename = argv[1];
  }
  ifstream inFile;
  
  inFile.open(filename);
  if (!inFile) {
    cout << "Could not open file " << filename;
    exit(1);
  } else {
    if (!test_suite) {
      Parser parser(&inFile);
      Production* p;
      cout << "-- BEGIN --\n";
      while (p = parser.read()) {
        switch (print_style) {
          case BEAUTIFUL:
            cout << p->beautiful(config) + "\n";
            break;
          case PRODUCTIONS:
            cout << p->to_string() + "\n";
            break;
          case PRODUCTIONS_PF:
            cout << p->to_string() + "\n";
            for (int i=p->infixes.size() - p->postfix_n; i < p->infixes.size(); i++) {
              cout << "~" << i - p->infixes.size() + p->postfix_n << ": ";
              if (p->infixes[i])
                cout << p->infixes[i]->to_string();
              cout << endl;
            }
            break;
        }
        delete(p);
      }
      cout << "-- END --\n";
    } else {
      // test battery
      perform_tests(inFile, config);
    }
  }
  
  inFile.close();
  return 0;
}

void perform_tests(ifstream& is, BeautifulConfig& config) {
  std::stringstream fbuff;
  fbuff << is.rdbuf();
  fbuff.seekg(0, fbuff.beg);
  Parser parser(&fbuff);
  Production* root = parser.parse();
  string s = root->beautiful(config);
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
    
    if (post != pre) {
      std::cout<< "Test failed!" <<endl;
      std::cout<< "Difference in comments " <<endl;
      std::cout<< "Pre:  " << pre.value << endl;
      std::cout<< "Post: " << post.value << endl;
      return;
    }
    
    if (eof_pre && eof_post)
      std::cout<< "Comments are the same in post and pre"<<endl;
      break;
  }
  
  // reset for next test
  is.seekg(0, is.beg);
  ss.seekg(0, ss.beg);
  
  // check if any logic was changed
  TokenStream lex_com_pre(&is);
  TokenStream lex_com_post(&ss);
}