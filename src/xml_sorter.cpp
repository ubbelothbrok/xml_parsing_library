#include "../include/xml_sorter.h"
#include <algorithm>

void XMLSorter::sort(XMLNode& node) {
    // Recurse into children first (bottom-up), so every level is sorted
    for (auto& child : node.children) {
        sort(child);
    }

    // Sort this node's direct children alphabetically by tag name
    std::stable_sort(node.children.begin(), node.children.end(),
        [](const XMLNode& a, const XMLNode& b) {
            return a.name < b.name;
        });
}
