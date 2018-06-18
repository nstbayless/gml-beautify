#include "object.h"
#include "util.h"
#include "parser.h"
#include "test.h"
#include "error.h"

#include <pugixml.hpp>
#include <string>

ResObject::ResObject(std::string path): path(path) {
  
}

std::string ResObject::beautify(BeautifulConfig bc, bool dry) {
  std::string _path = native_path(path);
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(_path.c_str(), pugi::parse_default | pugi::parse_escapes | pugi::parse_comments);
  
  std::cout<<"beautify "<<_path<<std::endl;
  
  pugi::xml_node node_object = doc.child("object");
  pugi::xml_node node_events = node_object.child("events");
  // if no events, skip
  if (node_events.child("event").empty()) {
    return read_file_contents(path);
  }
    
  for (pugi::xml_node event: node_events.children("event")) {
    std::string event_type = event.attribute("eventtype").value();
    std::string enumb = event.attribute("enumb").value();
    int action_n = 0;
    for (pugi::xml_node action: event.children("action")) {
      // is code event:
      if (action.child("kind").text().get() == std::string("7") &&
          action.child("id").text().get() == std::string("603")) {
        std::string descriptor = _path + ", event type " + event_type + ", enumb " + enumb;
        std::cout<<"beautify "<<descriptor<<std::endl;
        // read 
        auto node_code = action.child("arguments").child("argument").child("string");
        std::string raw_code = node_code.text().get();
        
        // skip @noformat
        if (raw_code.find("@noformat") != std::string::npos)
        {
          continue;
        }
        
        // test
        std::stringstream ss(raw_code);
        if (perform_tests(ss, bc))
          throw TestError("Error while testing " + descriptor);
        
        // beautify
        Parser p(raw_code);
        Production* syntree = p.parse();
        std::string beautiful = syntree->beautiful(bc).to_string(bc)+"\n";
        delete(syntree);
        
        if (!dry) {
          node_code.text() = beautiful.c_str();
        }
      }
      action_n ++;
    }
  }
  
  // reformatted object to string
  std::stringstream ssf;
  doc.save(ssf, "  ", pugi::format_default | pugi::format_no_empty_element_tags | pugi::format_no_declaration);
  std::string sf(ssf.str());

  // reformat pernicious <PhysicsShapePoints/> tag
  auto psp_index = sf.rfind("<PhysicsShapePoints>");
  if (psp_index != std::string::npos) {
    std::string sf_last(sf.substr(psp_index, sf.size() - psp_index));
    sf_last = replace_all(sf_last,"<PhysicsShapePoints></PhysicsShapePoints>","<PhysicsShapePoints/>");

    sf = sf.substr(0,psp_index) + sf_last;
  }
  
  // output beautified text
  if (!dry) {
    std::ofstream out(_path.c_str());
    out << sf;
  }
  return sf;
}