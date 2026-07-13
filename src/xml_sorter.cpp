#include "../include/xml_sorter.h"
#include <algorithm>

void XMLSorter::sort(XMLNode& node) {
    // First, go all the way down to the leaves — sort the deepest levels first.
    // That way, by the time we sort a node's children, those children are
    // already sorted internally too. Nice and tidy from the bottom up.
    for (auto& child : node.children) {
        sort(child);
    }

    // Now sort this node's direct children alphabetically by tag name.
    // We use stable_sort so that if two siblings share the same tag name,
    // they stay in the order they originally appeared — no surprise reshuffling.
    std::stable_sort(node.children.begin(), node.children.end(),
        [](const XMLNode& a, const XMLNode& b) {
            return a.name < b.name;
        });
}
