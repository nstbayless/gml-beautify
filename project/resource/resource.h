#include "beautify.h"
#include <unordered_map>

#ifndef RESOURCE_H
#define RESOURCE_H

class Resource
{ 
  //! beautifies resource file in-place.
  public:
    virtual void beautify(BeautifulConfig bc, bool dry = false) = 0;
};

class ResConstant : public Resource {
public:
  std::string value;
  virtual void beautify(BeautifulConfig bc, bool dry = false) { };
};

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

//! resource name -> RTE -> resource
typedef std::unordered_map<std::string, ResourceTableEntry> ResourceTable;

#endif /*RESOURCE_H*/