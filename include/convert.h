#ifndef CONVERT
#define CONVERT

#include <rel_struct.h>

#include <string>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

class ConverterJSON {
private:
    std::string config;
    std::string requests;
    std::string answers;
    
    inline void CheckConfig() {
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
        config = "../resources/config.json";
        requests = "../resources/requests.json";
        answers = "../resources/answers.json";
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

#endif // CONVERT
