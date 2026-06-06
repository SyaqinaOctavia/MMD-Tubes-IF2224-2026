# MMD-Tubes-IF2224-2026
![source: emesan desu](mascot.jpg)

# Arion Compiler
Codebase ini merupakan implementasi Lexical, Syntax, Semantic Analyzer, dan Interpreter untuk bahasa pemrograman Arion sebagai bagian dari Tugas Besar mata kuliah IF2224 Teori Bahasa Formal dan Automata.

## Deskripsi Program

<p align="justify">
Program ini menggabungkan proses <i>Lexical Analyzer</i> (Lexer), <i>Syntax Analysis</i> (Parser), <i>Semantic Analyzer</i>, dan <i>Interpreter</i> dari sebuah <i>source code</i>. Proses pembentukan token dalam lexer menggunakan <b>Deterministic Finite Automata (DFA)</b> untuk mengenali pola karakter dan menghasilkan token sesuai dengan spesifikasi bahasa pemrograman Arion. Selanjutnya, proses analisa syntax dari token yang dihasilkan menggunakan algoritma <b>Recursive Descent</b> yang didasarkan pada <b>Context Free Grammar (CFG)</b>. Proses analisa semantic dari parse tree dilakukan menggunakan <b>Decorated AST</b> dan <b>Symbol Table</b>. Terakhir, interpreter menghasilkan <b>Intermediate Code (ICG)</b> dan mengeksekusi program.
</p>

## Identitas Kelompok

Kode Kelompok: **MMD**

Anggota:
* `13524040` - `Kloce Paul William Saragih`
* `13524042` - `Suryani Mulia Utami `
* `13524048` - `Josh Reinhart Zidik `
* `13524088` - `Syaqina Octavia Rizha `

## Struktur Proyek

```text
MMD-TUBES-IF2224-2026/
│
├── src/
│ ├── main.cpp
│ ├── Symbol.cpp
│ ├── Symbol.hpp
│ ├── Token.cpp
│ ├── Token.hpp
│ │
│ ├── Lexer/
│ │ ├── dfa_rules.txt
│ │ ├── DFA.cpp
│ │ ├── DFA.hpp
│ │ ├── lexer.cpp
│ │ └── lexer.hpp
│ │
│ ├── Parser/
│ │ ├── Parser.cpp
│ │ ├── Parser.hpp
│ │ ├── ParseTree.cpp
│ │ └── ParseTree.hpp
│ │
│ └── Semantic/
│ │ ├── AST.cpp
│ │ ├── AST.hpp
│ │ ├── ASTer.cpp
│ │ ├── ASTer.hpp
│ │ ├── SemanticAnalyzer.cpp
│ │ ├── SemanticAnalyzer.hpp
│ │ ├── SymbolTable.cpp
│ │ └── SymbolTable.hpp
│ │
│ └── Interpreter/
│   ├── ICG.cpp
│   ├── ICG.hpp
│   ├── Instructions.hpp
│   ├── Interpreter.cpp
│   ├── Interpreter.hpp
│   ├── Pipeline.hpp
│   └── Stack.hpp
│
├── test/
│   ├── milestone-1/
│   ├── milestone-2/
│   ├── milestone-3/
│   └── milestone-4/
│
├── doc/
│   └── ...
│
├── README.md
├── Makefile
└── main
│
├── README.md
└── Makefile
```

## Requirements

* `g++` and `make` 
## Instalasi dan Kompilasi

### Clone Repository

```bash
gh repo clone SyaqinaOctavia/MMD-Tubes-IF2224-2026
cd MMD-Tubes-IF2224-2026
```

### Kompilasi Program

```bash
make
```

Program akan dikompilasi dan menghasilkan *executable* bernama `main`.

### Membersihkan Build Files

```bash
make clean
```

## Cara Penggunaan

### Format Perintah

```bash
make run ARGS="<option> <intput-file> <output-file>"
```
atau
```bash
./main <option> <intput-file> <output-file>
```

`option` digunakan untuk memilih antara:
- *Lexer* (`option == 1`)
- *Parser* (`option == 2a` untuk input file berisi token; `option == 2b` untuk input *source code*)
- *Semantic Analyzer* (`option == 3a` untuk input file berisi hasil parse tree; `option == 3b` untuk input *source code*)
- *Interpreter* (`option == 4a` untuk menjalankan dan output intermediate code; `option == 4b` untuk output intermediate code ke file)

### Contoh Penggunaan

```bash
# Lexer
make run ARGS="1 test/milestone-1/input-1.txt test/milestone-1/output-1.txt"

# Parser dari source code
make run ARGS="2b test/milestone-2/input-1.txt test/milestone-2/output-1.txt"

# Semantic Analyzer dari source code
make run ARGS="3b test/milestone-3/input-1.txt test/milestone-3/output-1.txt"

# Interpreter (run code)
make run ARGS="4a test/milestone-4/input-1.txt"

# Interpreter (output to file)
make run ARGS="4b test/milestone-4/input-1.txt test/milestone-4/output-1.txt"
```

### Format Input

Input berupa file teks (`.txt`) yang berisi source code bahasa pemrograman Arion.

Contoh input:

```pascal
program Hello;
var
  a, b: integer;
begin
  a := 5;
  b := a + 10;
end.
```

### Format Output

Output berupa daftar token yang dihasilkan, satu token per baris.

Contoh output hasil *lexer*:

```text
programsy
ident (Hello)
semicolon
varsy
ident (a)
comma
ident (b)
colon
ident (integer)
semicolon
beginsy
ident (a)
becomes
intcon (5)
semicolon
```

Contoh output hasil *parser*:

```text
<program>
├─ <program-header>
│  ├─ programsy
│  ├─ ident(Hello)
│  ├─ semicolon
├─ <declaration-part>
│  ├─ <var-declaration>
│  │  ├─ varsy
│  │  ├─ <identifier-list>
│  │  │  ├─ ident(a)
│  │  │  ├─ comma
│  │  │  ├─ ident(b)
│  │  ├─ colon
│  │  ├─ <type>
│  │  │  ├─ ident(integer)
│  │  ├─ semicolon
.
.
.
```

Contoh output hasil *semantic analyzer*:

```text
SYMBOL TABLE
IDX  ID              OBJ         TYPE        REF   NRM   LEV   ADR   CONST_VAL       
0    and             0           0           0     1     0     0                     
1    array           3           5           0     1     0     0                     
2    begin           0           0           0     1     0     0                     
3    case            0           0           0     1     0     0                     
4    const           0           0           0     1     0     0                     
5    div             0           0           0     1     0     0                     
...

BLOCK TABLE
IDX  LAST    LPAR    PSZE    VSZE    
0    38      0       0       2       

ARRAY TABLE
IDX  XTYP    ETYP    EREF    LOW     HIGH    ELSZ    SIZE    
idx xtyp etyp eref low high elsz size

=== DECORATED ABSTRACT SYNTAX TREE ===
ProgramNode(name: 'Hello')
├─ Declarations
│  └─ VarDecl('a') → tab_idx:37, type:integer, lev:0
│  └─ VarDecl('b') → tab_idx:38, type:integer, lev:0
└─ Block (main compound)
   ├─ Assign → type:void
   │  ├─ Var('a') → tab_idx:37, type:integer, lev:0
   │  └─ Literal(5) → type:integer
   ├─ Assign → type:void
   │  ├─ Var('b') → tab_idx:38, type:integer, lev:0
   │  └─ BinaryOp('plus') → type:integer
   │     ├─ Var('a') → tab_idx:37, type:integer, lev:0
   │     └─ Literal(10) → type:integer
   ├─ CallStmt('writeln')
   │  ├─ Literal('Result = ') → type:string
   │  └─ Var('b') → tab_idx:38, type:integer, lev:0
   └─ Compound(0 stmts)
```

Contoh output hasil *interpreter* (intermediate code):

```text
0 INT 0 5
1 LIT 0 5
2 STO 0 3
3 LOD 0 3
4 LIT 0 10
5 OPR 0 2
6 STO 0 4
7 LITS 0 0
8 OPR 0 13
9 LOD 0 4
10 OPR 0 14
11 RET 0 0
```

### Token yang Dapat Dikenali

Program dapat mengenali 52 jenis token, termasuk:

* **Konstanta:** `intcon`, `realcon`, `charcon`, `string`
* **Operator Aritmatika:** `plus` (+), `minus` (-), `times` (*), `idiv` (div), `rdiv` (/), `imod` (MOD)
* **Operator Logika:** `notsy` (NOT), `andsy` (AND), `orsy` (OR)
* **Operator Relasional:** `eql` (==), `neq` (<>), `gtr` (>), `geq` (>=), `lss` (<), `leq` (<=)
* **Keyword:** `program`, `var`, `const`, `type`, `function`, `procedure`, `begin`, `end`, `if`, `then`, `else`, `while`, `do`, `for`, `to`, `downto`, `repeat`, `until`, `case`, `of`, `array`, `record`
* **Delimiter:** `semicolon`, `colon`, `comma`, `period`, `lparent`, `rparent`, `lbrack`, `rbrack`
* **Assignment:** `becomes` (:=)
* **Identifier:** `ident` (case-insensitive)
* **Comment:** `comment` (tidak dioutput secara default)

### Implementasi DFA

Program menggunakan DFA yang didefinisikan dalam file `src/dfa_rules.txt` dengan format:

* `Start_state = <state>`: Mendefinisikan start state
* `Dead_state = <state>`: Mendefinisikan dead/trap state
* `Final_state = <state1>, <state2>, ...`: Mendefinisikan final states
* `<state> <input> <next_state>`: Mendefinisikan transisi

### Grammar yang Dapat dikenali
- **Program Structure:**
  `<program>`, `<program-header>`, `<declaration-part>`, `<block>`, `<compound-statement>`

- **Declarations:**
  `<const-declaration>`, `<type-declaration>`, `<var-declaration>`, `<subprogram-declaration>`, `<procedure-declaration>`, `<function-declaration>`

- **Type System:**
  `<type>`, `<array-type>`, `<range>`, `<enumerated>`, `<record-type>`, `<field-list>`, `<field-part>`

- **Variables & Parameters:**
  `<identifier-list>`, `<variable>`, `<component-variable>`, `<index-list>`, `<formal-parameter-list>`, `<parameter-group>`, `<parameter-list>`

- **Statements:**
  `<statement-list>`, `<statement>`, `<assignment-statement>`, `<if-statement>`, `<case-statement>`, `<case-block>`, `<while-statement>`, `<repeat-statement>`, `<for-statement>`, `<procedure/function-call>`

- **Expressions & Operators:**
  `<expression>`, `<simple-expression>`, `<term>`, `<factor>`, `<relational-operator>`, `<additive-operator>`, `<multiplicative-operator>`, `<constant>`

### AST Nodes yang Didefinisikan

- **Root:**
  `ProgramNode`

- **Declarations:**
  `ConstDeclNode`, `TypeDeclNode`, `VarDeclNode`, `ParamDeclNode`, `ProcDeclNode`, `FuncDeclNode`

- **Type System:**
  `SimpleTypeNode`, `ArrayTypeNode`, `RangeTypeNode`, `EnumTypeNode`, `FieldTypeNode`

- **Expressions:**
  `LiteralNode`, `BinaryOpNode`, `UnaryOpNode`, `VarRefNode`, `ArrayAccessNode`, `FieldAccessNode`, `CallNode`

- **Statements:**
  `AssignNode`, `IfNode`, `WhileNode`, `ForNode`, `RepeatNode`, `CaseNode`, `CompoundNode`, `CallStmtNode`

### SymbolTable

- **Tables:**
  `tab` (Tab[]), `atab` (ArrayTab[]), `btab` (BlockTab[]), `display` (int[])

- **State:**
  `level`, `predefinedCount`

### Tab
  `id`, `link`, `obj`, `type`, `ref`, `nrm`, `lev`, `adr`, `const_value`

### ArrayTab
  `arrays`, `xtyp`, `etyp`, `eref`, `low`, `high`, `elsz`, `size`

### BlockTab
  `blocks`, `last`, `lpar`, `psze`, `vsze`

### Reserved Words (initReserved)

- **Keywords:**
  `and`, `array`, `begin`, `case`, `const`, `div`, `downto`, `do`, `else`, `end`, `for`, `if`, `mod`, `not`, `of`, `or`, `program`, `record`, `repeat`, `then`, `to`, `type`, `until`, `var`, `while`

- **Built-in Types:**
  `integer`, `real`, `boolean`, `char`, `string`

- **Built-in Constants:**
  `true` (boolean, adr=1), `false` (boolean, adr=0)

- **Built-in Subprograms:**
  `function`, `procedure`, `readln`, `writeln`

## Intermediate Code Generator (ICG)

### Opcodes

* **Data Loading & Storage:**
  `LIT`, `LITS`, `LITR`, `LOD`, `STO`, `LODI`, `STOI`

* **Control Flow:**
  `JMP`, `JPC`, `CAL`, `RET`, `RETVAL`

* **Memory Management:**
  `INT`

* **Operation Dispatch:**
  `OPR`

### OPR Operations

* **Arithmetic Operations:**
  `NEG`, `ADD`, `SUB`, `MUL`, `DIV`, `IDIV`, `MOD`

* **Relational Operations:**
  `EQL`, `NEQ`, `LSS`, `LEQ`, `GTR`, `GEQ`

* **Input / Output Operations:**
  `WRT`, `WRTLN`

### Runtime Support

* **Activation Record Entries:**
  `Static Link`, `Dynamic Link`,
  `Return Address`, `Local Variables`,
  `Parameters`, `Function Return Value`

* **Literal Pools:**
  `String Table`, `Real Constant Table`

* **Structured Types:**
  `Array`, `Record`

## Pembagian Tugas
| NIM | Kontribusi Tugas | Persentase |
|:---:|:---:|:---:|
| 13524040 | Tokenizer, diagram DFA, production functions, debugging and testing, testcase input-output, Decorated AST, ICG, Interpreter | 25% |
| 13524042 | Penulisan laporan, DFA rules, diagram DFA, production functions, AST productions, Symbol Table, ICG | 25% |
| 13524048 | Class lexer, DFA, Parser, dan ParseTree, diagram DFA, production functions, AST definition, AST productions, Decorated AST Output, ICG, Interpreter | 25% |
| 13524088 | Penulisan laporan, DFA rules, diagram DFA, production functions, AST productions, ICG | 25% |