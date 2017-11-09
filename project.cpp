#include <pugixml.hpp>

#include "project.h"

const char* RESOURCE_TYPE_NAMES[] = {
  "sprite",
  "sound",
  "background",
  "path",
  "script",
  "shader",
  "font",
  "timeline",
  "object",
  "room",
  "constant"
};

const char* RESOURCE_TREE_NAMES[] = {
  "sprites",
  "sounds",
  "backgrounds",
  "paths",
  "scripts",
  "shaders",
  "fonts",
  "timelines",
  "objects",
  "rooms",
  "constants"
};

ResourceTableEntry::ResourceTableEntry(ResourceType r, std::string path): type(r), path(path), ptr(nullptr)
{ }

ResourceTableEntry::ResourceTableEntry(ResourceType r, Resource* ptr): type(r), ptr(ptr)
{ }

ResourceTableEntry::ResourceTableEntry(const ResourceTableEntry& r): type(r.type), path(r.path), ptr(r.ptr)
{ }

Resource& ResourceTableEntry::get() {
  return *ptr;
}

Project::Project(std::string path): root(path)
{ }

void Project::read_project_file() {
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(root.c_str());
  
  std::cout<<"reading project file " << root<<std::endl;
  std::cout<<"Load result: "<<result.description()<<std::endl;
  
  new (&resourceTree) ResourceTree();
  
  pugi::xml_node assets = doc.child("assets");
  
  for (int r = 0; r < NONE; r++) {
    int prev_rte_size = resourceTable.size();
    ResourceType r_type = (ResourceType)r;
    read_resource_tree(resourceTree, &assets, r_type);
    bool add = false;
    if (resourceTree.list.empty()){
      add = true;
    }
    else if (resourceTree.list.back().type != r_type) {
      add = true;
    }
    if (add)
      resourceTree.list.push_back(ResourceTree());
    
    std::cout<<"Added "<<resourceTable.size() - prev_rte_size<<" "<<RESOURCE_TREE_NAMES[r_type]<<std::endl;
  }
}

std::string resource_name_from_path(std:: string path) {
  size_t last_bsl = path.find_last_of("\\");
  size_t last_rsl = path.find_last_of("/");
  
  size_t sep;
  
  if (last_bsl == std::string::npos) {
    if (last_rsl == std::string::npos)
      return "";
    sep = last_rsl;
  }
  else if (last_rsl == std::string::npos) {
    sep = last_bsl;
  } else {
    sep = std::max(last_rsl, last_bsl);
  }
  
  return path.substr(sep+1,path.length() - sep-1);
}

void Project::read_resource_tree(ResourceTree& root, void* xml_v, ResourceType t) {
  pugi::xml_document& xml = *(pugi::xml_document*)xml_v;
  
  for (pugi::xml_node node: xml.children()) {
    // subtree
    if (node.name() == std::string(RESOURCE_TREE_NAMES[t])) {
      root.list.push_back(ResourceTree());
      root.list.back().type = t;
      ResourceTree& rt = root.list.back();
      read_resource_tree(rt, &node, t);
    }
    
    // actual resource
    if (node.name() == std::string(RESOURCE_TYPE_NAMES[t])) {
      //! add resourceTable entry
      root.list.push_back(ResourceTree());
      root.list.back().type = t;
      std::string value = node.text().get();
      ResourceTableEntry rte(t,value);
      std::string name;
      if (t == CONSTANT) {
        // constants are defined directly in the .project.gmx file; no additional cost
        // to realizing them immediately.
        ResConstant* res = new ResConstant();
        res->value = value;
        new(&rte) ResourceTableEntry(t,res);
        
        // determine resource name:
        name = node.attribute("name").value();
      } else {
        // determine resource name:
        name = resource_name_from_path(value);
      }
      // insert resource table entry
      resourceTable.insert(std::make_pair(name, rte));
      root.list.back().rtkey = name;
    }
  }
}

int Project::beautify(BeautifulConfig bc, bool dry) {
}