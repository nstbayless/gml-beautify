#include <cstring>
#include <unordered_map>

#include "beautify.h"
#include "resource.h"

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
  CONSTANT,
  NONE
};

extern const char* RESOURCE_TYPE_NAMES[NONE];

extern const char* RESOURCE_TREE_NAMES[NONE];


struct ResourceTableEntry {
  ResourceTableEntry(ResourceType, std::string path);
  ResourceTableEntry(ResourceType, Resource* ptr);
  ResourceTableEntry(const ResourceTableEntry&);
  Resource& get();
private:    
  // pointer to resource (if realized)
  Resource* ptr;
  ResourceType type;
  
  // path to resource (to construct if necessary)
  std::string path;
};

struct ResourceTree {
  bool is_leaf;
  
  ResourceType type;
  
  // for trees --
  std::vector<ResourceTree> list;
  
  // for leaves --
  //! key for resource's entry in resource table
  std::string rtkey;
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
  std::unordered_map<std::string, ResourceTableEntry> resourceTable;
  std::string root;
  
  //! parses the given DOM tree for the given type of resources
  void read_resource_tree(ResourceTree& out, void* xml, ResourceType type);
};

#endif /*PROJECT_H*/