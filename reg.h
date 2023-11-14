/*
 * Modified by Aditya Ranjit Kotwal, 2023
 */

#ifndef __REGEX_H__
#define __REGEX_H__

#include <string>
#include <vector>
#include "lexer.h"

struct REG_node
{
    struct REG_node *first_neighbor;
    char first_label;
    struct REG_node *second_neighbor;
    char second_label;
};

struct REG
{
    struct REG_node *start;
    struct REG_node *accept;
};

struct TokenRegex
{
    std::string token;
    REG *regex;
    int line;
};
class RegexGenerator
{
public:
    REG *parse_underscore()
    {
        //Constructs the REG Graph for epsilon
        REG_node *new_first = new REG_node;
        REG_node *new_second = new REG_node;

        new_first->first_neighbor = new_second;
        new_first->first_label = '_';
        new_first->second_neighbor = nullptr;
        new_first->second_label = '\0';

        new_second->first_neighbor = nullptr;
        new_second->first_label = '_';
        new_second->second_neighbor = nullptr;
        new_second->second_label = '\0';

        REG *new_reg = new REG;
        new_reg->start = new_first;
        new_reg->accept = new_second;
        return new_reg;
    }
    REG *parse_character(std::string expression)
    {
        //Constructs the REG Graph for a character
        REG_node *new_first = new REG_node;
        REG_node *new_second = new REG_node;

        new_first->first_neighbor = new_second;
        new_first->first_label = expression[0];
        new_first->second_neighbor = nullptr;
        new_first->second_label = '\0';

        new_second->first_neighbor = nullptr;
        new_second->first_label = '_';
        new_second->second_neighbor = nullptr;
        new_second->second_label = '\0';

        REG *new_reg = new REG;
        new_reg->start = new_first;
        new_reg->accept = new_second;
        return new_reg;
    }
};

#endif