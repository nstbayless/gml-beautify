#include "project.h"

Project::Project(std::string path): root(path)
{ }

void Project::read_project_file() {
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(root);
  if (!result) {
    // throw error
    return;
  }
  
  new (&resourceTree) ResourceTree();
  
  
}

int Project::beautify(BeautifulConfig bc, bool dry) {
}