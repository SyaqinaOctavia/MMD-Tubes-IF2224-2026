# MMD-Tubes-IF2224-2026
![source: emesan desu](mascot.jpg)

# Arion Compiler
Codebase ini merupakan implementasi Lexical Analyzer untuk bahasa pemrograman Arion sebagai bagian dari Tugas Besar mata kuliah IF2224 Teori Bahasa Formal dan Automata.

## Deskripsi Program

Program ini menggunakan Deterministic Finite Automata (DFA) untuk mengenali pola karakter dan menghasilkan token sesuai dengan spesifikasi bahasa pemrograman Arion. Program ini menggunakan Lexical Analyzer (Lexer) sebagai tahap pertama dalam kompilasi source code dari rangkaian karakter menjadi program yang dapat dijalankan. 

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
```

Program akan dikompilasi dan menghasilkan *executable* bernama `main`.

### Membersihkan Build Files

```bash
make clean
```

## Cara Penggunaan

### Format Perintah

```bash
make run ARGS="<intput-file> <output-file>"
```

### Contoh Penggunaan

```bash
make run ARGS="test/input-1.txt test/output-1.txt"
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

Contoh output:

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

## Pembagian Tugas
| NIM | Kontribusi Tugas | Persentase |
|:---:|:---:|:---:|
| 13524040 | Tokenizer, Testing program, Testcase input-output, diagram DFA | 25% |
| 13524042 | Penulisan laporan, DFA rules, diagram DFA | 25% |
| 13524048 | Class lexer, class DFA, diagram DFA | 25% |
| 13524088 | Penulisan laporan, DFA rules, Diagram DFA | 25% |