/*
** EPITECH PROJECT, 2019
** CodeGenerator
** File description:
** Created by besseausamuel
*/
#ifndef CODEGENERATOR_GRAMMAR_H
#define CODEGENERATOR_GRAMMAR_H

#include <string>
#include <vector>
#include <fstream>
#include <iterator>
#include <list>
#include <sstream>
#include <set>
#include <algorithm>
#include <utility>
#include <memory>
#include <iostream>

namespace MB
{

    namespace detail
    {

// Form the cartesian product of two sequences
        template <class InputIt1, class InputIt2, class OutputIt>
        OutputIt cartesian_product(InputIt1 begin1, InputIt1 end1, InputIt2 begin2, InputIt2 end2, OutputIt it)
        {
            for (InputIt1 it1 = begin1; it1 != end1; ++it1) {
                for (InputIt2 it2 = begin2; it2 != end2; ++it2) {
                    *it++ = std::make_pair(*it1, *it2);
                }
            }
            return it;
        }

// Find out if container contains all of range of items
        template <class AssociativeContainer, class InputIt>
        bool find_all(const AssociativeContainer& container, InputIt begin, InputIt end)
        {
            bool all = true;
            for (InputIt it = begin; it != end && all; ++it) {
                all = container.find(*it) != container.end();
            }
            return all;
        }

    } // namespace detail

    class rule
    {
    public:
        typedef std::shared_ptr<rule> ptr;
        typedef std::shared_ptr<const rule> const_ptr;
        rule(const rule&) = delete;
        rule& operator=(const rule&) = delete;
        explicit operator bool()
        {
            return succeeded_;
        }
        size_t size(unsigned int right) const
        {
            return right_[right].size();
        }
    private:
        std::string left_;
        std::vector<std::vector<std::string> > right_;
        bool succeeded_;
        rule(const std::string& left, const std::string& right)
            : left_(left)
        {
            right_.push_back(std::vector<std::string>(1, right));
        }
        explicit rule(const std::string &str)
        {
            unsigned int i = 0;
            while (i < str.size() && std::isspace(str[i])) {
                ++i;
            }
            // Get left-hand side
            while (i < str.size() && std::isalnum(str[i])) {
                left_.push_back(str[i]);
                ++i;
            }
            if (!(succeeded_ = !left_.empty())) {
                return;
            }
            while (i < str.size() && std::isspace(str[i])) {
                ++i;
            }
            // Get arrow
            if (!(succeeded_ = str.substr(i, 2) == "->")) {
                return;
            }
            i += 2;
            while (i < str.size() && std::isspace(str[i])) {
                ++i;
            }
            // Get right-hand side
            while (i < str.size()) {
                bool added = false;
                while (i < str.size() && str[i] != '|') {
                    std::string symbol;
                    while (i < str.size() && !std::isspace(str[i]) && str[i] != '|') {
                        symbol.push_back(str[i]);
                        ++i;
                    }
                    if (!symbol.empty()) {
                        if (!added) {
                            right_.push_back(std::vector<std::string>());
                            added = true;
                        }
                        right_.back().push_back(symbol);
                    }
                    while (i < str.size() && std::isspace(str[i])) {
                        ++i;
                    }
                }
                if (i < str.size()) {
                    ++i;
                }
            }
        }
    public:
        static ptr create(const std::string& left, const std::string& right)
        {
            return ptr(new rule(left, right));
        }
        static ptr create(const std::string& line)
        {
            return ptr(new rule(line));
        }
        const std::string& left() const
        {
            return left_;
        }
        const std::vector<std::vector<std::string> >& right() const
        {
            return right_;
        }
        friend std::ostream& operator <<(std::ostream& os, const rule& r)
        {
            os << r.left_ << " -> ";
            unsigned int i = 0;
            for (const std::vector<std::string>& alternative : r.right_) {
                for (const std::string& symbol: alternative) {
                    os << "<" << symbol << "> ";
                }
                if (i++ < r.right_.size() - 1) {
                    os << "| ";
                }
            }
            return os;
        }
    };

    class Grammar
    {
    public:
        // Create from a stream
        Grammar(std::istream& is)
        {
            std::string line;
            while (std::getline(is, line)) {
                line.erase(line.find_last_not_of("\r\n") + 1);
                if (line.size()) {
                    rule::ptr r = rule::create(line);
                    if (*r) {
                        rules_.push_back(r);
                    }
                }
            }
            // Get the terminals
            std::set<std::string> nonterminals;
            std::set<std::string> symbols;
            for (rule::const_ptr r : rules_) {
                nonterminals.insert(r->left());
                for (const std::vector<std::string>& alternative : r->right()) {
                    for (const std::string& symbol : alternative) {
                        symbols.insert(symbol);
                    }
                }
            }
            for (const std::string& symbol : symbols) {
                if (nonterminals.find(symbol) == nonterminals.end()) {
                    terminals_.push_back(symbol);
                }
            }

            for (const std::string& symbol : terminals_) {
                std::cout << symbol << std::endl;
            }
        }

        const std::vector<rule::ptr>& rules() const
        {
            return rules_;
        }

        // Get all of the rules where symbol is the subject
        template <class OutputIt>
        OutputIt get_rules_for_left(const std::string& symbol, OutputIt it) const
        {
            for (rule::const_ptr r : rules_) {
                if (r->left() == symbol) {
                    *it++ = r;
                }
            }
            return it;
        }

        // Is this symbol a terminal (doesn't occur as the subject of a rule)?
        bool symbol_is_terminal(const std::string& symbol) const
        {
            return std::find(terminals_.begin(), terminals_.end(), symbol) != terminals_.end();
        }

        // Get the rule(s) whose left-hand side is the start symbol
        template <class OutputIt>
        OutputIt get_start_rules(OutputIt it) const
        {
            std::string start_symbol;
            bool started = false;
            for (rule::const_ptr r : rules_) {
                if (!started || r->left() == start_symbol) {
                    *it++ = r;
                }
                if (!started) {
                    started = true;
                    start_symbol = r->left();
                }
            }
            return it;
        }

        // Get the rules where any of these pairs of symbols occur together as a right hand side alternative
        template <class InputIt, class OutputIt>
        OutputIt get_rules_for_symbols(InputIt begin, InputIt end, OutputIt it) const
        {
            for (InputIt init = begin; init != end; ++init) {
                for (rule::const_ptr r : rules_) {
                    for (const std::vector<std::string>& alternative : r->right()) {
                        if (alternative.size() == 2
                            && alternative[0] == init->first
                            && alternative[1] == init->second) {
                            *it++ = r;
                        }
                    }
                }
            }
            return it;
        }

        // Get the rules where this symbol occurs in a right hand side alternative
        template <class OutputIt>
        OutputIt get_rules_for_symbol(const std::string& symbol, OutputIt it) const
        {
            for (rule::const_ptr r : rules_) {
                for (const std::vector<std::string>& alternative : r->right()) {
                    if (std::find(alternative.begin(), alternative.end(), symbol) != alternative.end()) {
                        *it++ = r;
                    }
                }
            }
            return it;
        }

        // Get the subject of the start symbol
        const std::string& get_start_left() const
        {
            return rules_.front()->left();
        }

        // Is this symbol the left (subject) of a terminal?
        bool symbol_is_left_of_terminal(const std::string& symbol) const
        {
            for (rule::const_ptr r : rules_) {
                if (r->left() == symbol) {
                    for (const std::vector<std::string>& alternative : r->right()) {
                        if (alternative.size() == 1 && symbol_is_terminal(alternative[0])) {
                            return true;
                        }
                    }
                }
            }
            return false;
        }

        // Pretty-print
        friend std::ostream& operator <<(std::ostream& os, const Grammar& gram)
        {
            for (rule::const_ptr r : gram.rules_) {
                os << *r;
                os << '\n';
            }
            return os;
        }

    private:
        std::vector<rule::ptr> rules_;
        std::vector<rule::ptr> start_rules_;
        std::vector<std::string> terminals_;
    };

} // namespace MB

#endif //CODEGENERATOR_GRAMMAR_H
