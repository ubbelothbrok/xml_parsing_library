#pragma once

// ─────────────────────────────────────────────────────────────────────────────
// xml_sorter.h — An optional add-on that sorts your XML tree alphabetically.
//
// This module is completely self-contained. The core parser (xml_parser.h)
// has no idea this file exists — and that's intentional. If you ever want
// to remove the sorting feature, just delete this file and xml_sorter.cpp
// and everything else keeps working exactly as before.
//
// Quick usage:
//   #include "xml_sorter.h"
//   XMLSorter::sort(root);   // sorts every level of the tree, in place
// ─────────────────────────────────────────────────────────────────────────────

#include "xml_parser.h"   // we only need this for the XMLNode type

class XMLSorter {
public:
    // Walks the entire tree from the bottom up and sorts each node's
    // children alphabetically by tag name. Siblings with the same tag
    // name keep their original order relative to each other (stable sort).
    static void sort(XMLNode& node);
};
