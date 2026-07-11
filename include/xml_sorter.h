#pragma once

// xml_sorter.h — Optional sorting module for XMLNode trees.
//
// This file is completely independent of the core xml_parser library.
// If this file is removed, xml_parser.h / xml_parser.cpp continue to work
// without any changes.
//
// Usage:
//   #include "xml_sorter.h"
//   XMLSorter::sort(root);   // sorts children alphabetically in-place

#include "xml_parser.h"  // only needs XMLNode

class XMLSorter {
public:
    // Recursively sorts all children of 'node' alphabetically by tag name.
    // The sort is stable within groups of identically-named siblings.
    static void sort(XMLNode& node);
};
