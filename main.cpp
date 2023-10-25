#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <ranges>

auto readFile = [](const std::string& filename) -> std::vector<std::string> {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
};

auto tokenizeText = [](const std::string& text) -> std::vector<std::string> {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(text);
    while (std::getline(tokenStream, token, ' ')) {
        tokens.push_back(token);
    }
    return tokens;
};

int main() {
    std::vector<std::string> bookLines = readFile("book.in");
    std::vector<std::string> warTermsLines = readFile("war_terms.in");
    std::vector<std::string> peaceTermsLines = readFile("peace_terms.in");

    std::vector<std::string> bookWords;
    for (const auto& line : bookLines) {
        std::vector<std::string> tokens = tokenizeText(line);
        bookWords.insert(bookWords.end(), tokens.begin(), tokens.end());
    }

    std::vector<std::string> warTermsWords;
    for (const auto& line : warTermsLines) {
        std::vector<std::string> tokens = tokenizeText(line);
        warTermsWords.insert(warTermsWords.end(), tokens.begin(), tokens.end());
    }

    std::vector<std::string> peaceTermsWords;
    for (const auto& line : peaceTermsLines) {
        std::vector<std::string> tokens = tokenizeText(line);
        peaceTermsWords.insert(peaceTermsWords.end(), tokens.begin(), tokens.end());
    }

    std::ranges::copy(bookWords, std::ostream_iterator<std::string>(std::cout, "\n"));
    std::cout << std::endl;
    std::ranges::copy(warTermsWords, std::ostream_iterator<std::string>(std::cout, "\n"));
    std::cout << std::endl;
    std::ranges::copy(peaceTermsWords, std::ostream_iterator<std::string>(std::cout, "\n"));
    std::cout << std::endl;

    return 0;
}
