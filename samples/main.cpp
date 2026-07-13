
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "../include/xml_parser.h"

// Pull in the sorting module — but only if it's actually there.
// If you've removed xml_sorter.h, this whole block quietly disappears
// and --sort will just print a friendly warning instead of crashing.
#if defined(XML_SORTER_AVAILABLE) && __has_include("../include/xml_sorter.h")
#include "../include/xml_sorter.h"
#define SORT_ENABLED 1
#else
#define SORT_ENABLED 0
#endif

// Same idea for the search module — completely optional.
// Delete xml_searcher.h and this block vanishes too.
#if defined(XML_SEARCHER_AVAILABLE) && __has_include("../include/xml_searcher.h")
#include "../include/xml_searcher.h"
#define SEARCH_ENABLED 1
#else
#define SEARCH_ENABLED 0
#endif

#include <filesystem>

int main(int argc, char* argv[]) {
    // ── Step 1: Figure out what the user wants ───────────────────────────────
    // Walk through every argument and decide what to do.
    // We support: --sort, --search <tag>, and the XML file path.
    bool        do_sort   = false;
    bool        do_search = false;
    std::string search_tag;
    std::string input_file;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--sort") {
            do_sort = true;
        } else if (arg == "--search") {
            do_search = true;
            if (i + 1 < argc) {
                search_tag = argv[++i];   // grab the tag name that comes right after
            } else {
                std::cerr << "Oops! --search needs a tag name after it.\n"
                          << "Try something like: " << argv[0] << " --search Employee your_file.xml\n";
                return 1;
            }
        } else {
            input_file = arg;
        }
    }

    // ── Step 2: Make sure they gave us a file ────────────────────────────────
    if (input_file.empty()) {
        std::cout << "Hey! You forgot to give me an XML file to read.\n";
        std::cout << "Here's how to use it: " << argv[0] << " [--sort] [--search <tag>] <your_file.xml>\n";
        return 1;
    }

    // ── Step 3: Warn if a flag was used but that module isn't available ──────
    if (do_sort && !SORT_ENABLED) {
        std::cerr << "Heads up: --sort was ignored because the sorting module isn't installed.\n"
                  << "To enable it, add src/xml_sorter.cpp and include/xml_sorter.h, then rebuild.\n";
        do_sort = false;
    }

    if (do_search && !SEARCH_ENABLED) {
        std::cerr << "Heads up: --search was ignored because the search module isn't installed.\n"
                  << "To enable it, add src/xml_searcher.cpp and include/xml_searcher.h, then rebuild.\n";
        do_search = false;
    }

    // ── Step 4: Open and read the file ───────────────────────────────────────
    std::ifstream file(input_file);
    if (!file.is_open()) {
        std::cout << "Uh oh! I couldn't open '" << input_file << "'. Are you sure that file exists?\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string xml_content = buffer.str();
    file.close();

    try {
        // ── Step 5: Parse the XML into a tree ────────────────────────────────
        XMLNode root = XMLParser::parse(xml_content);

        // ── Step 6 (optional): Sort the tree ─────────────────────────────────
#if SORT_ENABLED
        if (do_sort) {
            XMLSorter::sort(root);
        }
#endif

        // ── Step 7 (optional): Search and print results, then stop ───────────
        // When searching, we just print the hits — no output file is written.
#if SEARCH_ENABLED
        if (do_search) {
            auto results = XMLSearcher::search(root, search_tag);
            std::cout << XMLSearcher::format(results, search_tag);
            return 0;
        }
#endif

        // ── Step 8: Format and save the structured output ────────────────────
        std::string structured_text = XMLParser::format(root);

        // Save it to xml_outputs/<filename>.structured
        std::filesystem::path input_path(input_file);
        std::string filename = input_path.filename().string();
        std::filesystem::path output_dir = "xml_outputs";
        std::filesystem::create_directories(output_dir);
        std::filesystem::path output_path = output_dir / (filename + ".structured");

        std::ofstream outfile(output_path);
        if (!outfile.is_open()) {
            std::cout << "Uh oh! Couldn't create the output file '" << output_path.string() << "'\n";
            return 1;
        }
        outfile << structured_text;
        outfile.close();

        // Also print it to the terminal so you can see it right away
        std::cout << structured_text;

    } catch (const std::exception& e) {
        std::cerr << "Something went wrong while parsing: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
