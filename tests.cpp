#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

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
    int count = 0;
    std::vector<std::vector<std::string>> chapters;
    std::vector<std::string> chapter;
    // split chapters and put into chapters vector by finding the word first "chapter" and ending the chapter when the next "chapter" is found
    std::ranges::for_each(bookWords, [&](const std::string& word) {
        if (word.find("CHAPTER") != std::string::npos) {
            if (!chapter.empty()) {
                chapters.push_back(chapter);
                count++;
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

    auto indexes = std::ranges::views::iota(0, std::ranges::ssize(chapters));
    std::ranges::for_each(indexes, [&chapters, &peaceTerms, &warTerms](int i) -> void {
        auto [peaceWords, warWords] = filterWords(chapters[i], peaceTerms, warTerms);
        double peaceDensity = calculateTermDensity(chapters[i], peaceWords);
        double warDensity = calculateTermDensity(chapters[i], warWords);

        std::cout << "Chapter " << i+1 << ": ";
        if (warDensity >= peaceDensity) {
            std::cout << "war-related\n";
        } else {
            std::cout << "peace-related\n";
        }
    });

};

// Test readFile function
TEST_CASE("ReadFileTest - FileExists") {
    // Create a test file with content
    std::ofstream testFile("test.txt");
    testFile << "Sample text\n";
    testFile.close();

    auto result = readFile("test.txt");

    REQUIRE(result.has_value());
    REQUIRE_EQ(result.value().size(), 1);
    REQUIRE_EQ(result.value()[0], "Sample text");

    // Remove the test file
    std::remove("test.txt");
}

// Test tokenizeText function
TEST_CASE("TokenizeTextTest - TokenizeWords") {
    std::string text = "This is a sample text!";

    auto result = tokenizeText(text);

    REQUIRE_EQ(result.size(), 5);
    REQUIRE_EQ(result[0], "This");
    REQUIRE_EQ(result[1], "is");
    REQUIRE_EQ(result[2], "a");
    REQUIRE_EQ(result[3], "sample");
    REQUIRE_EQ(result[4], "text");
}

// Test splitIntoChapters function
TEST_CASE("SplitIntoChaptersTest - SplitChapters") {
    std::vector<std::string> bookWords = {"Intro", "CHAPTER", "One", "CHAPTER", "Two", "CHAPTER", "Three" };

    auto result = splitIntoChapters(bookWords);

    REQUIRE_EQ(result.size(), 3);
    REQUIRE_EQ(result[0].size(), 2);
    REQUIRE_EQ(result[1].size(), 2);
    REQUIRE_EQ(result[2].size(), 2);
    REQUIRE_EQ(result[0][0], "CHAPTER");
    REQUIRE_EQ(result[1][0], "CHAPTER");
    REQUIRE_EQ(result[2][0], "CHAPTER");
    REQUIRE_EQ(result[0][1], "One");
    REQUIRE_EQ(result[1][1], "Two");
    REQUIRE_EQ(result[2][1], "Three");
}

// Test filterWords function
TEST_CASE("FilterWordsTest - FilterTerms") {
    std::vector<std::string> words = { "peace", "war", "love", "hate" };
    std::vector<std::string> peaceTerms = { "peace", "love" };
    std::vector<std::string> warTerms = { "war", "hate" };

    auto result = filterWords(words, peaceTerms, warTerms);

    REQUIRE_EQ(result.first.size(), 2);
    REQUIRE_EQ(result.second.size(), 2);
    REQUIRE_EQ(result.first[0], "peace");
    REQUIRE_EQ(result.first[1], "love");
    REQUIRE_EQ(result.second[0], "war");
    REQUIRE_EQ(result.second[1], "hate");
}

// Test calculateTermDensity function
TEST_CASE("CalculateTermDensityTest - CalculateDensity") {
    std::vector<std::string> words = { "peace", "war", "love", "hate" };
    std::vector<std::string> terms = { "peace", "love" };

    auto result = calculateTermDensity(words, terms);

    REQUIRE_EQ(result, 0.5);
}

// Test categorize_chapters function
TEST_CASE("CategorizeChaptersTest - Categorize") {
    std::vector<std::vector<std::string>> chapters = {
        {"war", "peace", "love"},
        {"war", "war", "hate"},
        {"peace", "peace", "peace"}
    };
    std::vector<std::string> peaceTerms = { "peace", "love" };
    std::vector<std::string> warTerms = { "war", "hate" };

    // Redirect cout to a stringstream for testing output
    std::stringstream ss;
    std::streambuf* originalCoutStream = std::cout.rdbuf();
    std::cout.rdbuf(ss.rdbuf());

    categorize_chapters(chapters, peaceTerms, warTerms);

    std::cout.rdbuf(originalCoutStream); // Restore cout

    std::string output = ss.str();
    REQUIRE_NE(output.find("war-related"), std::string::npos);
    REQUIRE_NE(output.find("peace-related"), std::string::npos);
    REQUIRE_EQ(std::count(output.begin(), output.end(), '\n'), 3); // Check for 3 chapters in output
}
