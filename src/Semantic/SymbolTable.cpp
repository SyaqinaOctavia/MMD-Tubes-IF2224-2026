#include "SymbolTable.hpp"
#include <sstream>

namespace {
    constexpr int OBJ_RESERVED = 0;
    constexpr int OBJ_CONST = 1;
    constexpr int OBJ_VAR = 2;
    constexpr int OBJ_TYPE = 3;
    constexpr int OBJ_FUNC = 4;
    constexpr int OBJ_PROC = 5;

    constexpr int TYPE_NONE = 0;
    constexpr int TYPE_INTEGER = 1;
    constexpr int TYPE_REAL = 2;
    constexpr int TYPE_BOOLEAN = 3;
    constexpr int TYPE_CHAR = 4;
    constexpr int TYPE_ARRAY = 5;
    constexpr int TYPE_RECORD = 6;
    constexpr int TYPE_STRING = 7;
}

void SymbolTable::initReserved(){
    tab.clear();
    atab.clear();
    btab.clear();
    display.clear();

    auto enter = [this](const std::string& id, int obj, int type, int ref = 0, int adr = 0) {
        Tab entry;
        entry.id = id;
        entry.obj = obj;
        entry.type = type;
        entry.ref = ref;
        entry.nrm = 1;
        entry.lev = 0;
        entry.adr = adr;
        tab.push_back(entry);
    };

    enter("and",       OBJ_RESERVED, TYPE_NONE);
    enter("array",     OBJ_TYPE,     TYPE_ARRAY);
    enter("begin",     OBJ_RESERVED, TYPE_NONE);
    enter("case",      OBJ_RESERVED, TYPE_NONE);
    enter("const",     OBJ_RESERVED, TYPE_NONE);
    enter("div",       OBJ_RESERVED, TYPE_NONE);
    enter("downto",    OBJ_RESERVED, TYPE_NONE);
    enter("do",        OBJ_RESERVED, TYPE_NONE);
    enter("else",      OBJ_RESERVED, TYPE_NONE);
    enter("end",       OBJ_RESERVED, TYPE_NONE);
    enter("for",       OBJ_RESERVED, TYPE_NONE);
    enter("function",  OBJ_FUNC,     TYPE_NONE);
    enter("if",        OBJ_RESERVED, TYPE_NONE);
    enter("mod",       OBJ_RESERVED, TYPE_NONE);
    enter("not",       OBJ_RESERVED, TYPE_NONE);
    enter("of",        OBJ_RESERVED, TYPE_NONE);
    enter("or",        OBJ_RESERVED, TYPE_NONE);
    enter("procedure", OBJ_PROC,     TYPE_NONE);
    enter("program",   OBJ_RESERVED, TYPE_NONE);
    enter("record",    OBJ_TYPE,     TYPE_RECORD);
    enter("repeat",    OBJ_RESERVED, TYPE_NONE);
    enter("integer",   OBJ_TYPE,     TYPE_INTEGER);
    enter("real",      OBJ_TYPE,     TYPE_REAL);
    enter("boolean",   OBJ_TYPE,     TYPE_BOOLEAN);
    enter("char",      OBJ_TYPE,     TYPE_CHAR);
    enter("string",    OBJ_TYPE,     TYPE_STRING);
    enter("then",      OBJ_RESERVED, TYPE_NONE);
    enter("to",        OBJ_RESERVED, TYPE_NONE);
    enter("type",      OBJ_RESERVED, TYPE_NONE);
    enter("until",     OBJ_RESERVED, TYPE_NONE);
    enter("var",       OBJ_RESERVED, TYPE_NONE);
    enter("while",     OBJ_RESERVED, TYPE_NONE);
    enter("true",      OBJ_CONST,    TYPE_BOOLEAN, 0, 1);
    tab.back().const_value = "true";
    enter("false",     OBJ_CONST,    TYPE_BOOLEAN, 0, 0);
    tab.back().const_value = "false";
    enter("readln",    OBJ_PROC,     TYPE_NONE);
    enter("writeln",   OBJ_PROC,     TYPE_NONE);
    predefinedCount = static_cast<int>(tab.size());

    for (auto& entry : tab)
        entry.link = 0;

    btab.push_back(BlockTab{});
    btab[0].last = 0;
    display.push_back(0);
    level = 0;
}

int SymbolTable::addTab(const std::string& identifiers, int obj, int type, int ref, int nrm, int adr){
    Tab entry;
    entry.id = identifiers;
    entry.obj = obj;
    entry.type = type;
    entry.ref = ref;
    entry.nrm = nrm;
    entry.lev = level;
    entry.adr = adr;
    entry.link = btab.empty() ? 0 : btab[getCurrentBlock()].last;

    tab.push_back(entry);
    int idx = static_cast<int>(tab.size()) - 1;
    if (!btab.empty())
        btab[getCurrentBlock()].last = idx;
    return idx;
}

int SymbolTable::addBlock(){
    BlockTab block;
    block.blocks = static_cast<int>(btab.size());
    btab.push_back(block);
    return block.blocks;
}

int SymbolTable::addArray(int xtyp, int etyp, int eref, int low, int high){
    ArrayTab array;
    array.arrays = atab.size();
    array.xtyp = xtyp;
    array.etyp = etyp;
    array.eref = eref;
    array.low = low;
    array.high = high;
    array.elsz = 1;
    array.size = high >= low ? high - low + 1 : 0;

    atab.push_back(array);
    return array.arrays;
}

int SymbolTable::searchTab(const std::string& identifiers){
    for (int lvl = level; lvl >= 0; --lvl) {
        if (lvl >= static_cast<int>(display.size()))
            continue;

        int blockIdx = display[lvl];
        if (blockIdx < 0 || blockIdx >= static_cast<int>(btab.size()))
            continue;

        int idx = btab[blockIdx].last;
        while (idx > 0 && idx < static_cast<int>(tab.size())) {
            if (tab[idx].id == identifiers)
                return idx;
            idx = tab[idx].link;
        }
    }

    for (int i = 0; i < predefinedCount && i < static_cast<int>(tab.size()); ++i) {
        if (tab[i].id == identifiers)
            return i;
    }

    return -1;
}

int SymbolTable::searchCurrentScope(const std::string& identifiers){
    if (btab.empty() || display.empty())
        return -1;

    int idx = btab[getCurrentBlock()].last;
    while (idx > 0 && idx < static_cast<int>(tab.size())) {
        if (tab[idx].id == identifiers && tab[idx].lev == level)
            return idx;
        idx = tab[idx].link;
    }
    return -1;
}

void SymbolTable::enterScope(){
    int blockIdx = addBlock();
    level++;

    if (static_cast<int>(display.size()) <= level) display.push_back(blockIdx);
    else display[level] = blockIdx;
}

void SymbolTable::exitScope(){
    if (level == 0) return;

    display[level] = 0;
    level--;
}

void SymbolTable::setBlockLast(int blockIdx, int lastIdx){
    btab[blockIdx].last = lastIdx;
}
void SymbolTable::setBlockLpar(int blockIdx, int lparIdx){
    btab[blockIdx].lpar = lparIdx;
}
void SymbolTable::setBlockSizes(int blockIdx, int psze, int vsze){
    btab[blockIdx].psze = psze;
    btab[blockIdx].vsze = vsze;
}

std::string SymbolTable::toString() const{
    std::ostringstream out;

    out << "tab\n";
    out << "idx id obj type ref nrm lev adr link const\n";
    for (std::size_t i = 0; i < tab.size(); ++i) {
        const Tab& entry = tab[i];
        out << i << ' ' << entry.id << ' ' << entry.obj << ' ' << entry.type << ' '
            << entry.ref << ' ' << entry.nrm << ' ' << entry.lev << ' '
            << entry.adr << ' ' << entry.link << ' ' << entry.const_value << '\n';
    }

    out << "btab\n";
    out << "idx last lpar psze vsze\n";
    for (std::size_t i = 0; i < btab.size(); ++i) {
        const BlockTab& block = btab[i];
        out << i << ' ' << block.last << ' ' << block.lpar << ' '
            << block.psze << ' ' << block.vsze << '\n';
    }

    out << "atab\n";
    out << "idx xtyp etyp eref low high elsz size\n";
    for (std::size_t i = 0; i < atab.size(); ++i) {
        const ArrayTab& array = atab[i];
        out << i << ' ' << array.xtyp << ' ' << array.etyp << ' '
            << array.eref << ' ' << array.low << ' ' << array.high << ' '
            << array.elsz << ' ' << array.size << '\n';
    }

    return out.str();
}
