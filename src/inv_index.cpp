#include <inv_index.h>
#include <rel_struct.h>

#include <map>
#include <vector>
#include <string>
#include <thread>

std::map<std::string, size_t> InvertedIndex::UpdateDoc(std::string& doc) {
    std::map<std::string, size_t> counter;
    for (int i = 0; i < doc.length(); i++) {
        std::string word = "";
        for (; i < doc.length() && doc[i] != ' '; i++)
            word += doc[i];
        counter[word]++;
    }
    return counter;
}

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
