#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <ranges>
#include <map>

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
    std::for_each(text.begin(), text.end(), [&](char c) {
        if (std::isalpha(c)) {
            token += std::tolower(c);
        } else if (!token.empty()) {
            tokens.push_back(token);
            token.clear();
        }
    });
    if (!token.empty()) {
        tokens.push_back(token);
    }
    return tokens;
};

//split the text into chapters
auto split_in_chapters = [](const std::vector<std::string>& lines) -> std::vector<std::vector<std::string>> {
    std::vector<std::vector<std::string>> chapters;
    std::string chapter;
    bool found_first = false;
    std::ranges::for_each(lines, [&](const std::string& line) {
        if (line.find("CHAPTER") != std::string::npos) {
            if (!chapter.empty() && found_first) {
                found_first = true;
                chapters.push_back(tokenizeText(chapter));
                chapter.clear();
            }
        }
        chapter+=line;
    });
    if (!chapter.empty()) {
        chapters.push_back(tokenizeText(chapter));
    }
    return chapters;
};



// auto filterWords = [](const std::vector<std::string>& words, const std::vector<std::string>& peaceTerms, const std::vector<std::string>& warTerms) -> std::vector<std::string> {
//     auto isFiltered = [&](const std::string& word) -> bool {
//         return std::ranges::any_of(peaceTerms, [&](const std::string& term) { return word == term; }) || std::ranges::any_of(warTerms, [&](const std::string& term) { return word == term; });
//     };
//     std::vector<std::string> filteredWords;
//     std::ranges::copy_if(words, std::back_inserter(filteredWords), [&](const std::string& word) { return !isFiltered(word); });
//     return filteredWords;
// };

auto filterWords = [](const std::vector<std::string>& words, const std::vector<std::string>& peaceTerms, const std::vector<std::string>& warTerms) -> std::pair<std::vector<std::string>, std::vector<std::string>> {
    auto isPeaceTerm = [&](const std::string& word) -> bool {
        return std::ranges::any_of(peaceTerms, [&](const std::string& term) { return word == term; });
    };
    auto isWarTerm = [&](const std::string& word) -> bool {
        return std::ranges::any_of(warTerms, [&](const std::string& term) { return word == term; });
    };
    std::vector<std::string> peaceWords;
    std::vector<std::string> warWords;
    std::ranges::for_each(words, [&](const std::string& word) {
        if (isPeaceTerm(word)) {
            peaceWords.push_back(word);
        } else if (isWarTerm(word)) {
            warWords.push_back(word);
        }
    });
    return std::make_pair(peaceWords, warWords);
};

// mapping -> dictionary -> shuffle -> reduce
// all of standard library

auto countOccurrences = [](const std::vector<std::string>& words) -> std::map<std::string, int> {
    std::map<std::string, int> wordCounts;
    std::for_each(words.begin(), words.end(), [&](const std::string& word) {
        ++wordCounts[word];
    });
    return wordCounts;
};

auto calculateTermDensity = [](const std::vector<std::string>& words, const std::vector<std::string>& terms) -> double {
    
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

    return 0;

}
