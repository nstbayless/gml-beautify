#include <cstring>
#include <unordered_map>

#include "beautify.h"
#include "resource/resource.h"
#include "resource/script.h"
#include "resource/object.h"

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
  ResourceTableEntry() {};
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
  Project(std::string path_to_project_file);
  
  // reads the project file and stores the resource tree.
  void read_project_file();
  
  //! performs beautification and returns if there are any errors
  //! if an error occurs, beautification stops immediately.
  void beautify(BeautifulConfig bc, bool dry = false);
  
private:
  ResourceTree resourceTree;
  std::unordered_map<std::string, ResourceTableEntry> resourceTable;
  std::string root;
  std::string project_file;
  
  //! parses the given DOM tree for the given type of resources
  void read_resource_tree(ResourceTree& out, void* xml, ResourceType type);
  
  //! helper for beautify
  //! beautifies the script resource tree.
  void beautify_script_tree(BeautifulConfig bc, bool dry, ResourceTree&);
  
  //! helper for beautiy_script_tree
  //! beautifies a single script
  void beautify_script(BeautifulConfig bc, bool dry, ResScript&);
  
  //! helper for beautify
  //! beautifies the object resource tree.
  void beautify_object_tree(BeautifulConfig bc, bool dry, ResourceTree&);
  
  //! helper for beautiy_object_tree
  //! beautifies a single object
  void beautify_object(BeautifulConfig bc, bool dry, ResObject&);
};

#endif /*PROJECT_H*/
