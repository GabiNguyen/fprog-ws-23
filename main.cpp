#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <ranges>
#include <map>
#include <optional>

auto readFile = [](const std::string& filename) -> std::optional<std::vector<std::string>> {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }

    // chek if the file is empty
    if (file.peek() == std::ifstream::traits_type::eof()) {
        return std::nullopt;
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        if(line.find("END OF THE PROJECT GUTENBERG EBOOK, WAR AND PEACE") != std::string::npos){
            return lines;
        }
        lines.push_back(line);
    }
    return make_optional(lines);
};

auto tokenizeText = [](const std::string& text) -> std::vector<std::string> {
    std::vector<std::string> tokens;
    std::string token;
    std::for_each(text.begin(), text.end(), [&](char c) {
        if (std::isalpha(c)) {
            token += c;
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

auto splitIntoChapters = [](const std::vector<std::string>& bookWords) -> std::vector<std::vector<std::string>> {
    std::vector<std::vector<std::string>> chapters;
    std::vector<std::string> chapter;
    // split chapters and put into chapters vector by finding the word first "chapter" and ending the chapter when the next "chapter" is found
    std::ranges::for_each(bookWords, [&](const std::string& word) {
        if (word.find("CHAPTER") != std::string::npos) {
            if (!chapter.empty()) {
                chapters.push_back(chapter);
                chapter.clear();
            }
        }
        chapter.push_back(word);
    });
    if (!chapter.empty()) {
        chapters.push_back(chapter);
    }
    //remove the first chapter because it is the title
    chapters.erase(chapters.begin());

    // std::vector<std::string> lastChapter = chapters.back();
    // // remove everything after the string "end of the project gutenberg ebook"
    // auto lastChapter = std::search(lastChapter.begin(), lastChapter.end(), ("end of the project gutenberg ebook"));
    // lastChapter.erase(lastChapter.begin(), lastChapter.end());

    return chapters;
};

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

// auto calculateTermDensity = [](const std::vector<std::string>& words, const std::vector<std::string>& terms) -> double {
//     auto isTerm = [&](const std::string& word) -> bool {
//         return std::ranges::any_of(terms, [&](const std::string& term) { return word == term; });
//     };
//     int termCount = 0;
//     std::ranges::for_each(words, [&](const std::string& word) {
//         if (isTerm(word)) {
//             ++termCount;
//         }
//     });
//     return static_cast<double>(termCount) / words.size();
// };

auto calculateTermDensity = [](const std::vector<std::string>& words, const std::vector<std::string>& terms) -> double {
    return static_cast<double>(terms.size()) / words.size();
};

int main() {
    std::string filename;

    std::cout << " enter the name of the book file: ";
    std::cin >> filename;
    std::optional<std::vector<std::string>> bookLines = readFile(filename);
    if (bookLines == std::nullopt) {
        std::cout << "The file is empty\n";
        return 0;
    }

    std::cout << " enter the name of the war terms file: ";
    std::cin >> filename;
    std::optional<std::vector<std::string>> warTermsLines = readFile(filename);
    if (warTermsLines == std::nullopt) {
        std::cout << "The file is empty\n";
        return 0;
    }

    std::cout << " enter the name of the war terms file: ";
    std::cin >> filename;
    std::optional<std::vector<std::string>> peaceTermsLines = readFile(filename);
    if (peaceTermsLines == std::nullopt) {
        std::cout << "The file is empty\n";
        return 0;
    }

    std::vector<std::string> bookWords;
    for (const auto& line : bookLines.value()) {
        std::vector<std::string> tokens = tokenizeText(line);
        bookWords.insert(bookWords.end(), tokens.begin(), tokens.end());
    }

    std::vector<std::string> warTermsWords;
    for (const auto& line : warTermsLines.value()) {
        std::vector<std::string> tokens = tokenizeText(line);
        warTermsWords.insert(warTermsWords.end(), tokens.begin(), tokens.end());
    }

    std::vector<std::string> peaceTermsWords;
    for (const auto& line : peaceTermsLines.value()) {
        std::vector<std::string> tokens = tokenizeText(line);
        peaceTermsWords.insert(peaceTermsWords.end(), tokens.begin(), tokens.end());
    }

    std::vector<std::vector<std::string>> chapters = splitIntoChapters(bookWords);

    int chapterNumber = 1;

    for (const auto& chapter : chapters) {
        auto [peaceWords, warWords] = filterWords(chapter, peaceTermsWords, warTermsWords);
        double peaceDensity = calculateTermDensity(chapter, peaceWords);
        double warDensity = calculateTermDensity(chapter, warWords);
        // peaceDensities.push_back(peaceDensity);
        // warDensities.push_back(warDensity);

        std::cout << "Chapter " << chapterNumber << ": ";

        if (warDensity > peaceDensity) {
            std::cout << "war-related\n";
        } else {
            std::cout << "peace-related\n";
        }

        chapterNumber++;
    }

    return 0;
}

