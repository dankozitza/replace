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
         bool* bool_opt;
         vector<string>* vector_opt;
         bool has_arguments;
      };

      typedef map<string, Option>::iterator cmds_iter;

      map<string, Option> opts;

   public:
      options();
      void handle(char option_entry, bool& option_set);
      void handle(char option_entry, bool& option_set, vector<string>& option_args);
      void evaluate(vector<string>& arguments);
};

#endif
