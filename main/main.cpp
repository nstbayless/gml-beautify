#include <fstream>
#include <cstdlib>
#include <cstring>
#include <sstream>

#include "tokenstream.h"
#include "parser.h"
#include "util.h"
#include "test.h"
#include "error.h"
#include "project/project.h"
#include "project/resource/object.h"
#include "compile.h"

using namespace std;

int main (int argn, char** argv) {
  enum PrintStyle {
    BEAUTIFUL,
    PRODUCTIONS,
    PRODUCTIONS_PF,
  } print_style = BEAUTIFUL;
  bool test_suite = false;
  bool mark_nesting = false;
  bool beautify_project = false;
  bool dry = false;
  bool build = false;
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
      if (strcmp(arg, "mark-nest") == 0) {
        mark_nesting = true;
      }
      if (strcmp(arg,"dry") == 0) {
        dry = true;
      }
      if (strcmp(arg,"dry-run") == 0) {
        dry = true;
      }
      if (strcmp(arg,"compile") == 0 || strcmp(arg, "build") == 0) {
        build = true;
      }
    } else {
      filename = argv[i];
    }
  }
  
  BeautifulConfig config;
  
  if (argn >= 2) {
    filename = argv[1];
  }
  
  // beautify project
  if (ends_with(filename,".project.gmx")) {
    Project* p = new Project(filename);
    p->read_project_file();
    p->beautify(config, dry);
    if (test_suite) {
      p->beautify(config);
    }
    return 0;
  }
  
  // beautify object
  if (ends_with(filename,".object.gmx")) {
    ResObject obj(filename);
    std::cout<<obj.beautify(config, dry);
    return 0;
  }
  
  ifstream inFile;
  
  inFile.open(filename);
  if (!inFile) {
    cout << "Could not open file " << filename;
    exit(1);
  } else {
    if (build) {
      Parser parser(&inFile);
      PrBody* p(parser.parse());
      std::cout<<compileModule(*p).to_string(config,0)<<std::endl;
    } else if (!test_suite) {
      Parser parser(&inFile);
      Production* p;
      bool first = true;
      while (p = parser.read()) {
        switch (print_style) {
          case BEAUTIFUL:
            cout << p->beautiful(config).to_string(config,0,mark_nesting,first,false) << "\n";
            first = false;
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
    } else {
      // test battery
      std::string file_contents = read_file_contents(inFile);
      std::stringstream ss(file_contents);
      if (perform_tests(ss, config))
        throw TestError("Error while testing " + std::string(filename));
    }
  }
  
  inFile.close();
  return 0;
}
