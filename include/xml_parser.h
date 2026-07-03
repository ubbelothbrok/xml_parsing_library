#pragma once

#include <string>
#include <vector>

struct XMLNode {
    std::string name;
    std::string text;
    std::vector<XMLNode> children;
};

class XMLParser {
public:
    // Parses XML content and returns the root node.
    // Throws std::runtime_error on parsing failures.
    static XMLNode parse(const std::string& xml_content);

    // Formats the XMLNode tree structure into a human-readable string.
    static std::string format(const XMLNode& node, int indent = 0);
};
