#include <pugixml.hpp>

#include "project.h"
#include "util.h"
#include "test.h"
#include "resource/script.h"

#include <iostream>
#include <fstream>

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
  if (ptr)
    return *ptr;
  // if resource not realized, construct it:
  switch (type) {
    case SCRIPT:
      ptr = new ResScript(path);
      break;
  }
  return *ptr;
}

Project::Project(std::string path): root(path_directory(path)), project_file(path_leaf(path))
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
        name = path_leaf(value);
      }
      // insert resource table entry
      resourceTable.insert(std::make_pair(name, rte));
      root.list.back().rtkey = name;
    }
  }
}

void Project::beautify(BeautifulConfig bc, bool dry) {
  beautify_script_tree(bc, dry, resourceTree.list[SCRIPT]);
}

void Project::beautify_script_tree(BeautifulConfig bc, bool dry, ResourceTree& tree) {
  if (!tree.is_leaf) {
    for (auto iter : tree.list) {
      beautify_script_tree(bc, dry, iter);
    }    
  } else {
    beautify_script(bc, dry, (ResScript&)resourceTable[tree.rtkey].get());
  }
}

void Project::beautify_script(BeautifulConfig bc, bool dry, ResScript& script) {
  std::string beautified_script;
  std::string raw_script;
  
  // read in script
  raw_script = read_file_contents(root + script.path);
  
  // test
  std::stringstream ss(raw_script);
  bool test = perform_tests(ss, bc);
  
  if (!test) {
    throw ParseError("Tests failed on file " script->path);
  }
  
  // beautify
  Parser p(raw_script);
  std::string beautiful p->beautify(bc)->to_string(bc);
  
  std::cout<< beautiful<<std::endl;
  
  if (!dry) {
    // write out
  }
}
