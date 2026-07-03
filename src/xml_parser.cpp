#include "../include/xml_parser.h"
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cctype>

// A helper to trim whitespace
static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

XMLNode XMLParser::parse(const std::string& xml_content) {
    std::vector<XMLNode> stack;
    size_t pos = 0;
    
    while (pos < xml_content.length()) {
        pos = xml_content.find('<', pos);
        if (pos == std::string::npos) {
            break;
        }
        
        // Check if it's a comment or declaration (e.g., <?xml ... ?> or <!-- ... -->)
        if (pos + 1 < xml_content.length() && (xml_content[pos + 1] == '?' || xml_content[pos + 1] == '!')) {
            size_t end_pos = xml_content.find('>', pos);
            if (end_pos == std::string::npos) {
                throw std::runtime_error("Malformed XML: unclosed comment or declaration");
            }
            pos = end_pos + 1;
            continue;
        }
        
        size_t close_bracket = xml_content.find('>', pos);
        if (close_bracket == std::string::npos) {
            throw std::runtime_error("Malformed XML: unclosed tag");
        }
        
        std::string tag_content = xml_content.substr(pos + 1, close_bracket - pos - 1);
        
        if (tag_content.empty()) {
            throw std::runtime_error("Malformed XML: empty tag");
        }
        
        if (tag_content[0] == '/') {
            // Close tag
            std::string tag_name = trim(tag_content.substr(1));
            if (stack.empty()) {
                throw std::runtime_error("Malformed XML: closing tag '" + tag_name + "' without open tag");
            }
            
            XMLNode finished_node = stack.back();
            stack.pop_back();
            
            if (finished_node.name != tag_name) {
                throw std::runtime_error("Malformed XML: mismatched tag. Expected '" + finished_node.name + "' but got '" + tag_name + "'");
            }
            
            if (stack.empty()) {
                return finished_node;
            } else {
                stack.back().children.push_back(finished_node);
            }
            pos = close_bracket + 1;
        } else {
            // Open tag or self-closing tag
            bool self_closing = false;
            if (tag_content.back() == '/') {
                self_closing = true;
                tag_content.pop_back();
            }
            
            std::string tag_name;
            size_t space_pos = tag_content.find(' ');
            if (space_pos == std::string::npos) {
                tag_name = trim(tag_content);
            } else {
                tag_name = trim(tag_content.substr(0, space_pos));
            }
            
            XMLNode new_node;
            new_node.name = tag_name;
            
            if (self_closing) {
                if (stack.empty()) {
                    return new_node;
                } else {
                    stack.back().children.push_back(new_node);
                }
                pos = close_bracket + 1;
            } else {
                size_t next_bracket = xml_content.find('<', close_bracket + 1);
                std::string text_content;
                if (next_bracket != std::string::npos) {
                    text_content = trim(xml_content.substr(close_bracket + 1, next_bracket - (close_bracket + 1)));
                } else {
                    text_content = trim(xml_content.substr(close_bracket + 1));
                }
                new_node.text = text_content;
                
                stack.push_back(new_node);
                pos = close_bracket + 1;
            }
        }
    }
    
    if (!stack.empty()) {
        throw std::runtime_error("Malformed XML: unclosed tag '" + stack.back().name + "'");
    }
    
    throw std::runtime_error("Empty or invalid XML document");
}

std::string XMLParser::format(const XMLNode& node, int indent) {
    std::string result = std::string(indent, ' ') + "- " + node.name;
    if (!node.text.empty()) {
        result += ": " + node.text;
    }
    result += "\n";
    for (const auto& child : node.children) {
        result += format(child, indent + 2);
    }
    return result;
}
