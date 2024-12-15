#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>
#include <limits>

class TrieNode {
public:
    std::unordered_map<char, std::unique_ptr<TrieNode>> children;
    bool is_end_of_word;
    int frequency;

    TrieNode() : is_end_of_word(false), frequency(0) {}
};

class Trie {
public:
    std::unique_ptr<TrieNode> root;

    Trie() : root(std::make_unique<TrieNode>()) {}

    void insert(const std::string& word, int frequency = 1) {
        TrieNode* node = root.get();
        for (char c : word) {
            if (node->children.find(c) == node->children.end()) {
                node->children[c] = std::make_unique<TrieNode>();
            }
            node = node->children[c].get();
        }
        node->is_end_of_word = true;
        node->frequency += frequency;
    }

    bool search(const std::string& word) {
        TrieNode* node = root.get();
        for (char c : word) {
            if (node->children.find(c) == node->children.end()) {
                return false;
            }
            node = node->children[c].get();
        }
        return node->is_end_of_word;
    }

    std::vector<std::string> suggest_corrections(const std::string& word, int max_suggestions = 5) {
        std::vector<std::pair<std::string, int>> word_list = starts_with(word.substr(0, 3));

        if (word_list.empty()) {
            return {};
        }

        std::vector<std::pair<std::string, int>> suggestions;
        for (const auto& pair : word_list) {
            int dist = distance(word, pair.first);
            suggestions.push_back({pair.first, dist});
        }

        std::sort(suggestions.begin(), suggestions.end(), [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
            return a.second < b.second;
        });

        std::vector<std::string> result;
        for (int i = 0; i < std::min(max_suggestions, (int)suggestions.size()); ++i) {
            result.push_back(suggestions[i].first);
        }
        return result;
    }

private:
    void collect_words(TrieNode* node, const std::string& current_prefix, std::vector<std::pair<std::string, int>>& word_list) {
        if (node->is_end_of_word) {
            word_list.push_back({current_prefix, node->frequency});
        }
        for (const auto& child : node->children) {
            collect_words(child.second.get(), current_prefix + child.first, word_list);
        }
    }

    int distance(const std::string& a, const std::string& b) {
        std::vector<std::vector<int>> dp(a.length() + 1, std::vector<int>(b.length() + 1));

        for (int i = 0; i <= a.length(); ++i) {
            for (int j = 0; j <= b.length(); ++j) {
                if (i == 0) {
                    dp[i][j] = j;
                } else if (j == 0) {
                    dp[i][j] = i;
                } else if (a[i - 1] == b[j - 1]) {
                    dp[i][j] = dp[i - 1][j - 1];
                } else {
                    dp[i][j] = 1 + std::min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]});
                }
            }
        }

        return dp[a.length()][b.length()];
    }

    std::vector<std::pair<std::string, int>> starts_with(const std::string& prefix) {
        TrieNode* node = root.get();
        for (char c : prefix) {
            if (node->children.find(c) == node->children.end()) {
                return {};
            }
            node = node->children[c].get();
        }

        std::vector<std::pair<std::string, int>> word_list;
        collect_words(node, prefix, word_list);
        return word_list;
    }
};

void load_dictionary(Trie& trie, const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << file_path << "'." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string word;
        int frequency = 1;
        if (std::getline(ss, word, ',')) {
            if (std::getline(ss, line)) {
                frequency = std::stoi(line);
            }
            trie.insert(word, frequency);
        }
    }

    file.close();
    std::cout << "Dictionary loaded into the Trie." << std::endl;
}

void load_common_words(Trie& trie, const std::string& txt_file_path, int boost_frequency = 1000) {
    std::ifstream file(txt_file_path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << txt_file_path << "'." << std::endl;
        return;
    }

    std::string word;
    while (file >> word) {
        trie.insert(word, boost_frequency);
    }

    file.close();
    std::cout << "Common words loaded into the Trie with boosted priority." << std::endl;
}

void process_batch_file(Trie& trie, const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << file_path << "'." << std::endl;
        return;
    }

    std::string word;
    while (file >> word) {
        if (trie.search(word)) {
            std::cout << "The word '" << word << "' is correct." << std::endl;
        } else {
            std::cout << "The word '" << word << "' is not found." << std::endl;
            auto corrections = trie.suggest_corrections(word);
            if (corrections.empty()) {
                std::cout << "No suggestions available." << std::endl;
            } else {
                std::cout << "Did you mean:" << std::endl;
                for (int i = 0; i < corrections.size(); ++i) {
                    std::cout << i + 1 << ". " << corrections[i] << std::endl;
                }
            }
        }
    }

    file.close();
}

int main() {
    Trie trie;
    load_dictionary(trie, "dictionary.csv");
    load_common_words(trie, "common_words.txt");

    std::string input;
    std::cout << "Enter 'batch' to process a batch file, or 'interactive' for single word checks: ";
    std::cin >> input;

    if (input == "batch") {
        std::string file_path;
        std::cout << "Enter the file path for batch processing: ";
        std::cin >> file_path;
        process_batch_file(trie, file_path);
    } else if (input == "interactive") {
        std::string word;
        while (true) {
            std::cout << "Enter a word (or type 'exit' to quit): ";
            std::cin >> word;

            if (word == "exit") {
                break;
            }

            if (trie.search(word)) {
                std::cout << "The word '" << word << "' is correct." << std::endl;
            } else {
                std::cout << "The word '" << word << "' is not found." << std::endl;
                auto corrections = trie.suggest_corrections(word);
                if (corrections.empty()) {
                    std::cout << "No suggestions available." << std::endl;
                } else {
                    std::cout << "Did you mean:" << std::endl;
                    for (int i = 0; i < corrections.size(); ++i) {
                        std::cout << i + 1 << ". " << corrections[i] << std::endl;
                    }

                    int choice = -1;
                    std::cout << "Enter the number for your choice, or 0 to skip: ";
                    std::cin >> choice;

                    if (choice > 0 && choice <= corrections.size()) {
                        std::cout << "You chose: " << corrections[choice - 1] << std::endl;
                    }
                }
            }
        }
    } else {
        std::cout << "Invalid option, exiting." << std::endl;
    }

    return 0;
}
