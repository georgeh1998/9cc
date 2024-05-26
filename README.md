# georgecc: A simple, limited, self-hosting C compiler

Welcome to georgecc! This project is a self-hosting compiler for a subset of the C programming language. It was developed as a toy project with the primary goal of deepening my understanding of compiler construction. georgecc is minimalistic and supports only the essential features needed to make it capable of self-hosting, meaning it can compile its own source code.

# Feature

- Supported Data Types:
  - The compiler supports basic data types including integers (int), characters (char), arrays, and pointers.
- Operations: 
  - Supports arithmetic operations, parentheses for controlling operation precedence, and common comparison operators like == and >=.
- Control Structures: 
  - Basic syntax for control flow such as while, if, for, and return statements are supported. 



# Limitation

- Restricted Syntax: 
  - Features not mentioned above, such as switch statements, are not supported.
- Single File Support: 
  - The compiler can only process one input file at a time.
- No Linker: 
  - georgecc does not include linker functionality. If standard library calls or external functions are needed, the output assembly will need to be linked using another tool like cc.
 

# Getting Started
To use georgecc, follow these steps:

```
git clone https://github.com/georgeh1998/georgecc
cd georgecc
make
./georgecc "path_to_c_file_you_want_to_compile" > assemble.s
```

This will compile the specified C file using georgecc. Remember, the output will be in assembly, which can then be linked manually if needed using a standard C compiler like gcc to create an executable.


# Running Tests
To verify that georgecc correctly compiles the C files located in the test folder, you can use the provided test suite. Simply run the following command from the project's root directory:

```
make test
```
This command will compile the test C files and check for correctness, ensuring that the compiler functions as expected.


# Thaks
The development of georgecc was inspired by and based on the online resource [低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook) by Rui Ueyama san. This resource is highly recommended for anyone interested in the details of low-level programming and compiler construction.
