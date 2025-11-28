#ifndef ENTRY_STRUCT
#define ENTRY_STRUCT

#include <cstddef>

struct Entry {
    size_t doc_id; 
    size_t count;
    bool operator ==(const Entry& other) const {
        return (doc_id == other.doc_id &&
        count == other.count);
    }
};

#endif // ENTRY_STRUCT
