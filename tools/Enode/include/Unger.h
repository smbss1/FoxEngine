/*
** EPITECH PROJECT, 2019
** CodeGenerator
** File description:
** Created by besseausamuel
*/
#ifndef CODEGENERATOR_UNGER_H
#define CODEGENERATOR_UNGER_H

#include <vector>
#include <iterator>
#include <iostream>
#include <iterator>

#include "Grammar.h"


namespace MB
{

    namespace detail
    {
// First partition of sequence into k parts
        void first_sequence_partition(unsigned int *arr, unsigned int k)
        {
            unsigned int p;
            for (p = 0; p < k - 1; p++) {
                arr[p] = p + 1;
            }
        }

// Next partition of sequence into k parts
        bool next_sequence_partition(unsigned int *arr, size_t len, unsigned int k)
        {
            int p; // Separator number
            unsigned int s; // Its highest place
            bool changed = 0;
            for (p = k - 2, s = len - 1; p >= 0 && !changed; p--, s--) {
                if (arr[p] < s) {
                    arr[p]++;
                    changed = true;
                    if (static_cast<unsigned int>(p) < k - 2) {
                        p++;
                        while (static_cast<unsigned int>(p) <= k - 2) {
                            arr[p] = arr[p - 1] + 1;
                            p++;
                        }
                    }
                }
            }
            if (!changed) {
                first_sequence_partition(arr, k);
            }
            return changed;
        }

// Fill a vector with len elements starting at start
        void fill(const std::vector<std::string>& input, unsigned int start, unsigned int len,
                  std::vector<std::string>& output)
        {
            unsigned int i, j;
            for (i = start, j = 0; j < len; ++i, ++j) {
                output.push_back(input[i]);
            }
        }

    } // namespace detail

    class unger_parser
    {
    public:
        explicit unger_parser(const MB::Grammar& grammar)
            : grammar_(grammar), os_(nullptr)
        {
        }

        template <class InputIt>
        bool parse(InputIt begin, InputIt end)
        {
            return parse(begin, end, nullptr);
        }

        template <class InputIt>
        bool parse(InputIt begin, InputIt end, std::ostream& os)
        {
            os_ = &os;
            bool success = parse(begin, end, &os);
            os_ = nullptr;
            return success;
        }

    private:
        template <class InputIt>
        bool parse(InputIt begin, InputIt end, std::ostream *os)
        {
            std::vector<std::string> input;
            std::copy(begin, end, std::back_inserter(input));
            std::vector<MB::rule::const_ptr> rules;
            grammar_.get_start_rules(std::back_inserter(rules));
            return valid_rules(input, rules);
        }

        // If symbol is a single terminal, it needs to match cell
        bool valid_cell_terminal(const std::vector<std::string>& cell,
                                 const std::string& symbol)
        {
            return cell.size() == 1 && cell[0] == symbol;
        }

        // If symbol is a non-terminal, find its rules and try to match one of them
        bool valid_cell_nonterminal(const std::vector<std::string>& cell,
                                    const std::string& symbol)
        {
            // Try all of the rules where symbol is the left hand side
            std::vector<MB::rule::const_ptr> rules;
            grammar_.get_rules_for_left(symbol, std::back_inserter(rules));
            return valid_rules(cell, rules);
        }

        // A cell is valid if it matches a terminal or a rule for a non-terminal succeeds
        bool valid_cell(const std::vector<std::string>& cell,
                        const std::string& symbol)
        {
            bool terminal = grammar_.symbol_is_terminal(symbol);
            return (terminal && valid_cell_terminal(cell, symbol))
                   || (!terminal && valid_cell_nonterminal(cell, symbol));
        }

        // All of the cells in a row need to succeed
        bool valid_row(const std::vector<std::string>& input,
                       const std::vector<std::string>& right, const unsigned int *arr, unsigned int k)
        {
            const size_t len = input.size();
            bool success = true;
            std::vector<std::vector<std::string> > cells;
            for (unsigned int p = 0; p < k && success; p++) {
                std::vector<std::string> cell;
                if (p == 0) {
                    // First partition goes from the beginning to arr[0]
                    detail::fill(input, 0, arr[0], cell);
                }
                else if (p == k - 1) {
                    // Last partition goes from arr[k - 2] to the end
                    detail::fill(input, arr[k - 2], len - arr[k - 2], cell);
                }
                else {
                    // Partition goes from separator p - 1 to separator p
                    detail::fill(input, arr[p - 1], arr[p] - arr[p - 1], cell);
                }
                cells.push_back(cell);
                std::string symbol = right[p];
                if (grammar_.symbol_is_terminal(symbol)) {
                    success = valid_cell_terminal(cell, symbol);
                }
            }
            if (success) {
                for (unsigned int p = 0; p < k && success; p++) {
                    std::string symbol = right[p];
                    success = valid_cell(cells[p], symbol);
                }
            }
            return success;
        }

        // One of the rows in a table needs to succeed
        bool valid_table(const std::vector<std::string>& input,
                         const std::vector<std::string>& right)
        {
            const unsigned int k = right.size(); // Number of parts
            const size_t len = input.size(); // Number of tokens
            bool success = false;
            if (k <= len) {
                if (k > 1) {
                    std::vector<unsigned int> arr(k);
                    detail::first_sequence_partition(arr.data(), k);
                    do {
                        success = valid_row(input, right, arr.data(), k);
                    } while (detail::next_sequence_partition(arr.data(), len, k) && !success);
                }
                else {
                    success = valid_cell(input, right[0]);
                }
            }
            return success;
        }
        // One of the right hand side alternatives of one of the rules needs to succeed
        bool valid_rules(const std::vector<std::string>& input,
                         const std::vector<MB::rule::const_ptr>& rules)
        {
            for (MB::rule::const_ptr r : rules) {
                for (const std::vector<std::string>& alternative : r->right()) {
                    if (valid_table(input, alternative)) {
                        if (os_) {
                            *os_ << "Succeeded in matching rule ";
                            *os_ << *r;
                            *os_ << " with input ";
                            std::copy(input.begin(), input.end(),
                                      std::ostream_iterator<std::string>(*os_, " "));
                            *os_ << '\n';
                        }
                        return true;
                    }
                }
            }
            return false;
        }

    private:
        const MB::Grammar& grammar_;
        std::ostream* os_;
    };

} // namespace MB

#endif //CODEGENERATOR_UNGER_H
