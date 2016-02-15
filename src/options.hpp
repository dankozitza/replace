// 
// options.hpp
//
// Created by Daniel Kozitza
//

#ifndef _ARGUMENTS
#define _ARGUMENTS

#include <map>
#include <string>
#include <vector>

using namespace std;

class options {

   private:
      struct Option {
         bool* option_set;
         bool has_arguments;
         vector<string>* opt_args;
      };

      size_t args_removed; // number of entries removed from argv during eval
      size_t opts_found;   // number of options found during evaluate
      map<char, Option> opts;

   public:
      options();
      void handle(char option_entry, bool& option_set);
      void handle(char option_entry, bool& option_set, string& argument);
      // option_args.size() will determine the number of arguments to take
      // from the arguments vector during evaluate.
      // considering: If option_args.size() is 0 then take all following
      // arguments that do not begin with -
      void handle(
            char option_entry,
            bool& option_set,
            vector<string>& option_args); 
      void evaluate(vector<string>& arguments);
      size_t arguments_removed();
      size_t options_found();
};

#endif
