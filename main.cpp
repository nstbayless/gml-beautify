#include <fstream>
#include <cstdlib>

#include "tokenstream.h"
#include "parser.h"

using namespace std;

int main (int argn, char** argv) {
  const char* filename = "in.gml";
  if (argn >= 2) {
    filename = argv[1];
  }
  ifstream inFile;
  
  inFile.open(filename);
  if (!inFile) {
    cout << "Could not open file " << filename;
    exit(1);
  } else {
    /*
    LLKTokenStream token_stream(&inFile, 4);
    cout << "-- BEGIN --\n";
    while (!token_stream.eof())
      cout << token_stream.read() << endl;
    if (token_stream.peek().type == ERR)
      cout << "-- ERROR !! --\n";
    else
      cout << "-- END --\n";
    */
    Parser parser(&inFile);
    Production* p;
    cout << "-- BEGIN --\n";
    while (p = parser.read())
      cout << p->to_string() + "\n";
    cout << "-- END --\n";
  }
  
  inFile.close();
  return 0;
}
