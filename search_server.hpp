#ifndef SEARCH_SERVER
#define SEARCH_SERVER

#include <map>
#include <vector>
#include <string>
#include <set>
#include <algorithm>
#include <thread>
#include "rel_struct.hpp"


struct Entry {
    size_t doc_id; 
    size_t count;
    bool operator ==(const Entry& other) const {
        return (doc_id == other.doc_id &&
        count == other.count);
    }
};

class InvertedIndex {
public:
    InvertedIndex() = default;
/// Обновить или заполнить базу документов, по которой будем совершать поиск
/// @param texts_input содержимое документов
    void UpdateDocumentBase(std::vector<std::string> input_docs);
/// Метод определяет количество вхождений слова word в загруженной базе документов
/// @param word слово, частоту вхождений которого необходимо определить
/// @return возвращает подготовленный список с частотой слов
    std::vector<Entry> GetWordCount(const std::string& word);
private:
    std::vector<std::string> docs; // список содержимого документов
    std::map<std::string, std::vector<Entry>> freq_dictionary; // частотный словарь
    
    std::map<std::string, size_t> UpdateDoc(std::string& doc) {
        std::map<std::string, size_t> counter;
        for (int i = 0; i < doc.length(); i++) {
            std::string word = "";
            for (; i < doc.length() && doc[i] != ' '; i++)
                word += doc[i];
            counter[word]++;
        }
        return counter;
    }
};

void InvertedIndex::UpdateDocumentBase(std::vector<std::string> input_docs) {
    freq_dictionary.clear();
    std::vector<std::map<std::string, size_t>> local_maps{input_docs.size()};
    std::vector<std::thread> threads;
    
    for (size_t doc_id = 0; doc_id < input_docs.size(); doc_id++) {
        threads.emplace_back(
            [this, &input_docs, &local_maps, doc_id] (){
                local_maps[doc_id] = this->UpdateDoc(input_docs[doc_id]);
            }
        );
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    for (size_t i = 0; i < local_maps.size(); i++) {
        for (auto [key, value]: local_maps[i]) 
            freq_dictionary[key].push_back({i, value});
        
    }
}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string& word) {
    return this->freq_dictionary[word];
}

class SearchServer {
public:
/// @param idx в конструктор класса передаётся ссылка на класс InvertedIndex,
/// чтобы SearchServer мог узнать частоту слов встречаемых в запросе
    SearchServer(InvertedIndex& idx) : _index(idx){ };
/// Метод обработки поисковых запросов
/// @param queries_input поисковые запросы взятые из файла requests.json
/// @return возвращает отсортированный список релевантных ответов для заданных запросов
    std::vector<std::vector<RelativeIndex>> search(const std::vector<std::string>& queries_input);
private:
    InvertedIndex _index;
};

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

#endif // SEARCH_SERVER
