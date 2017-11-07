#include <cstring>

#include "beautify.h"

#ifndef PROJECT_H
#define PROJECT_H

enum ResourceType {
  SPRITE,
  SOUND,
  BACKGROUND,
  PATH,
  SCRIPT,
  SHADER,
  FONT,
  TIMELINE,
  OBJECT,
  ROOM,
  NONE
};

struct ResourceTreeNode {
  ResourceType type;
};

struct ResourceTree: ResourceTreeNode {
  std::vector<ResourceTree> list;
};

struct ResourceTreeFile: ResourceTreeNode {
  //! path on disk relative to project root; .gmx or .gml
  std::string path;
};

//! Manages a GMX project on the disk
class Project {
public:
  Project(std::string root_directory);
  
  // reads the project file and stores the resource tree.
  void read_project_file();
  
  //! performs beautification and returns if there are any errors
  //! if an error occurs, beautification stops immediately.
  int beautify(BeautifulConfig bc, bool dry = false);
  
private:
  ResourceTree resourceTree;
  std::string root;
};

#endif /*PROJECT_H*/