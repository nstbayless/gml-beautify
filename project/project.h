#include <cstring>

#include "beautify.h"
#include "resource/resource.h"
#include "resource/script.h"
#include "resource/object.h"

#ifndef PROJECT_H
#define PROJECT_H

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
  ResourceTable resourceTable;
  std::string root;
  std::string project_file;
  
  //! parses the given DOM tree for the given type of resources
  void read_resource_tree(ResourceTree& out, void* xml, ResourceType type);
  
  //! helper for beautify
  //! beautifies the script resource tree.
  void beautify_script_tree(BeautifulConfig bc, bool dry, ResourceTree&);
  
  //! helper for beautify
  //! beautifies the object resource tree.
  void beautify_object_tree(BeautifulConfig bc, bool dry, ResourceTree&);
};

#endif /*PROJECT_H*/
