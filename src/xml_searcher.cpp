#include "../include/xml_searcher.h"
#include <sstream>

// ── The recursive engine ─────────────────────────────────────────────────────
// This is the function that actually does the searching. It walks through
// every node in the tree, building up a path like "Company > Employees > Employee"
// as it goes deeper. When it finds a tag name that matches, it saves that hit.
void XMLSearcher::searchRecursive(const XMLNode& node,
                                  const std::string& tag_name,
                                  const std::string& current_path,
                                  std::vector<XMLSearchResult>& results)
{
    // Add this node's name to the running path
    std::string this_path = current_path.empty()
                            ? node.name
                            : current_path + " > " + node.name;

    // Is this what we're looking for? If so, save it.
    if (node.name == tag_name) {
        results.push_back({this_path, node.text});
    }

    // Keep digging into each child — no stone left unturned
    for (const auto& child : node.children) {
        searchRecursive(child, tag_name, this_path, results);
    }
}

// ── Public API ───────────────────────────────────────────────────────────────
// Kick off the search from the root and return everything we found.
std::vector<XMLSearchResult> XMLSearcher::search(const XMLNode& root,
                                                  const std::string& tag_name)
{
    std::vector<XMLSearchResult> results;
    searchRecursive(root, tag_name, "", results);
    return results;
}

// Turn the list of results into something readable and print-friendly.
// If nothing was found, we say so clearly rather than just returning a blank screen.
std::string XMLSearcher::format(const std::vector<XMLSearchResult>& results,
                                 const std::string& tag_name)
{
    std::ostringstream out;

    if (results.empty()) {
        out << "Couldn't find any elements with the tag <" << tag_name << ">. "
            << "Double-check the name — the search is case-sensitive!\n";
        return out.str();
    }

    out << "Found " << results.size()
        << " element(s) matching <" << tag_name << ">:\n";
    out << std::string(50, '-') << "\n";

    for (size_t i = 0; i < results.size(); ++i) {
        out << "[" << (i + 1) << "] " << results[i].path << "\n";
        if (!results[i].text.empty()) {
            out << "     value : " << results[i].text << "\n";
        }
    }

    out << std::string(50, '-') << "\n";
    return out.str();
}
