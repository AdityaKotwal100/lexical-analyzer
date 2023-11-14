/*
 * Modified by Aditya Ranjit Kotwal, 2023
 */

/*
 * Copyright (C) Rida Bazzi, 2019
 *
 * Do not share this file with anyone
 */
#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include <vector>
#include "lexer.h"
#include "reg.h"
#include "mylexicalanalyzer.h"
class Parser
{
public:
  void parse_input();
  void readAndPrintAllInput();

private:
  LexicalAnalyzer lexer;
  MyLexicalAnalyzer mylexer;
  void syntax_error();

  Token expect_expr(TokenType expected_type, std::string token_name);
  Token expect(TokenType expected_type);
  std::vector<TokenRegex> parse_tokens_section();
  void parse_token_list(std::vector<TokenRegex> &token_regex_list);
  TokenRegex parse_token();
  REG *parse_expr(std::string token);
  void syntax_error_in_expression(std::string token);
};

#endif
