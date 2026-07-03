
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "../include/xml_parser.h"

#include <filesystem>

int main(int argc, char* argv[]) {
    // Let's make sure the user provided a file!
    if (argc < 2) {
        std::cout << "Oops! It looks like you forgot to give me an XML file to read.\n";
        std::cout << "Try running it like this: " << argv[0] << " <your_file.xml>\n";
        return 1;
    }

    // Attempt to open the file they provided
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cout << "Uh oh, I couldn't open the file '" << argv[1] << "'. Are you sure it exists?\n";
        return 1;
    }

    // Read the entire file content into a string stream
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string xml_content = buffer.str();
    file.close();

    try {
        XMLNode root = XMLParser::parse(xml_content);
        std::string structured_text = XMLParser::format(root);

        // Define output directory and output path
        std::filesystem::path input_path(argv[1]);
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
