#include <fstream>
#include <cstdlib>
#include <cstring>

#include "tokenstream.h"
#include "parser.h"

using namespace std;

int main (int argn, char** argv) {
  enum PrintStyle {
    BEAUTIFUL,
    PRODUCTIONS,
    PRODUCTIONS_PF,
  } print_style = BEAUTIFUL;
  const char* filename = "in.gml";
  for (int i=1;i<argn;i++) {
    if (strncmp(argv[i],"--",2) == 0) {
      char* arg = (argv[i]+2);
      cout<<arg<<endl;
      if (strcmp(arg, "beautiful") == 0)
        print_style = BEAUTIFUL;
      if (strcmp(arg, "productions") == 0)
        print_style = PRODUCTIONS;
      if (strcmp(arg, "postfixes") == 0 || strcmp(arg, "pf") == 0)
        print_style = PRODUCTIONS_PF;
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
    Parser parser(&inFile);
    Production* p;
    cout << "-- BEGIN --\n";
    while (p = parser.read()) {
      p->flattenPostfixes();
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
  }
  
  inFile.close();
  return 0;
}
