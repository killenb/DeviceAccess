/*
 * LogicalNameMapParser.h
 *
 *  Created on: Feb 8, 2016
 *      Author: Martin Hierholzer
 */

#ifndef CHIMERA_TK_LOGICAL_NAME_MAP_PARSER_H
#define CHIMERA_TK_LOGICAL_NAME_MAP_PARSER_H

#include <map>
#include <unordered_set>
#include <boost/shared_ptr.hpp>

#include "RegisterCatalogue.h"
#include "RegisterPath.h"
#include "DynamicValue.h"

// forward declaration
namespace xmlpp {
  class Node;
  class Element;
}

namespace ChimeraTK {

  /** Logical name map: store information from xlmap file and provide it to the LogicalNameMappingBackend and
   *  its register accessors. */
  class LogicalNameMapParser {

    public:

      /** Constructor: parse map from XML file */
      LogicalNameMapParser(const std::string &fileName) {
        parseFile(fileName);
      }

      /** Default constructor: Create empty map. */
      LogicalNameMapParser() {
      }

      /** Obtain the parsed register catalogue */
      const RegisterCatalogue& getCatalogue() const {
        return _catalogue;
      }

      /** Obtain list of all target devices referenced in the map */
      std::unordered_set<std::string> getTargetDevices() const;

    protected:

      /** parse the given XML file */
      void parseFile(const std::string &fileName);

      /** called inside parseFile() to parse an XML element and its sub-elements recursivly */
      void parseElement(RegisterPath currentPath, const xmlpp::Element *element);

      /** throw a parsing error with more information */
      void parsingError(const std::string &message);

      /** Build a Value object for a given subnode. */
      template<typename ValueType>
      DynamicValue<ValueType> getValueFromXmlSubnode(const xmlpp::Node *node, const std::string &subnodeName,
          bool hasDefault=false, ValueType defaultValue=ValueType());

      /** file name of the logical map */
      std::string _fileName;

      /** current register path in the map */
      RegisterPath currentModule;

      /** actual register info map (register name to target information) */
      RegisterCatalogue _catalogue;

  };

} // namespace ChimeraTK

#endif /* CHIMERA_TK_LOGICAL_NAME_MAP_PARSER_H */
