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

auto countOccurrences = [](const std::vector<std::string>& words) -> std::map<std::string, int> {
    std::map<std::string, int> wordCounts;
    std::for_each(words.begin(), words.end(), [&](const std::string& word) {
        ++wordCounts[word];
    });
    return wordCounts;
};

// auto parallelCountOccurrences = [](const std::vector<std::string>& words) -> std::map<std::string, int> {
//     const int numThreads = std::thread::hardware_concurrency();
//     std::vector<std::future<std::map<std::string, int>>> futures;
//     const int chunkSize = words.size() / numThreads;
//     auto chunkStart = words.begin();
//     auto chunkEnd = chunkStart + chunkSize;
//     for (int i = 0; i < numThreads; ++i) {
//         if (i == numThreads - 1) {
//             chunkEnd = words.end();
//         }
//         futures.push_back(std::async(std::launch::async, countOccurrences, std::vector<std::string>(chunkStart, chunkEnd)));
//         chunkStart = chunkEnd;
//         chunkEnd += chunkSize;
//     }
//     std::map<std::string, int> wordCounts;
//     for (auto& future : futures) {
//         auto chunkCounts = future.get();
//         for (const auto& [word, count] : chunkCounts) {
//             wordCounts[word] += count;
//         }
//     }
//     return wordCounts;
// };

int main() {
    // std::vector<std::string> bookLines = readFile("book.in");
    // std::vector<std::string> warTermsLines = readFile("war_terms.in");
    // std::vector<std::string> peaceTermsLines = readFile("peace_terms.in");

    // std::vector<std::string> bookWords;
    // for (const auto& line : bookLines) {
    //     std::vector<std::string> tokens = tokenizeText(line);
    //     bookWords.insert(bookWords.end(), tokens.begin(), tokens.end());
    // }

    // std::vector<std::string> warTermsWords;
    // for (const auto& line : warTermsLines) {
    //     std::vector<std::string> tokens = tokenizeText(line);
    //     warTermsWords.insert(warTermsWords.end(), tokens.begin(), tokens.end());
    // }

    // std::vector<std::string> peaceTermsWords;
    // for (const auto& line : peaceTermsLines) {
    //     std::vector<std::string> tokens = tokenizeText(line);
    //     peaceTermsWords.insert(peaceTermsWords.end(), tokens.begin(), tokens.end());
    // }

    // std::ranges::copy(bookWords, std::ostream_iterator<std::string>(std::cout, "\n"));
    // std::cout << std::endl;
    // std::ranges::copy(warTermsWords, std::ostream_iterator<std::string>(std::cout, "\n"));
    // std::cout << std::endl;
    // std::ranges::copy(peaceTermsWords, std::ostream_iterator<std::string>(std::cout, "\n"));
    // std::cout << std::endl;

    // return 0;

    std::vector<std::string> bookLines = readFile("file.txt");

    // for (const auto& line : bookLines) {
    //     std::cout << line << std::endl;
    // }

    std::vector<std::string> bookWords;
    for (const auto& line : bookLines) {
        std::vector<std::string> tokens = tokenizeText(line);
        bookWords.insert(bookWords.end(), tokens.begin(), tokens.end());
    }

    std::ranges::copy(bookWords, std::ostream_iterator<std::string>(std::cout, "\n"));

    std::vector<std::string> peaceTerms = {"peace", "love", "harmony"};
    std::vector<std::string> warTerms = {"war", "hate", "chaos"};

    auto [peaceWords, warWords] = filterWords(bookWords, peaceTerms, warTerms);

    std::cout << "Peace words:" << std::endl;
    std::ranges::copy(peaceWords, std::ostream_iterator<std::string>(std::cout, "\n"));
    std::cout << std::endl;

    std::cout << "War words:" << std::endl;
    std::ranges::copy(warWords, std::ostream_iterator<std::string>(std::cout, "\n"));
    std::cout << std::endl;

    std::map<std::string, int> peaceWordCounts = countOccurrences(peaceWords);
    std::map<std::string, int> warWordCounts = countOccurrences(warWords);

    std::cout << "Peace word counts:" << std::endl;
    for (const auto& [word, count] : peaceWordCounts) {
        std::cout << word << ": " << count << std::endl;
    }

    std::cout << "War word counts:" << std::endl;
    for (const auto& [word, count] : warWordCounts) {
        std::cout << word << ": " << count << std::endl;
    }

}
