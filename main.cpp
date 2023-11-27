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
    //check if the file exists
    std::ifstream file(filename);
    if (!file.is_open()) {
        return std::nullopt;
    }

    // chek if the file is empty
    if (file.peek() == std::ifstream::traits_type::eof()) {
        return std::nullopt;
    }

    // read the file line by line and put the lines into a vector and get out of the loop when the end of the book is reached
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

auto calculateTermDensity = [](const std::vector<std::string>& words, const std::vector<std::string>& terms) -> double {
    return static_cast<double>(terms.size()) / words.size();
};

auto categorize_chapters = [](const std::vector<std::vector<std::string>>& chapters, const std::vector<std::string>& peaceTerms, const std::vector<std::string>& warTerms) -> void {

    auto indexes = std::ranges::views::iota(0, std::ranges::ssize(chapters)-1);
    std::ranges::for_each(indexes, [&chapters, &peaceTerms, &warTerms](int i) -> void {
        auto [peaceWords, warWords] = filterWords(chapters[i], peaceTerms, warTerms);
        double peaceDensity = calculateTermDensity(chapters[i], peaceWords);
        double warDensity = calculateTermDensity(chapters[i], warWords);

        std::cout << "Chapter " << i << ": ";
        if (warDensity > peaceDensity) {
            std::cout << "war-related\n";
        } else {
            std::cout << "peace-related\n";
        }
    });

};

int main() {
    std::string filename;

    std::cout << " enter the name of the book file: ";
    std::cin >> filename;
    std::optional<std::vector<std::string>> bookLines = readFile(filename);
    if (bookLines == std::nullopt) {
        std::cout << "The file is empty or can't be opened\n";
        return 0;
    }

    std::cout << " enter the name of the war terms file: ";
    std::cin >> filename;
    std::optional<std::vector<std::string>> warTermsLines = readFile(filename);
    if (warTermsLines == std::nullopt) {
        std::cout << "The file is empty or can't be opened\n";
        return 0;
    }

    std::cout << " enter the name of the peace terms file: ";
    std::cin >> filename;
    std::optional<std::vector<std::string>> peaceTermsLines = readFile(filename);
    if (peaceTermsLines == std::nullopt) {
        std::cout << "The file is empty or can't be opened\n";
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
    categorize_chapters(chapters, peaceTermsWords, warTermsWords);

    return 0;
}

