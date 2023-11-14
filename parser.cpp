/*
 * Modified by Aditya Ranjit Kotwal, 2023
 */

/*
 * Copyright (C) Rida Bazzi, 2020
 *
 * Do not share this file with anyone
 *
 * Do not post this file or derivatives of
 * of this file online
 *
 */
#include <iostream>
#include <cstdlib>
#include <string>
#include "parser.h"
#include <vector>
#include <unordered_set>
#include <map>
#include "reg.h"
#include "stringmatch.h"
#include "mylexicalanalyzer.h"

using namespace std;

// this syntax error function needs to be
// modified to produce the appropriate message
void Parser::syntax_error()
{
    cout << "SNYTAX ERORR\n";
    exit(1);
}

// Used when Syntax error is found inside parse_expr() function
void Parser::syntax_error_in_expression(std::string token)
{
    cout << "SYNTAX ERROR IN EXPRESSION OF " + token << endl;
    exit(1);
}

// this function gets a token and checks if it is
// of the expected type. If it is, the token is
// returned, otherwise, synatx_error() is generated
// this function is particularly useful to match
// terminals in a right hand side of a rule.
// Written by Mohsen Zohrevandi
Token Parser::expect(TokenType expected_type)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error();
    return t;
}

// this syntax error function is called inside the parse_expression() function
// to catch any errors in the regex expression
Token Parser::expect_expr(TokenType expected_type, std::string token_name)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error_in_expression(token_name);
    return t;
}

// This function parses the entire expression according to the provided grammar rules
// expr = (expr).(expr)
// expr = (expr)|(expr)
// expr = (expr)*
// expr = CHAR
// expr = _
REG *Parser::parse_expr(std::string token)
{

    Token t = lexer.peek(1);
    if (t.token_type == CHAR)
    {
        // Construct regex graph for character
        expect_expr(CHAR, token);
        RegexGenerator r;
        REG *R1 = r.parse_character(t.lexeme);
        return R1;
    }
    else if (t.token_type == UNDERSCORE)
    {
        // Construct regex graph for underscore
        expect_expr(UNDERSCORE, token);
        RegexGenerator r;
        REG *R1 = r.parse_underscore();
        return R1;
    }
    else if (t.token_type == LPAREN)

    {
        // If LPAREN is met, consume it and recurse for creating regex of next expression
        expect_expr(LPAREN, token);

        REG *R1 = parse_expr(token);

        t = expect_expr(RPAREN, token);
        t = lexer.peek(1);
        if (t.token_type == DOT)
        {
            // Construct regex graph for DOR operator
            expect_expr(DOT, token);
            expect_expr(LPAREN, token);
            REG *R2 = parse_expr(token);
            expect_expr(RPAREN, token);
            R2->accept->first_label = '\0';
            R1->accept->first_neighbor = R2->start;
            R1->accept->first_label = '_';
            R1->accept->second_neighbor = nullptr;
            R1->accept->second_label = '\0';
            R1->accept = R2->accept;
            return R1;
        }

        else if (t.token_type == OR)
        {
            // Construct regex graph for OR operator
            expect_expr(OR, token);
            expect_expr(LPAREN, token);
            REG *R2 = parse_expr(token);
            expect_expr(RPAREN, token);
            REG_node *new_start_node = new REG_node;
            REG_node *new_accept_node = new REG_node;
            new_start_node->first_label = '_';
            new_start_node->second_label = '_';
            new_start_node->first_neighbor = R1->start;
            new_start_node->second_neighbor = R2->start;

            new_accept_node->first_label = '\0';
            new_accept_node->second_label = '\0';
            new_accept_node->first_neighbor = nullptr;
            new_accept_node->second_neighbor = nullptr;

            R1->accept->first_neighbor = new_accept_node;
            R1->accept->first_label = '_';
            R2->accept->first_neighbor = new_accept_node;
            R2->accept->first_label = '_';

            R1->start = new_start_node;
            R1->accept = new_accept_node;
            return R1;
        }
        else if (t.token_type == STAR)
        {
            // Construct regex graph for Kleene star operator
            expect_expr(STAR, token);
            REG_node *new_star_node_1 = new REG_node;
            REG_node *new_star_node_2 = new REG_node;
            new_star_node_1->first_label = '_';
            new_star_node_1->second_label = '_';
            new_star_node_1->first_neighbor = R1->start;
            new_star_node_1->second_neighbor = new_star_node_2;

            new_star_node_2->first_label = '_';
            new_star_node_2->second_label = '\0';
            new_star_node_2->first_neighbor = nullptr;
            new_star_node_2->second_neighbor = nullptr;

            R1->accept->second_neighbor = R1->start;
            R1->accept->second_label = '_';
            R1->accept->first_neighbor = new_star_node_2;
            R1->accept->first_label = '_';

            R1->start = new_star_node_1;
            R1->accept = new_star_node_2;
            return R1;
        }
        else
            syntax_error_in_expression(token);
    }

    else
        syntax_error_in_expression(token);
}

TokenRegex Parser::parse_token()
{
    // Parse a single token and construct the regex for that token

    Token t;
    t = expect(ID);

    TokenRegex token_regex;

    token_regex.token = t.lexeme;
    token_regex.line = t.line_no;
    REG *regex_graph = parse_expr(token_regex.token);
    if (!regex_graph)
        syntax_error_in_expression(token_regex.token);
    token_regex.regex = regex_graph;
    return token_regex;
}

// Function to parse token list
void Parser::parse_token_list(std::vector<TokenRegex> &token_regex_list)
{
    // Token list is of the form
    // tok1 a, tok2 (a)|(b) etc...

    // Get current token and generate it's regex graph
    TokenRegex current_token_regex = parse_token();
    token_regex_list.push_back(current_token_regex);
    Token t;
    t = lexer.peek(1);
    // If the next token is comma, we recurse by consuming comma to process
    // the next token in the same way

    if (t.token_type == COMMA)
    {
        expect(COMMA);
        parse_token_list(token_regex_list);
    }
}

std::vector<TokenRegex> Parser::parse_tokens_section()
{
    // Parse the token section, it ends with a hash
    std::vector<TokenRegex> tokens_regex;
    parse_token_list(tokens_regex);
    expect(HASH);
    return tokens_regex;
}

// Function that helps print any Epsilon is not a token messages
void epsilon_not_a_token(std::vector<std::string> epsilon_found_tokens)
{
    cout << "EPSILON IS NOOOOOOOT A TOKEN !!! ";
    // Iterate through all the tokens that have an epsilon path
    for (auto it : epsilon_found_tokens)
    {
        cout << it << " ";
    }
    cout << endl;
    exit(1);
}

void Parser::parse_input()
{
    std::vector<TokenRegex> regex_tokens = parse_tokens_section();
    Token t = lexer.peek(1);

    expect(INPUT_TEXT);

    StringMatcher matcher;
    // Format input string by removing leading and trailing whitespaces and splitting by comma
    std::vector<std::string> string_vector = matcher.format_input(t.lexeme);
    std::vector<MatchResult> matches;
    std::vector<MatchResult> result;
    

    expect(END_OF_FILE);

    // After parsing the entire input, we check for semantic erros and epsilon path errors

    std::map<std::string, int> exists;
    bool semantic = false;
    bool epsilon_found = false;
    std::vector<std::string> epsilon_found_tokens;

    // Check if the generated Regex has an epsilon path
    for (auto each_regex : regex_tokens)
    {
        if (matcher.check_if_epsilon_path(each_regex.regex, each_regex.regex->accept))
        {
            epsilon_found = true;
            epsilon_found_tokens.push_back(each_regex.token);
        }
    }

    if (epsilon_found)
        epsilon_not_a_token(epsilon_found_tokens);

    // Check if the given input has any semantic errors
    for (auto it : regex_tokens)
    {
        if (exists.count(it.token) > 0)
        {
            semantic = true;
            cout << "Line " << it.line << ": " << it.token << " already declared on line " << exists[it.token] << endl;
        }
        else
            exists[it.token] = it.line;
    }
    if (semantic)
        exit(1);

    int st = 0;
    // For every string that is generated, find the appropriate matching tokens
    while (st < string_vector.size())
    {
        std::string selected_string = string_vector[st++];
        result = mylexer.my_GetToken(regex_tokens, selected_string);
        // Once a match is made, we reset the position of p to the 0th index again as the
        // current string is done processing and we move on to the next one
        mylexer.update_p(0);
        matches.insert(matches.end(), result.begin(), result.end());
    }

    // Print all the stored results
    for (int i = 0; i < matches.size(); i++)
    {
        if (matches[i].lexeme.length())
            std::cout << matches[i].token << ", " << '"' << matches[i].lexeme << '"' << endl;
        else
        {
            cout << "ERROR" << endl;
            exit(1);
        }
    }
}

// This function simply reads and prints all tokens
// I included it as an example. You should compile the provided code
// as it is and then run ./a.out < tests/test0.txt to see what this function does
// This function is not needed for your solution and it is only provided to
// illustrate the basic functionality of getToken() and the Token type.

void Parser::readAndPrintAllInput()
{
    Token t;

    // get a token
    t = lexer.GetToken();

    // while end of input is not reached
    while (t.token_type != END_OF_FILE)
    {
        t.Print();            // pringt token
        t = lexer.GetToken(); // and get another one
    }

    // note that you should use END_OF_FILE and not EOF
}

int main()
{
    // note: the parser class has a lexer object instantiated in it (see file
    // parser.h). You should not be declaring a separate lexer object.
    // You can access the lexer object in the parser functions as shown in
    // the example  method Parser::readAndPrintAllInput()
    // If you declare another lexer object, lexical analysis will
    // not work correctly
    Parser parser;

    parser.parse_input();
}
