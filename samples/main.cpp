
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "../include/xml_parser.h"

// Optional sorting module — included only when the file is present.
// If xml_sorter.h is missing this block is skipped and --sort prints a warning.
#if defined(XML_SORTER_AVAILABLE) && __has_include("../include/xml_sorter.h")
#include "../include/xml_sorter.h"
#define SORT_ENABLED 1
#else
#define SORT_ENABLED 0
#endif

#include <filesystem>

int main(int argc, char* argv[]) {
    // Parse arguments: expect [--sort] <file.xml>
    bool do_sort = false;
    std::string input_file;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--sort") {
            do_sort = true;
        } else {
            input_file = arg;
        }
    }

    // Let's make sure the user provided a file!
    if (input_file.empty()) {
        std::cout << "Oops! It looks like you forgot to give me an XML file to read.\n";
        std::cout << "Try running it like this: " << argv[0] << " [--sort] <your_file.xml>\n";
        return 1;
    }

    // Warn if --sort was requested but the sorter module is not compiled in
    if (do_sort && !SORT_ENABLED) {
        std::cerr << "Warning: --sort flag ignored. The xml_sorter module is not available.\n"
                  << "         Add src/xml_sorter.cpp and include/xml_sorter.h, then rebuild.\n";
        do_sort = false;
    }

    // Attempt to open the file they provided
    std::ifstream file(input_file);
    if (!file.is_open()) {
        std::cout << "Uh oh, I couldn't open the file '" << input_file << "'. Are you sure it exists?\n";
        return 1;
    }

    // Read the entire file content into a string stream
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string xml_content = buffer.str();
    file.close();

    try {
        XMLNode root = XMLParser::parse(xml_content);

        // Optionally sort the tree alphabetically by tag name
#if SORT_ENABLED
        if (do_sort) {
            XMLSorter::sort(root);
        }
#endif

        std::string structured_text = XMLParser::format(root);

        // Define output directory and output path
        std::filesystem::path input_path(input_file);
        std::string filename = input_path.filename().string();
        std::filesystem::path output_dir = "xml_outputs";
        std::filesystem::create_directories(output_dir);
        std::filesystem::path output_path = output_dir / (filename + ".structured");

        // Write the structured form to the output file
        std::ofstream outfile(output_path);
        if (!outfile.is_open()) {
            std::cout << "Uh oh, I couldn't create the output file '" << output_path.string() << "'\n";
            return 1;
        }
        outfile << structured_text;
        outfile.close();

        std::cout << structured_text;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing XML: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
