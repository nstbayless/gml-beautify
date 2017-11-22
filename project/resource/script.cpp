#include "script.h"
#include "util.h"
#include "parser.h"
#include "test.h"
#include "error.h"

#include <string>

ResScript::ResScript(std::string path): path(path)
{ }

std::string ResScript::beautify(BeautifulConfig bc, bool dry) {
  std::string beautified_script;
  std::string raw_script;

  std::string _path = native_path(path);

  std::cout<<"beautify "<<_path<<std::endl;

  // read in script
  raw_script = read_file_contents(_path);

  // test
  std::stringstream ss(raw_script);
  if (perform_tests(ss, bc))
    throw TestError("Error while testing " + _path);

  // beautify
  Parser p(raw_script);
  Production* syntree = p.parse();
  std::string beautiful = syntree->beautiful(bc).to_string(bc)+"\n";
  delete(syntree);

  if (!dry) {
    std::ofstream out(_path);
    out << beautiful;
    std::cout<<"writing output to "<<path<<std::endl;
  }
  return beautiful;
}