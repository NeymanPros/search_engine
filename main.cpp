#include <convert.h>
#include <inv_index.h>
#include <search_server.h>
#include <iostream>

int main() {
    try {
        ConverterJSON converter{};
        
        auto documents = converter.GetTextDocuments();
        auto requests = converter.GetRequests();
        
        InvertedIndex ind{};
        ind.UpdateDocumentBase(documents);
        
        SearchServer server{ind};
        auto answer = server.search(requests);
        converter.PutAnswers(answer);
        for (auto i: answer) {
            for (auto j: i) {
                std::cout << j.doc_id << " " << j.rank << "\n";
            }
        }
    }
    catch (std::exception &e) {
        std::cerr << e.what() << "\n";;
        return 1;
    }
}

