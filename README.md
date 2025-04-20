# 24-Bit Assembler in C 🧠⚙️

**Final Project - Computer Science (Open University)**  
Course: 20465 - Systems Programming in C (Assembler Project)

---

## 🔍 Project Overview

This project is an assembler implemented in ANSI C for a custom 24-bit instruction set architecture (ISA), created as part of a final course project. The assembler processes an assembly source file through multiple stages, resolving macros, labels, and symbols, and producing final machine code output in a custom format.

The main purpose of this project is to simulate a basic assembler for educational purposes, with modular components and careful memory handling.

This project was developed by **Dvir Haham Pour** and **Yarin Lemel**.

---

## 🚦 Stages Overview

### ✅ Pre-processing
- **Macro Management:** Identifies and replaces macros in the source.
- **Error Detection:** Catches incorrect macro definitions or usage.
- **Single Pass Macro Resolution:** All macros are processed before compilation begins.

### 🌀 First Pass
- **Symbol Table Creation:** Builds a table of symbols (labels) and tracks addresses.
- **Instruction Parsing:** Parses the source code to collect label addresses and instruction sizes.
- **Error Handling:** Detects undefined symbols and incorrect syntax.

### 📘 Second Pass
- **Label Resolution:** Resolves addresses of all previously defined labels.
- **Final Code Generation:** Produces machine code output, with all addresses and operands finalized.

---

## ⚠️ Error Handling

The assembler identifies syntax errors, undefined labels, and macro issues at various stages. If an error is detected in early passes, compilation is halted to ensure data consistency.

---

## 💾 Memory Management

Dynamic memory allocation is used for flexible data structure handling (symbol tables, instruction lists, etc.). Static/global variables are avoided to ensure modularity and reusability.

---

## 📂 Files

- `main.c` – Entry point
- `first_pass.c/h` – Logic for the first pass
- `second_pass.c/h` – Logic for the second pass
- `symbol_list.c/h` – Symbol table implementation
- `utils.c/h` – Helper functions

---

## 👨‍💻 Authors

- **Dvir Haham Pour**  
- **Yarin Lemel**

Assembler Project – 2025

---

## 📎 Notes

This project was written and compiled in a Unix/Linux environment using `gcc`.  
You can compile the project using:

```bash
gcc -Wall -ansi -pedantic *.c -o assembler
