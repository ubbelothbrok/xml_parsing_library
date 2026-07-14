#include "../include/xml_parser.h"
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <vector>

// Strips leading and trailing whitespace from a string.
// Handy for cleaning up text content pulled out of between XML tags.
static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}
#ifdef OLD
XMLNode XMLParser::parse(const std::string& xml_content) {
    // We use a stack to keep track of tags we've opened but haven't closed yet.
    // Think of it like a pile — when we see <Tag>, we push it on.
    // When we see </Tag>, we pop it off and attach it to its parent.
    std::vector<XMLNode> stack;
    size_t pos = 0;

    while (pos < xml_content.length()) {
        // Jump ahead to the next '<' — that's where the action is
        pos = xml_content.find('<', pos);
        if (pos == std::string::npos) {
            break;  // no more tags, we're done
        }

        // Skip over XML declarations (<?xml ...?>) and comments (<!-- ... -->)
        // — we don't need those for building the tree
        if (pos + 1 < xml_content.length() && (xml_content[pos + 1] == '?' || xml_content[pos + 1] == '!')) {
            size_t end_pos = xml_content.find('>', pos);
            if (end_pos == std::string::npos) {
                throw std::runtime_error("Hmm, this XML looks broken — found an unclosed comment or declaration.");
            }
            pos = end_pos + 1;
            continue;
        }

        // Find where this tag closes
        size_t close_bracket = xml_content.find('>', pos);
        if (close_bracket == std::string::npos) {
            throw std::runtime_error("Hmm, this XML looks broken — found a tag that was never closed with '>'.");
        }

        std::string tag_content = xml_content.substr(pos + 1, close_bracket - pos - 1);

        if (tag_content.empty()) {
            throw std::runtime_error("Hmm, this XML looks broken — found an empty '<>' tag with nothing inside.");
        }

        if (tag_content[0] == '/') {
            // ── Closing tag (e.g. </Employee>) ──────────────────────────────
            std::string tag_name = trim(tag_content.substr(1));

            if (stack.empty()) {
                throw std::runtime_error("Hmm, this XML looks broken — found a closing tag </" + tag_name + "> but nothing was open.");
            }

            // Pop the top of the stack — this node is complete!
            XMLNode finished_node = stack.back();
            stack.pop_back();

            if (finished_node.name != tag_name) {
                throw std::runtime_error(
                    "Hmm, the tags don't match up — expected </" + finished_node.name +
                    "> but found </" + tag_name + ">.");
            }

            if (stack.empty()) {
                // That was the root node — we're done!
                return finished_node;
            } else {
                // Attach it to its parent and keep going
                stack.back().children.push_back(finished_node);
            }
            pos = close_bracket + 1;

        } else {
            // ── Opening tag or self-closing tag (e.g. <Employee> or <Member />) ──
            bool self_closing = false;
            if (tag_content.back() == '/') {
                // Self-closing tags like <Member /> don't need a matching </Member>
                self_closing = true;
                tag_content.pop_back();
            }

            // Pull out just the tag name (everything before the first space or attribute)
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
                // Self-closing nodes are leaf nodes — no children, no waiting
                if (stack.empty()) {
                    return new_node;
                } else {
                    stack.back().children.push_back(new_node);
                }
                pos = close_bracket + 1;
            } else {
                // Peek at the text sitting between this tag and the next one
                size_t next_bracket = xml_content.find('<', close_bracket + 1);
                std::string text_content;
                if (next_bracket != std::string::npos) {
                    text_content = trim(xml_content.substr(close_bracket + 1, next_bracket - (close_bracket + 1)));
                } else {
                    text_content = trim(xml_content.substr(close_bracket + 1));
                }
                new_node.text = text_content;

                // Push it onto the stack and wait for its closing tag
                stack.push_back(new_node);
                pos = close_bracket + 1;
            }
        }
    }

    // If anything is still on the stack, someone forgot to close a tag
    if (!stack.empty()) {
        throw std::runtime_error("Hmm, this XML looks broken — the tag <" + stack.back().name + "> was never closed.");
    }

    throw std::runtime_error("This doesn't look like a valid XML document — couldn't find any tags.");
}

std::string XMLParser::format(const XMLNode& node, int indent) {
    // Each node gets a "- TagName" line, optionally followed by its value.
    // Children are indented by two extra spaces to show nesting.
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

#else

static int _skip_whitespace(const std::string& str, int idx) {
    int len = str.length();
    while (idx < len) {
        if (str[idx] == ' ')
            idx++;
        else
            break;
    }
    return idx;
}

static int _next_symbol(const std::string& str, int idx, char c) {
    int len = str.length();
    while (idx < len) {
        if (str[idx] != c)
            idx++;
        else
            break;
    }
    return idx;
}


typedef enum {
    INIT,
    TAG,
    ATTR,
    DECENT,
    CLOSE,
    FINISH,
    ERROR
} parse_stage_t;

bool is_valid_tag_name(const std::string& str, int s, int e) {
  // first index inclusive and second exclusive
    for (int i = s; i < e && i < str.length(); i++) {
        char c = str[i];
        if (i == s) {
            if (!((c >= 'a' && c <= 'z') ||
                  (c >= 'A' && c <= 'Z') ||
                  c == '_')) {
                return false;
            }
            continue;
        }


        if (!((c >= 'a' && c <= 'z') ||
              (c >= 'A' && c <= 'Z') ||
              (c >= '0' && c <= 9) ||
              c == '.' || c == '_' || c == '-')) {
            return false;
        }
    }
    return true;
}

static XMLNode* decent_parse(const std::string& xml_content, int& index, parse_stage_t& status, std::vector<XMLNode*>& stack) {
    if (index >= xml_content.length()) {
        status = FINISH;
    }
    switch (status) {
    case INIT:
        int n;
        n = _next_symbol(xml_content, index, '<');
        if (n >= xml_content.length()) return NULL;
        index = ++n;
        index = _skip_whitespace(xml_content, index);
        if (index >= xml_content.length()) return NULL;

    case TAG:
        break;
    case ATTR:
        break;
    case DECENT:
        break;
    case CLOSE:
        break;
    case ERROR:
        break;
    case FINISH:
        break;
    default:
        throw std::runtime_error("Invalid state at parsing");
        break;
    }
}


XMLNode* XMLParser::parse(const std::string& xml_content) {
    parse_stage_t status = INIT;
    std::vector<XMLNode*> stack;
    int idx = 0;
    return decent_parse(xml_content, idx, status, stack);
}

std::string XMLParser::format(const XMLNode& node, int indent) { return ""; }
#endif // !OLD
