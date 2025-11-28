#include <search_server.h>
#include <rel_struct.h>
#include <inv_index.h>

#include <set>
#include <algorithm>
#include <vector>
#include <string>
#include <map>


std::vector<std::vector<RelativeIndex>> SearchServer::search(const std::vector<std::string>& queries_input) {
    std::vector<std::vector<RelativeIndex>> answers;
    for (const auto& query: queries_input) {
        std::vector<std::string> all_words;
        
        for (int i = 0; i < query.length(); i++) {
            std::string word = "";
            for (; i < query.length() && query[i] != ' '; i++)
                word += query[i];
            if (word != " ")
                all_words.push_back(word);
        }
        
        std::vector<std::string> words;
        while (all_words.size() > 0) {
            std::string new_word = all_words.back();
            all_words.pop_back();
            
            bool found = false;
            for (const auto& w : words) {
                if (w == new_word) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                words.push_back(new_word);
            }
        }
        
        if (words.empty()) {
            answers.push_back({});
            continue;
        }
        
        std::map<size_t, float> doc_rel;
        
        for (const auto& word: words) {
            auto entries = _index.GetWordCount(word);
            for (const auto& entry: entries) {
                doc_rel[entry.doc_id] += entry.count;
            }
        }
        
        bool all_words_present = true;
        for (const auto& word: words) {
            auto entries = _index.GetWordCount(word);
            if (entries.empty()) {
                all_words_present = false;
                break;
            }
            
            std::set<size_t> docs_with_word;
            for (const auto& entry : entries) {
                docs_with_word.insert(entry.doc_id);
            }
            
            auto iter = doc_rel.begin();
            while (iter != doc_rel.end()) {
                if (docs_with_word.find(iter->first) == docs_with_word.end()) { 
                    iter = doc_rel.erase(iter);
                } else {
                    ++iter;
                }
            }
        }
        
        if (doc_rel.empty() || !all_words_present) {
            answers.push_back({});
            continue;
        }
        
        float max_rel = 0;
        for (const auto& [doc_id, rel]: doc_rel) {
            if (rel > max_rel) {
                max_rel = rel;
            }
        }
        
        std::vector<RelativeIndex> current_answer;
        for (const auto& [doc_id, absolute_rel]: doc_rel) {
            float rank = absolute_rel / max_rel;
            current_answer.push_back({doc_id, rank});
        }
        
        std::sort(current_answer.begin(), current_answer.end(),
            [](auto& a, auto& b) {
                return a.rank > b.rank;
            });
        
        answers.push_back(current_answer);
    }
    return answers;
}
