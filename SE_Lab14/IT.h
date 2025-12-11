#pragma once
#include <string>

#define ID_MAXSIZE 32
#define TI_MAXSIZE 4096 
#define TI_INT_DEFAULT 0
#define TI_STR_DEFAULT 0x00
#define TI_NULLIDX 0xffffffff
#define TI_STR_MAXSIZE 255

namespace In {
    struct IN;
}

namespace IT {
    enum IDDATATYPE { INT = 1, STR = 2, UNKNOWN = 0 };
    enum IDTYPE { V = 1, F = 2, P = 3, L = 4 };

    struct Entry {
        int idxfirstLE;
        char id[ID_MAXSIZE + 1];
        IDDATATYPE iddatatype;
        IDTYPE idtype;
        union Value {
            unsigned int vint; 
            struct {
                char len;
                char str[TI_STR_MAXSIZE - 1];
            } vstr;

            Value() : vint(TI_INT_DEFAULT) {}
            Value(unsigned int v) : vint(v) {}
        } value;
    };

    struct IdTable {
        int maxsize;
        int size;
        Entry* table;
    };

    int AddId(IdTable& idtable, const std::string& name, IDDATATYPE datatype, IDTYPE type, int idxfirstLE);
    int AddIntLiteral(IdTable& idtable, unsigned int value, int idxfirstLE);
    int AddStringLiteral(IdTable& idtable, const std::string& value, int idxfirstLE);
    int IsId(IdTable& idtable, const std::string& name);

    IdTable Create(int size);
    void Add(IdTable& idtable, Entry entry);
    Entry GetEntry(IdTable& idtable, int index);
    void Delete(IdTable& idtable);
}