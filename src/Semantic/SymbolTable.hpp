#pragma once 
#include <vector>
#include <string>

// obj: reserved = 0, konstanta = 1, variabel = 2, tipe = 3, fungsi = 4, prosedur = 5
// type: none/procedure = 0, integer = 1, real = 2, boolean = 3, char = 4,
//       array = 5, record = 6, string = 7
struct Tab{
    std::string id;
    int link;
    int obj;
    int type;
    int ref;
    int nrm;
    int lev;
    int adr;

    std::string const_value;

    Tab() : link(0), obj(2), type(0), ref(0), nrm(1), lev(0), adr(0) {}
};

struct ArrayTab{
    int arrays;
    int xtyp;
    int etyp;
    int eref;
    int low;
    int high;
    int elsz;
    int size;

    ArrayTab() : arrays(0), xtyp(1), etyp(1), eref(0), low(0), high(0), elsz(1), size(0) {}
};

struct BlockTab{
    int blocks;
    int last;
    int lpar;
    int psze;
    int vsze;

    BlockTab() : blocks(0), last(0), lpar(0), psze(0), vsze(0) {}
};

class SymbolTable {
    private:
        std::vector<Tab> tab;     
        std::vector<ArrayTab> atab; 
        std::vector<BlockTab> btab;   
        std::vector<int> display;
        int level;
        int predefinedCount;

        void initReserved();

    public:
        SymbolTable() : level(0), predefinedCount(0){
            initReserved();
        }
        int addTab(const std::string& identifiers, int obj, int type, int ref = 0, int nrm = 1, int adr = 0);
        int addBlock();
        int addArray(int xtyp, int etyp, int eref, int low, int high);
        int searchTab(const std::string& identifiers);
        int searchCurrentScope(const std::string& identifiers);

        void enterScope();
        void exitScope();

        Tab& getTab(int index){return tab[index];}
        BlockTab& getBlockTab(int index){return btab[index];}
        ArrayTab& getArrayTab(int index){return atab[index];}

        int getCurrentLevel() {return level;}
        int getCurrentBlock() {return display[level];}
        int getTabSize() {return tab.size();}
        int getBlocktabSize() {return btab.size();}
        int getArraytabSize() {return atab.size();}
        const std::vector<int>& getDisplay() {return display;}

        void setBlockLast(int blockIdx, int lastIdx);
        void setBlockLpar(int blockIdx, int lparIdx);
        void setBlockSizes(int blockIdx, int psze, int vsze);

        std::string toString() const;
};
