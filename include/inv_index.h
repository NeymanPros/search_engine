#ifndef INV_INDEX
#define INV_INDEX

#include <entry_struct.h>

#include <string>
#include <vector>
#include <map>

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
    
    std::map<std::string, size_t> UpdateDoc(std::string& doc);
};

#endif // INV_INDEX
