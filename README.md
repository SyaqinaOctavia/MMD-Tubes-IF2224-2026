# MMD-Tubes-IF2224-2026
![source: emesan desu](mascot.jpg)

# Arion Compiler
Codebase ini merupakan implementasi Lexical Analyzer untuk bahasa pemrograman Arion sebagai bagian dari Tugas Besar mata kuliah IF2224 Teori Bahasa Formal dan Automata.

## Deskripsi Program

Program ini menggabungkan proses *Lexical Analyzer* (Lexer) dengan *Syntax Analysis* (Parser) dari sebuah *source code*. Proses pembentukan token dalam lexer menggunakan Deterministic Finite Automata (DFA) untuk mengenali pola karakter dan menghasilkan token sesuai dengan spesifikasi bahasa pemrograman Arion. Sementara proses analisa syntax dari token yang dihasilkan menggunakan algoritma Recursive Descent yang didasarkan pada Context Free Grammar (CFG)

## Identitas Kelompok

Kode Kelompok: **MMD**

Anggota:
* `13524040` - `Kloce Paul William Saragih`
* `13524042` - `Suryani Mulia Utami `
* `13524048` - `Josh Reinhart Zidik `
* `13524088` - `Syaqina Octavia Rizha `

## Struktur Proyek

```text
.
├── src/
│   ├── main.cpp
│   ├── lexer.cpp
│   ├── lexer.hpp
│   ├── DFA.cpp
│   ├── DFA.hpp
│   └── dfa_rules.txt
├── build/
│   └── milestone-1/
│       ├── input-1.txt
│       ├── output-1.txt
│       └── ...
├── doc/
│   └── Laporan-1-MMD.pdf
├── Makefile
└── README.md
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
make build
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

`option` digunakan untuk memilih antara *lexer* (`option == 1`) dan *parser* (`option == 2` untuk input file berisi token dan `option == 3` untuk input *source code*).

### Contoh Penggunaan

```bash
make run ARGS="1 test/input-1.txt test/output-1.txt"
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


## Pembagian Tugas
| NIM | Kontribusi Tugas | Persentase |
|:---:|:---:|:---:|
| 13524040 | Tokenizer, diagram DFA, production functions, debugging and testing, testcase input-output, | 25% |
| 13524042 | Penulisan laporan, DFA rules, diagram DFA, production functions | 25% |
| 13524048 | Class lexer, DFA, Parser, dan ParseTree, diagram DFA, production functions | 25% |
| 13524088 | Penulisan laporan, DFA rules, diagram DFA, production functions | 25% |