# Lexical Analyzer Generator

## Overview
This project provides an automatic lexical analyzer generator that processes lists of tokens defined by regular expressions. It is designed to facilitate the parsing and tokenization of strings according to user-defined patterns, making it a versatile tool for compiler construction, data parsing, and other applications requiring lexical analysis.

## Features
- **Token Definition Parsing**: Parse a list of token definitions provided in a specific format, including token names and their corresponding regular expressions.
- **Input String Lexical Analysis**: Perform lexical analysis on a given input string, breaking it down into a sequence of token-lexeme pairs based on the provided token definitions.
- **Error Handling**: Identify and report various types of errors, including syntax errors in the input, duplicate token names, and regular expressions that generate the empty string.

## Input Format
The input to the program consists of two parts:
1. A list of token definitions, each comprising a token name and a token description (regular expression), separated by commas and terminated with a hash (`#`) symbol.
2. An input string composed of letters, digits, and space characters.

Example:
```token1 reg_exp1, token2 reg_exp2, ... tokenN reg_expN #```



## Output Format
The program outputs one of the following based on the input:
- A sequence of tokens and their corresponding lexemes if the input is correctly formatted and matches the token definitions.
- An error message if there are syntax errors, duplicate token names, or if a token's regular expression can generate the empty string.

## Requirements
- Do not use any built-in or library support for regular expressions in C/C++.
- Ensure the program compiles and runs on Ubuntu 22.04 with GCC version 4:11.2.0-1ubuntu1, using the `-std=c++11` option.

## How to Compile and Run
Compile the program using GCC with C++11 support:
```bash
g++ -std=c++11 your_program.cpp -o lexer
```

Run the program and redirect input from a file:

```./lexer < input_file.txt```



