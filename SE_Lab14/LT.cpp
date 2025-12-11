#include "stdafx.h"
#include "LT.h"
#include "IT.h"
#include "Error.h"
#include "FST.h"
#include <string>
#include <vector>
#include <algorithm>
#include <new>
#include <iostream>

namespace FST_Graphs {
    std::vector<FST::RELATION> make_range(char start, char end, short target_state) {
        std::vector<FST::RELATION> rels;
        for (char c = start; c <= end; ++c) rels.push_back(FST::RELATION(c, target_state));
        return rels;
    }
    void add_char(std::vector<FST::RELATION>& rels, char c, short target_state) {
        rels.push_back(FST::RELATION(c, target_state));
    }
    FST::RELATION* copy_rels_to_raw(const std::vector<FST::RELATION>& source) {
        if (source.empty()) return nullptr;
        FST::RELATION* raw = (FST::RELATION*)malloc(source.size() * sizeof(FST::RELATION));
        if (!raw) throw ERROR_THROW(214);
        for (size_t i = 0; i < source.size(); ++i) raw[i] = source[i];
        return raw;
    }

    FST::FST* fst_id = nullptr;
    FST::FST* fst_int = nullptr;
    FST::FST* fst_oct = nullptr;

    void Init() {
        if (fst_id) return;

        std::vector<FST::RELATION> r0_id, r1_id;
        auto az = make_range('a', 'z', 1); r0_id.insert(r0_id.end(), az.begin(), az.end());
        auto AZ = make_range('A', 'Z', 1); r0_id.insert(r0_id.end(), AZ.begin(), AZ.end());
        add_char(r0_id, '_', 1);
        r1_id = r0_id;
        auto d09 = make_range('0', '9', 1); r1_id.insert(r1_id.end(), d09.begin(), d09.end());

        FST::NODE* nodes_id = new FST::NODE[2];
        nodes_id[0].n_relation = (short)r0_id.size(); nodes_id[0].relations = copy_rels_to_raw(r0_id);
        nodes_id[1].n_relation = (short)r1_id.size(); nodes_id[1].relations = copy_rels_to_raw(r1_id);
        fst_id = new FST::FST(0, FST::NODE()); fst_id->nstates = 2; fst_id->node = nodes_id; fst_id->rstates = new short[2];

        std::vector<FST::RELATION> r_int;
        auto nums = make_range('0', '9', 1);
        r_int.insert(r_int.end(), nums.begin(), nums.end());
        FST::NODE* nodes_int = new FST::NODE[2];
        nodes_int[0].n_relation = (short)r_int.size(); nodes_int[0].relations = copy_rels_to_raw(r_int);
        nodes_int[1].n_relation = (short)r_int.size(); nodes_int[1].relations = copy_rels_to_raw(r_int);
        fst_int = new FST::FST(0, FST::NODE()); fst_int->nstates = 2; fst_int->node = nodes_int; fst_int->rstates = new short[2];

        std::vector<FST::RELATION> r0_oct, r1_oct;
        add_char(r0_oct, '0', 1);
        r1_oct = make_range('0', '7', 1);
        FST::NODE* nodes_oct = new FST::NODE[2];
        nodes_oct[0].n_relation = (short)r0_oct.size(); nodes_oct[0].relations = copy_rels_to_raw(r0_oct);
        nodes_oct[1].n_relation = (short)r1_oct.size(); nodes_oct[1].relations = copy_rels_to_raw(r1_oct);
        fst_oct = new FST::FST(0, FST::NODE()); fst_oct->nstates = 2; fst_oct->node = nodes_oct; fst_oct->rstates = new short[2];
    }
}

LT::Keyword keywords[KEYWORDS_COUNT]{
    {"text",	LEX_TYPE},
    {"uint",	LEX_TYPE},
    {"proc",	LEX_FUNCTION},
    {"var",		LEX_DECLARE},
    {"ret",		LEX_RETURN},
    {"echo",	LEX_PRINT},
    {"entry",	LEX_MAIN},
    {"while",	LEX_WHILE}
};

LT::Separator separators[SEPARATORS_COUNT]{
    {';', LEX_SEMICOLON}, {',', LEX_COMMA},
    {'{', LEX_LEFTBRACE}, {'}', LEX_BRACELET},
    {'(', LEX_LEFTHESIS}, {')', LEX_RIGHTHESIS}
};

namespace LT {

    bool checkFST(FST::FST* _fst, const std::string& word) {
        char* cstr = new char[word.length() + 1];
        strcpy_s(cstr, word.length() + 1, word.c_str());
        FST::FST fst(cstr, *_fst);
        bool result = FST::execute(fst);
        delete[] fst.rstates;
        delete[] fst.node;
        delete[] cstr;
        return result;
    }

    char getKeywordLexem(const std::string& word) {
        for (const auto& kw : keywords) {
            if (kw.keyword == word) return kw.lexem;
        }
        return 0;
    }

    enum class Context {
        GLOBAL, DECLARE_SECTION, FUNCTION_DECLARATION, PARAMETER_LIST
    };

    void FillLTIT(LexTable& lextable, IT::IdTable& idtable, In::IN& in) {
        FST_Graphs::Init();

        std::vector<Context> contextStack;
        contextStack.push_back(Context::GLOBAL);

        IT::IDDATATYPE lastDataType = IT::UNKNOWN;
        std::string currentScope = "global";
        bool entryDefined = false;
        bool nextIsFunctionName = false;

        std::string word = "";
        int line = 1;
        int col = 0;   

        auto processWord = [&](int wordStartCol) {
            if (word.empty()) return;

            char lexem = getKeywordLexem(word);

            if (lexem) {
                Add(lextable, { lexem, line, LT_TI_NULLIDX });
                if (lexem == LEX_DECLARE) contextStack.push_back(Context::DECLARE_SECTION);
                else if (lexem == LEX_FUNCTION) { contextStack.push_back(Context::FUNCTION_DECLARATION); nextIsFunctionName = true; }
                else if (lexem == LEX_MAIN) {
                    if (entryDefined) throw ERROR_THROW_IN(310, line, wordStartCol);
                    entryDefined = true;
                    currentScope = "entry";
                }
                else if (word == "text") lastDataType = IT::STR;
                else if (word == "uint") lastDataType = IT::INT;
            }
            else {
                if (word.length() > 1 && word[0] == '0') {
                    if (checkFST(FST_Graphs::fst_oct, word)) {
                        unsigned int value = 0;
                        try { value = std::stoul(word, nullptr, 8); }
                        catch (...) { throw ERROR_THROW_IN(116, line, wordStartCol); }
                        int idx = IT::AddIntLiteral(idtable, value, line);
                        Add(lextable, { LEX_LITERAL, line, idx });
                    }
                    else throw ERROR_THROW_IN(116, line, wordStartCol);
                }
                else if (isdigit(word[0])) {
                    if (checkFST(FST_Graphs::fst_int, word)) {
                        unsigned int value = 0;
                        try { value = std::stoul(word); }
                        catch (...) { throw ERROR_THROW_IN(116, line, wordStartCol); }
                        int idx = IT::AddIntLiteral(idtable, value, line);
                        Add(lextable, { LEX_LITERAL, line, idx });
                    }
                    else throw ERROR_THROW_IN(117, line, wordStartCol);
                }
                else if (checkFST(FST_Graphs::fst_id, word)) {
                    if (word.length() > ID_MAXSIZE) throw ERROR_THROW_IN(104, line, wordStartCol);
                    IT::IDTYPE idType = IT::V;
                    Context currentContext = contextStack.back();

                    if (nextIsFunctionName) {
                        idType = IT::F;
                        nextIsFunctionName = false;
                        currentScope = word;
                        if (IT::IsId(idtable, word) != TI_NULLIDX) throw ERROR_THROW_IN(302, line, wordStartCol);
                        int idx = IT::AddId(idtable, word, lastDataType, idType, line);
                        Add(lextable, { LEX_ID, line, idx });
                    }
                    else {
                        if (currentContext == Context::PARAMETER_LIST) idType = IT::P;
                        std::string scopedName = currentScope + "$" + word;
                        if (currentContext == Context::DECLARE_SECTION || currentContext == Context::PARAMETER_LIST) {
                            if (IT::IsId(idtable, scopedName) != TI_NULLIDX) throw ERROR_THROW_IN(302, line, wordStartCol);
                            int idx = IT::AddId(idtable, scopedName, lastDataType, idType, line);
                            Add(lextable, { LEX_ID, line, idx });
                        }
                        else {
                            int idx = IT::IsId(idtable, scopedName);
                            if (idx == TI_NULLIDX) idx = IT::IsId(idtable, word);
                            if (idx == TI_NULLIDX) throw ERROR_THROW_IN(301, line, wordStartCol);
                            Add(lextable, { LEX_ID, line, idx });
                        }
                    }
                }
                else {
                    throw ERROR_THROW_IN(114, line, wordStartCol);
                }
            }
            word.clear();
            };

        for (int i = 0; i < in.size; ++i) {
            unsigned char c = in.text[i];
            unsigned char next_c = (i + 1 < in.size) ? in.text[i + 1] : 0;
            col++;   

            if (c == '/' && next_c == '/') {
                processWord(col - (int)word.length());

                while (i < in.size && in.text[i] != '\n') {
                    i++;
                }
                i--;
                continue;
            }

            if (c == '\'') {
                processWord(col - 1 - (int)word.length());

                int strStartCol = col;
                int strLine = line;
                std::string literalContent = "";

                i++;    
                col++;     

                while (i < in.size && in.text[i] != '\'') {
                    if (in.text[i] == '\n') throw ERROR_THROW_IN(119, strLine, col);
                    literalContent += in.text[i];
                    i++;
                    col++;
                }

                if (i >= in.size) throw ERROR_THROW_IN(119, strLine, col);   

                col++;

                int idx = IT::AddStringLiteral(idtable, literalContent, line);
                Add(lextable, { LEX_LITERAL, line, idx });
                continue;
            }

            if (isspace(c)) {
                processWord(col - 1 - (int)word.length());
                if (c == '\n') {
                    line++;
                    col = 0;   
                }
                continue;
            }

            bool is_separator = false;
            for (const auto& sep : separators) {
                if (c == sep.separator) {
                    processWord(col - 1 - (int)word.length());
                    Add(lextable, { sep.lexem, line, LT_TI_NULLIDX });

                    if (c == '(' && contextStack.back() == Context::FUNCTION_DECLARATION) contextStack.back() = Context::PARAMETER_LIST;
                    else if (c == ')' && contextStack.back() == Context::PARAMETER_LIST) contextStack.pop_back();
                    else if (c == ';' && contextStack.back() == Context::DECLARE_SECTION) contextStack.pop_back();
                    else if (c == '{' && contextStack.back() == Context::FUNCTION_DECLARATION) contextStack.pop_back();

                    is_separator = true; break;
                }
            }
            if (is_separator) continue;

            char opLexem = 0;
            bool doubleChar = false;
            if (c == '+') opLexem = LEX_PLUS;
            else if (c == '-') opLexem = LEX_MINUS;
            else if (c == '*') opLexem = LEX_STAR;
            else if (c == '/') opLexem = LEX_DIRSLASH;
            else if (c == '%') opLexem = LEX_MODULO;
            else if (c == '=') { if (next_c == '=') { opLexem = LEX_EQ; doubleChar = true; } else { opLexem = LEX_ASSIGN; if (!contextStack.empty() && contextStack.back() == Context::DECLARE_SECTION) contextStack.pop_back(); } }
            else if (c == '!') { if (next_c == '=') { opLexem = LEX_NE; doubleChar = true; } }
            else if (c == '<') { if (next_c == '=') { opLexem = LEX_LE; doubleChar = true; } else opLexem = LEX_LESS; }
            else if (c == '>') { if (next_c == '=') { opLexem = LEX_GE; doubleChar = true; } else opLexem = LEX_MORE; }

            if (opLexem != 0) {
                processWord(col - 1 - (int)word.length());
                Add(lextable, { opLexem, line, LT_TI_NULLIDX });
                if (doubleChar) {
                    i++;
                    col++;
                }
                continue;
            }

            word += c;
        }
        processWord(col - (int)word.length());
    }

    LexTable Create(int size) { if (size <= 0 || size > LT_MAXSIZE) throw ERROR_THROW(211); return LexTable{ size, 0, new Entry[size] }; }
    void Add(LexTable& lextable, Entry entry) { if (lextable.size >= lextable.maxsize) throw ERROR_THROW(210); lextable.table[lextable.size++] = entry; }
    Entry GetEntry(LexTable& lextable, int index) { if (index < 0 || index >= lextable.size) throw ERROR_THROW(212); return lextable.table[index]; }
    void Delete(LexTable& lextable) { delete[] lextable.table; lextable.table = nullptr; }
}