/*
 * Copyright (C) Rida Bazzi, 2019
 *
 * Do not share this file with anyone
 */
#ifndef __LEXER__H__
#define __LEXER__H__

#include <vector>
#include <string>

#include "inputbuf.h"

// ------- token types -------------------

typedef enum { END_OF_FILE = 0,
    LPAREN, RPAREN, HASH, ID, COMMA, DOT, STAR,
    OR, UNDERSCORE, SYMBOL, CHAR, INPUT_TEXT, ERROR
    } TokenType;

class Token {
  public:
    void Print();

    std::string lexeme;
    TokenType token_type;
    int line_no;
};

class LexicalAnalyzer {
  public:
    Token GetToken();
    Token peek(int);
    LexicalAnalyzer();

  private:
    std::vector<Token> tokenList;
    Token GetTokenMain();
    int line_no;
    int index;
    Token tmp;
    InputBuffer input;

    bool SkipSpace();
    Token ScanIdOrChar();
    Token ScanSymbol();
    Token ScanInput();
};

#endif  //__LEXER__H__
