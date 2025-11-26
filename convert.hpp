#ifndef CONVERTER
#define CONVERTER

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include "rel_struct.hpp"


class ConverterJSON {
private:
    std::string config;
    std::string requests;
    std::string answers;
    
    void CheckConfig() {
        auto opening = std::ifstream(config);
        if (!opening.is_open())
            throw std::runtime_error("config file is missing");
        nlohmann::json config_file;
        opening >> config_file;
        if (!config_file.contains("config"))
            throw std::runtime_error("config.json does not have config");
        if (!config_file["config"].contains("version"))
            throw std::runtime_error("config.json has incorrect file version");
        if (!config_file["config"].contains("name"))
            throw std::runtime_error("config.json does not have name");
        else 
            std::cout << config_file["config"]["name"] << " v" << config_file["config"]["version"] << " is running...\n";
        if (!config_file.contains("files"))
            throw std::runtime_error("config.json does not have files");
        opening.close();
        for (int i = 0; i < config_file["files"].size(); i++) {
            std::string which = config_file["files"][i];
            opening = std::ifstream(which);
            if (!opening.is_open()){
                std::cout << which << " can't be opened\n";
            }
            opening.close();
        }
        opening = std::ifstream(requests);
        if (!opening.is_open()) 
            throw std::runtime_error("no requests file found");
        opening.close();
    }
public:
    ConverterJSON() {
        config = "../src/config.json";
        requests = "../src/requests.json";
        answers = "../src/answers.json";
        this->CheckConfig();
    };
/// Метод получения содержимого файлов
/// @return Возвращает список с содержимым файлов перечисленных
/// config.json
    std::vector<std::string> GetTextDocuments();
/// Метод считывает поле max_responses для определения предельного
/// количества ответов на один запрос
/// @return
    int GetResponsesLimit();
/// Метод получения запросов из файла requests.json
/// @return возвращает список запросов из файла requests.json
    std::vector<std::string> GetRequests();
/// Положить в файл answers.json результаты поисковых запросов
    void PutAnswers(const std::vector<std::vector<RelativeIndex>>& answers);
};

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

#endif // CONVERTER
