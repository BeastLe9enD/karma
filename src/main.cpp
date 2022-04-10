#include <iostream>
#include <fstream>
#include <sstream>
#ifdef WIN32
#include <Windows.h>
#endif

#include "tokenize/tokenizer.hpp"
#include "util/text/utf8_iterator.hpp"

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

    const karmac::Tokenizer tokenizer(text);
    for(const auto* token : tokenizer.get_tokens()) {
        std::cout << karmac::token_type::get_name(token->get_type()) << ':' << token->to_string() << std::endl;
    }

    return 0;
}
