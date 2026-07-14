#pragma once

// ─────────────────────────────────────────────────────────────────────────────
// xml_parser.h — The heart of the XML Parser project.
//
// This is the core of everything. It gives you two things:
//
//   XMLNode  — a simple tree node that holds a tag name, optional text,
//              and a list of child nodes. Think of it like one branch
//              of your XML document.
//
//   XMLParser — the class that does the heavy lifting:
//               * parse()  reads raw XML text and hands you back a tree
//               * format() walks that tree and turns it into something
//                          nice and readable
//
// Everything else in this project (sorting, searching) is optional and
// built on top of this — but this file itself has no dependencies on them.
// ─────────────────────────────────────────────────────────────────────────────

#include <string>
#include <unordered_map>
#include <vector>

// A single node in the XML tree.
// Every tag you see in your XML file becomes one of these.
#ifdef OLD
struct XMLNode {
    std::string name; // the tag name, e.g. "Employee"
    std::string text; // text content inside the tag (if any)
    std::unordered_map<std::string, std::string> attributes;
    std::vector<XMLNode> children; // nested tags live here
};

class XMLParser {
public:
  // Give this function your raw XML string and it'll hand you back
  // the root node of the whole tree. If something looks wrong with
  // the XML, it'll throw a std::runtime_error to let you know.
    static XMLNode parse(const std::string& xml_content);

  // Takes a node (usually the root) and turns the whole tree into
  // a nicely indented, human-readable string you can print or save.
    static std::string format(const XMLNode& node, int indent = 0);
};
#else
struct XMLNode {
    std::string name; // the tag name, e.g. "Employee"
    std::string text; // text content inside the tag (if any)
    std::unordered_map<std::string, std::string> attributes;
    std::vector<XMLNode*> children; // nested tags live here
};

class XMLParser {
public:
  // Give this function your raw XML string and it'll hand you back
  // the root node of the whole tree. If something looks wrong with
  // the XML, it'll throw a std::runtime_error to let you know.
    static XMLNode* parse(const std::string& xml_content);

  // Takes a node (usually the root) and turns the whole tree into
  // a nicely indented, human-readable string you can print or save.
    static std::string format(const XMLNode& node, int indent = 0);
};


#endif
