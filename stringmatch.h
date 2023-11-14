/*
 * Modified by Aditya Ranjit Kotwal, 2023
 */

#ifndef __STRINGMATCHH__
#define __STRINGMATCHH__

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include "lexer.h"
#include "reg.h"
#include <sstream>
#include <unordered_map>
#include <algorithm>
using namespace std;

class StringMatcher
{
public:
    // Function to check if a REG graph generates epsilon
    bool check_if_epsilon_path(REG *regex_token, REG_node *accepting_node)
    {
        vector<REG_node *> result;
        vector<REG_node *> epsilon_path;
        result.push_back(regex_token->start);

        // Finds all nodes in the REG Graph that can be reached by only consuming '_'
        epsilon_path = find_epsilon_path(result);

        // Returns true if accepting node is in the REG path that consumes only '_' else false
        return (std::find(epsilon_path.begin(), epsilon_path.end(), accepting_node) != epsilon_path.end());
    }

    // Function that removes leading and trailing whitespaces and splits input string by seperators
    std::vector<std::string> format_input(std::string input_string)
    {
        std::vector<std::string> result;
        std::replace(input_string.begin(), input_string.end(), '\"', ' '); // Replace double quotes with spaces

        std::istringstream iss(input_string);
        std::string token;

        while (iss >> token)
        {
            result.push_back(token);
        }

        // Iterate through the vector and remove empty strings
        for (auto it = result.begin(); it != result.end();)
        {
            if (it->empty())
            {
                it = result.erase(it); // Erase the empty string and advance the iterator
            }
            else
            {
                ++it; // Move to the next element
            }
        }
        return result;
    }

    // Function that returns all nodes in the REG Graph that can be reached by only consuming '_'
    vector<REG_node *> find_epsilon_path(vector<REG_node *> &S)
    {
        vector<REG_node *> S_prime;
        S_prime.push_back(S[0]);
        int i = 0;
        while (i < S_prime.size())
        {
            REG_node *n = S_prime[i];
            if (!n)
            {
                i++;
                continue;
            }
            // If first neighbor can be reached by only consuming '_' and this node is not processed, add it to the selection
            if (n->first_label == '_' && n->first_neighbor && std::find(S_prime.begin(), S_prime.end(), n->first_neighbor) == S_prime.end())
            {
                S_prime.push_back(n->first_neighbor);
            }
            // If second neighbor can be reached by only consuming '_' and this node is not processed, add it to the selection
            if (n->second_label == '_' && n->second_neighbor && std::find(S_prime.begin(), S_prime.end(), n->second_neighbor) == S_prime.end())
            {
                S_prime.push_back(n->second_neighbor);
            }
            i += 1;
        }

        return S_prime;
    }
}

;

#endif
