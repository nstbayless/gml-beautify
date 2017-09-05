#include <fstream>
#include <cstdlib>

#include "tokenstream.h"
#include "parser.h"

using namespace std;

int main (int argn, char** argv) {
  const char* filename = "in.gml";
  
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
    while (p = parser.read())
      cout << p->beautiful(config) + "\n";
    cout << "-- END --\n";
  }
  
  inFile.close();
  return 0;
}
