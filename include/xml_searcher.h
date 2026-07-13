#pragma once

// ─────────────────────────────────────────────────────────────────────────────
// xml_searcher.h — An optional add-on for finding elements in your XML tree.
//
// This module is completely self-contained. The core parser (xml_parser.h)
// has no idea this file exists — and that's by design. Want to remove the
// search feature? Just delete this file and xml_searcher.cpp. Done.
//
// Quick usage:
//   #include "xml_searcher.h"
//   auto results = XMLSearcher::search(root, "Employee");
//   for (const auto& r : results) {
//       std::cout << r.path << " => " << r.text << "\n";
//   }
// ─────────────────────────────────────────────────────────────────────────────

#include "xml_parser.h"
#include <string>
#include <vector>

// One search hit — everything you'd want to know about a match.
struct XMLSearchResult {
    std::string path;   // the full trail from root to this node, e.g. "Company > Employees > Employee"
    std::string text;   // whatever text was inside the tag (empty string if there was none)
};

class XMLSearcher {
public:
    // Walks the whole tree looking for nodes whose tag name matches
    // what you're after. The search is case-sensitive, so "Email" and
    // "email" are treated as different things. Returns every match it finds.
    static std::vector<XMLSearchResult> search(const XMLNode& root,
                                               const std::string& tag_name);

    // Takes the list of results and turns them into a tidy, readable
    // summary you can print straight to the terminal.
    static std::string format(const std::vector<XMLSearchResult>& results,
                              const std::string& tag_name);

private:
    // The actual recursive engine that does the searching — dives into
    // each child, building up the path as it goes.
    static void searchRecursive(const XMLNode& node,
                                const std::string& tag_name,
                                const std::string& current_path,
                                std::vector<XMLSearchResult>& results);
};
