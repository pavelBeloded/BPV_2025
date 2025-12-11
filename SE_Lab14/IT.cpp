#include "stdafx.h"
#include "IT.h"
#include "Error.h"
#include <new>

namespace IT {
    IdTable Create(int size)
    {
        if (size <= 0) {
            throw ERROR_THROW(205);
        }
        if (size > TI_MAXSIZE) {
            throw ERROR_THROW(204);
        }
        try {
            IdTable table{ size, 0, new Entry[size] };
            return table;
        }
        catch (const std::bad_alloc&) {
            throw ERROR_THROW(206);
        }
    }

    Entry GetEntry(IdTable& idtable, int index) {
        if (index < 0 || index >= idtable.size) {
            throw ERROR_THROW(202);
        }
        return idtable.table[index];
    }

    void Delete(IdTable& idtable) {
        delete[] idtable.table;
        idtable.table = nullptr;
    }

    void Add(IdTable& idtable, Entry entry) {
        if (idtable.size >= idtable.maxsize) {
            throw ERROR_THROW(200);
        }
        idtable.table[idtable.size++] = entry;
    }

    int IsId(IdTable& idtable, const std::string& name) {
        for (int i = 0; i < idtable.size; i++) {
            if (name == idtable.table[i].id) {
                return i;
            }
        }
        return TI_NULLIDX;
    }

    int AddId(IdTable& idtable, const std::string& name, IDDATATYPE datatype, IDTYPE type, int idxfirstLE) {
        if (name.empty()) {
            throw ERROR_THROW(207);
        }

        std::string processed_name = name.substr(0, ID_MAXSIZE);

        for (int i = 0; i < idtable.size; i++) {
            if (processed_name == idtable.table[i].id) {
                return i;
            }
        }

        if (idtable.size >= idtable.maxsize) {
            throw ERROR_THROW(200);
        }

        Entry newEntry;
        strcpy_s(newEntry.id, ID_MAXSIZE + 1, processed_name.c_str());
        newEntry.iddatatype = datatype;
        newEntry.idtype = type;
        newEntry.idxfirstLE = idxfirstLE;

        idtable.table[idtable.size] = newEntry;
        return idtable.size++;
    }

     int AddIntLiteral(IdTable& idtable, unsigned int value, int idxfirstLE) {
        for (int i = 0; i < idtable.size; i++) {
            Entry& entry = idtable.table[i];
            if (entry.idtype == L && entry.iddatatype == INT && entry.value.vint == value) {
                return i;
            }
        }

        if (idtable.size >= idtable.maxsize) throw ERROR_THROW(200);

        Entry newEntry;
        std::string literal_name = "L" + std::to_string(idtable.size);
        strcpy_s(newEntry.id, ID_MAXSIZE + 1, literal_name.c_str());

        newEntry.iddatatype = INT;
        newEntry.idtype = L;
        newEntry.value.vint = value;
        newEntry.idxfirstLE = idxfirstLE;

        idtable.table[idtable.size] = newEntry;
        return idtable.size++;
    }

    int AddStringLiteral(IdTable& idtable, const std::string& value, int idxfirstLE) {
        if (value.length() > TI_STR_MAXSIZE - 1) {
            throw ERROR_THROW(203);
        }

        for (int i = 0; i < idtable.size; i++) {
            Entry& entry = idtable.table[i];
            if (entry.idtype == L && entry.iddatatype == STR && strcmp(entry.value.vstr.str, value.c_str()) == 0) {
                return i;
            }
        }

        if (idtable.size >= idtable.maxsize) throw ERROR_THROW(200);

        Entry newEntry;
        std::string literal_name = "L" + std::to_string(idtable.size);
        strcpy_s(newEntry.id, ID_MAXSIZE + 1, literal_name.c_str());

        newEntry.iddatatype = STR;
        newEntry.idtype = L;
        newEntry.idxfirstLE = idxfirstLE;

        newEntry.value.vstr.len = value.length();
        strcpy_s(newEntry.value.vstr.str, TI_STR_MAXSIZE, value.c_str());

        idtable.table[idtable.size] = newEntry;
        return idtable.size++;
    }
}