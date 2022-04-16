#include <iostream>
#include <fstream>
#include <sstream>
#ifdef WIN32
#include <Windows.h>
#endif

#include "tokenize/tokenizer.hpp"
#include "tokenize/tokenize_exception.hpp"

std::string read_string_from_file(const std::string &file_path) {
    const std::ifstream input_stream(file_path, std::ios_base::binary);

    if (input_stream.fail()) {
        throw std::runtime_error("Failed to open file");
    }

    std::stringstream buffer;
    buffer << input_stream.rdbuf();

    return buffer.str();
}

int main() {
#ifdef WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    const auto text = read_string_from_file("test.karma");
    std::cout << text << std::endl;

    try {
        for(auto i = 0; i < 1000; i++) {
            const karmac::Tokenizer sgd(text);
        }

        const karmac::Tokenizer tokenizer(text);
        for(const auto* token : tokenizer.get_tokens()) {
            std::cout << '[' << token->get_line_offset().line+1 << ':' << token->get_line_offset().offset+1 << "] " << karmac::token_type::get_name(token->get_type()) << ':' << token->to_string() << std::endl;
        }
    } catch(const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
