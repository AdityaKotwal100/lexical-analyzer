/*
 * Modified by Aditya Ranjit Kotwal, 2023
 */
#ifndef __MYLEXICALANALYZERH__
#define __MYLEXICALANALYZERH__

#include <string>
#include <vector>
#include "lexer.h"
#include <iostream>
#include "reg.h"
#include <algorithm>
#include <limits.h>
using namespace std;

struct MatchResult
{
    std::string token;
    std::string lexeme;
};

class MyLexicalAnalyzer
{
private:
    int p;

public:
    MyLexicalAnalyzer() : p(0) {}

    // Function to set the value of 'p'. This helps in keep track of how much of the string is processed.
    void update_p(int index)
    {
        p = index;
    }
    std::vector<REG_node *> find_epsilon_path(std::vector<REG_node *> &S)
    {
        // Function to find all nodes that can be reached by consuming in '_'
        std::vector<REG_node *> S_prime; 
        S_prime.push_back(S[0]);
        int i = 0;
        // Iterate through each node in S
        while (i < S_prime.size())
        {
            REG_node *n = S_prime[i];
            if (!n)
            {
                i++;
                continue;
            }
            if (n->first_label == '_' && n->first_neighbor && std::find(S_prime.begin(), S_prime.end(), n->first_neighbor) == S_prime.end())
            {
                S_prime.push_back(n->first_neighbor);
            }
            if (n->second_label == '_' && n->second_neighbor && std::find(S_prime.begin(), S_prime.end(), n->second_neighbor) == S_prime.end())
            {
                S_prime.push_back(n->second_neighbor);
            }
            i += 1;
        }

        return S_prime;
    }
    bool check_vector_equality(std::vector<REG_node *> vector_1, std::vector<REG_node *> vector_2)
    // Function to check if 2 vectors are equal
    {
        // If the size of the vectors are not same, then they are not equal
        if (vector_1.size() != vector_2.size())
        {
            return false;
        }

        // Sort both vectors to ensure elements are in the same order
        std::vector<REG_node *> sorted_vector_1 = vector_1;
        std::vector<REG_node *> sorted_vector_2 = vector_2;
        std::sort(sorted_vector_1.begin(), sorted_vector_1.end());
        std::sort(sorted_vector_2.begin(), sorted_vector_2.end());

        // Return match of vectors
        return sorted_vector_1 == sorted_vector_2;
    }

    // Function to match a single character in the corresponding REG Graph
    std::vector<REG_node *> Match_One_Char(std::vector<REG_node *> S, char c)
    {
        std::vector<REG_node *> S_prime;

        int i = 0;
        // First: Find all nodes that can be reached from current node by consuming character 'c'
        while (i < S.size())

        {

            REG_node *n = S[i];

            if (!n)
            {
                i += 1;
                continue;
            }
            // If first neighbor can be reached by only consuming character 'c' and this node is not processed, add it to the selection
            if (n->first_label == c && n && n->first_neighbor && std::find(S_prime.begin(), S_prime.end(), n->first_neighbor) == S_prime.end())
            {
                S_prime.push_back(n->first_neighbor);
            }
            // If second neighbor can be reached by only consuming character 'c' and this node is not processed, add it to the selection
            if (n->second_label == c && n && n->second_neighbor && std::find(S_prime.begin(), S_prime.end(), n->first_neighbor) == S_prime.end())
            {
                S_prime.push_back(n->second_neighbor);
            }
            i += 1;
        }

        // Return an empty set if there are no nodes that can be reached by consuming character 'c
        if (S_prime.empty())
        {
            std::vector<REG_node *> ret;
            return ret;
        }

        bool changed = true;
        // Now from the current set of nodes, find all nodes that can be reached by consuming '_'
        std::vector<REG_node *> S_double_prime;
        while (changed)
        {
            changed = false;
            int i = 0;
            while (i < S_prime.size())
            {
                REG_node *n = S_prime[i];
                if (!n)
                {
                    i += 1;
                    continue;
                }
                if (std::find(S_double_prime.begin(), S_double_prime.end(), n) == S_double_prime.end())
                {
                    S_double_prime.push_back(n);
                }
                // If first neighbor can be reached by only consuming '_' and this node is not processed, add it to the selection
                if (n->first_neighbor && n->first_label == '_' && std::find(S_double_prime.begin(), S_double_prime.end(), n->first_neighbor) == S_double_prime.end())
                {
                    S_double_prime.push_back(n->first_neighbor);
                    changed = true;
                }
                // If second neighbor can be reached by only consuming '_' and this node is not processed, add it to the selection
                if (n->second_neighbor && n->second_label == '_' && std::find(S_double_prime.begin(), S_double_prime.end(), n->second_neighbor) == S_double_prime.end())
                {
                    S_double_prime.push_back(n->second_neighbor);
                    changed = true;
                }
                i += 1;
            }
            // If there are any new nodes added since last iteration,
            // we repeat the process with the new nodes added
            if (!check_vector_equality(S_prime, S_double_prime))
            {
                changed = true;
                S_prime = S_double_prime;
                S_double_prime.clear();
            }
        }
        return S_prime;
    }

    // Function that finds the longest matching substring from
    // position 'p' given Regex r
    int match(REG *r, std::string s, int p)
    {
     

        // Add start node of regex as starting point
        std::vector<REG_node *> epsilon_path;
        epsilon_path.push_back(r->start);
        REG_node *accepting_node = r->accept;
        // Find all nodes that can be reached by consuming only '_'
        epsilon_path = find_epsilon_path(epsilon_path);

        // If the inital epsilon path contains the acception node,
        // it means that this can cause epsilon string
        if (std::find(epsilon_path.begin(), epsilon_path.end(), accepting_node) != epsilon_path.end())
            return -1;
        int longest_so_far = INT_MIN;
        std::vector<REG_node *> path;

        // For every character in the string starting from position 'p',
        // find if it has a corresponding regex path
        while (p < s.length())
        {
            path = Match_One_Char(epsilon_path, s[p]);
            if (!path.size())
            {
                return longest_so_far;
            }
            // Find the longest matching string
            if (std::find(path.begin(), path.end(), accepting_node) != path.end())
            {
                longest_so_far = max(longest_so_far, p);
            }
            epsilon_path = path;
            p++;
        }

        return longest_so_far;
    }

    // Function that processes a string and splits it into tokens that match the strings
    // according to their corresponding regex pattern
    std::vector<MatchResult> my_GetToken(std::vector<TokenRegex> &token_list, std::string s)
    {
        

        std::vector<MatchResult> result;

        bool found_one = true;
        // Continue till either string is fully consumed
        // or atleast one match is found
        while (p != s.length() && found_one)
        {

            found_one = false;
            int token_list_cursor = 0;
            MatchResult each_match;
            int longest_so_far = INT_MIN;
            int current_match;
            while (token_list_cursor < token_list.size())
            {


                // Get the longest match for string s starting from position 'p' given
                // the regex
                current_match = match(token_list[token_list_cursor].regex, s, p);

                // If there is no match, try with the next regex and reset the current posiiton of 'p'
                if (current_match == INT_MIN)
                {

                    token_list_cursor += 1;
                    update_p(p); // reset to original p
                    continue;
                }
                // If entire string has been processed, return the matching token as it is the longest and first match
                else if (current_match == s.length() - 1)
                {
                    found_one = true;
                    each_match.lexeme = s.substr(p, (current_match + 1) - p);
                    each_match.token = token_list[token_list_cursor].token;
                    result.push_back(each_match);

                    return result;
                }
                // If the entire string has not been processed yet,
                // check if the current regex has the longest match so far,
                // if not, continue else 
                // store the longest match

                else if (current_match != s.length())
                {
                    if (longest_so_far < current_match)
                    {
                        found_one = true;
                        longest_so_far = current_match;
                        each_match.lexeme = s.substr(p, (current_match + 1) - p);
                        each_match.token = token_list[token_list_cursor].token;
                    }
                    token_list_cursor += 1;
                }
            }
            result.push_back(each_match);

            // Update 'p' with the next index to process
            update_p(longest_so_far + 1);
        }

        return result;
    }
};

#endif