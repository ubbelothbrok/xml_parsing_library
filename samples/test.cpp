#include "../include/xml_parser.h"
#include <stdexcept>
#include <iostream>
#define ASSERT(x, test_name)                                                     \
    if (!(x)) {                                                                  \
        std::cout << "Failed: \"" << test_name << "\" at line " << __LINE__ << std::endl; \
        fail++;                                                                  \
    } else {                                                                     \
        pass++;                                                                  \
    }

extern bool is_valid_tag_name(const std::string& str, int s, int e);
int pass = 0, fail = 0;
int main() {
    std::string s;
    s = "Hospital";
    ASSERT(is_valid_tag_name(s, 0, s.length()) == false, s);
    s = "_Hospital";
    ASSERT(is_valid_tag_name(s, 0, s.length()) == true, s);
    s = "9Hospital";
    ASSERT(is_valid_tag_name(s, 0, s.length()) == false, s);
    s = "Hos pital";
    ASSERT(is_valid_tag_name(s, 0, s.length()) == false, s);
    s = " Hosp!ital";
    ASSERT(is_valid_tag_name(s, 0, s.length()) == false, s);
    s = "Hospital";
    ASSERT(is_valid_tag_name(s, 0, s.length()) == true, s);

    std::cout << "Result: " << pass << "/" << pass + fail << std::endl;
}
