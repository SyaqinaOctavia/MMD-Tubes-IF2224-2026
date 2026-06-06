#include "Lexer/lexer.hpp"
#include "Parser/Parser.hpp"
#include "Parser/ParseTree.hpp"
#include "Semantic/ASTer.hpp"
#include "Semantic/SymbolTable.hpp"
#include "Semantic/SemanticAnalyzer.hpp"
#include "Interpreter/ICG.hpp"
#include "Interpreter/Interpreter.hpp"
#include <cstring>
#include <iostream>
#include <fstream>
#include <stdexcept>

static void runPipeline(const string& sourceFile, const string& icodeFile) {
    // Lex
    Lexer lex;
    vector<Token> tokens = lex.generateToken(sourceFile);

    // Parse
    Parser parser(tokens);
    shared_ptr<TreeNode> tree = parser.buildTree();
    if (!tree) { cerr << "Parser gagal.\n"; return; }

    // Semantic Analysis + build Decorated AST
    ASTer aster;
    shared_ptr<ASTNode> astree = aster.buildProgramNode(tree);
    if (!astree) { cerr << "ASTer gagal.\n"; return; }

    SymbolTable symtab;
    SemanticAnalyzer seman(symtab);

    string tempAst = icodeFile.empty() ? "temp_ast.txt" : icodeFile + ".ast.tmp";
    bool ok = seman.analyzeAndOutput(astree, tempAst);
    if (!ok) { cerr << "Semantic Analysis menemukan error.\n"; return; }

    cerr << "[DEBUG] Semantic OK, starting ICG...\n";

    // Intermediate Code Generation
    IntermediateCode icg(symtab);
    vector<Bytecode> code;
    try {
        code = icg.generate(astree);
    } catch (const exception& e) {
        cerr << "ICG Error: " << e.what() << "\n";
        return;
    }

    cerr << "[DEBUG] ICG done, " << code.size() << " instructions\n";

    if (!icodeFile.empty()) {
        icg.writeToFile(code, icodeFile);
        cout << "=== INTERMEDIATE CODE ===\n";
        icg.writeToStream(code, cout);
        cout << "\n";
    } else {
        cout << "=== INTERMEDIATE CODE ===\n";
        icg.writeToStream(code, cout);
        cout << "\n";
    }

    cout << "=== OUTPUT PROGRAM ===\n";
    Interpreter interp;
    interp.setStringTable(icg.getStringTable());
    try {
        interp.execute(code);
    } catch (const RuntimeError& e) {
        cerr << e.what() << "\n";
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
    }
}