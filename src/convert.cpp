#include <convert.h>
#include <rel_struct.h>

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

std::vector<std::string> ConverterJSON::GetTextDocuments() {
    std::ifstream configStream(this->config);
    auto j = nlohmann::json::parse(configStream);
    std::vector<std::string> paths = j["files"];
    std::vector<std::string> documents;
    for (const auto &path: paths) {
        std::string doc;
        std::ifstream reading(path);
        getline(reading, doc);
        documents.push_back(doc);
    }
    return documents;
}

int ConverterJSON::GetResponsesLimit() {
    std::ifstream configStream(this->config);
    auto j = nlohmann::json::parse(configStream);
    int limit = j["config"]["max_responses"];
    return limit;
}

std::vector<std::string> ConverterJSON::GetRequests() {
    std::ifstream requestStream(this->requests);
    auto j = nlohmann::json::parse(requestStream);
    auto requests = j["requests"];
    return requests;
}

void ConverterJSON::PutAnswers(const std::vector<std::vector<RelativeIndex>>& answers) {
    nlohmann::json output;
    auto limit = this->GetResponsesLimit();
    
    for (size_t i = 0; i < answers.size(); i++) {
        std::string requestId = "request" + std::string(3 - std::to_string(i+1).length(), '0') 
                                + std::to_string(i+1);
        
        if (answers[i].empty()) {
            output["answers"][requestId]["result"] = false;
        }
        else if (answers[i].size() == 1) {
            output["answers"][requestId]["result"] = true;
            output["answers"][requestId]["docid"] = answers[i][0].doc_id;
            output["answers"][requestId]["rank"] = answers[i][0].rank;
        std::cerr << requestId << " " << answers[i].size() << "\n";
        }
        else {
            output["answers"][requestId]["result"] = true;
            auto write_answer = nlohmann::json::array();
            for (int answer_num = 0; answer_num < answers[i].size() && answer_num < limit; answer_num++) {
                nlohmann::json entry;
                entry["docid"] = answers[i][answer_num].doc_id;
                entry["rank"] = answers[i][answer_num].rank;
                write_answer.push_back(entry);
            }
            output["answers"][requestId]["relevance"] = write_answer;
        }
        
    }
    
    std::ofstream file("../src/answers.json");
    file << output.dump(4);
    file.close();
}
