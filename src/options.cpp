// 
// options.cpp
//
// Created by Daniel Kozitza
//

#include "options.hpp"

options::options() {
   args_removed = 0;
   opts_found = 0;
}

void options::handle(char option_entry, bool& option_set) {
   Option tmp;
   tmp.option_set = &option_set;
   tmp.has_arguments = false;
   opts[option_entry] = tmp;
}

// seg faults
//void options::handle(char option_entry, bool& option_set, string& argument) {
//   vector<string> tmp(1);
//   //tmp[0] = argument;
//   handle(option_entry, option_set, tmp);
//}

void options::handle(
      char option_entry,
      bool& option_set,
      vector<string>& option_args) {
   Option tmp;
   tmp.option_set = &option_set;
   tmp.has_arguments = true;
   tmp.opt_args = &option_args;
   opts[option_entry] = tmp;
}

tools::Error options::evaluate(vector<string>& arguments) {
   vector<string> new_arguments;
   size_t i;
   for (i = 0; i < arguments.size(); ++i) {
      if (arguments[i] == "--") {
         ++i;
         break;
      }
      
      if (arguments[i].size() < 2 || arguments[i][0] != '-') {
         new_arguments.push_back(arguments[i]);
         continue;
      }

      args_removed++;

      size_t removed = 0;
      for (size_t j = 1; j < arguments[i].size(); ++j) {
         char c = arguments[i][j];
         Option opt = opts[c];
         if (opt.option_set != NULL) {
            opts_found++;
            bool* ptr = opt.option_set;
            *ptr = true;

            if (opt.has_arguments) {
               // make sure this is the last character in the option string
               if (j+1 != arguments[i].size()) {
                  return tools::errorf(
                        "options::evaluate: The %c option must be followed by "
                        "%i arguments.", c, opt.opt_args->size());
               }

               for (size_t k = 0; k < opt.opt_args->size(); ++k) {
                  if (arguments.size() <= i + 1 + k) {
                     return tools::errorf(
                        "options::evaluate: The %c option requires "
                        "%i arguments.", c, opt.opt_args->size());
                  }
                  args_removed++;
                  opt.opt_args->operator[](k) = arguments[i + 1 + k];
               }
               // make sure we jump ahead of the arguments we just took
               removed = opt.opt_args->size();
            }

            opts[c] = opt;
            if (j+1 >= arguments[i].size()) {
               i += removed;
               break;
            }
            else if (removed != 0) {
               return tools::errorf(
                  "options::evaluate: The %c option was entered incorrectly.",
                  c);
            }
         }
         else {
            cout << "options::evaluate: Invalid option `" << c << "`.\n";
         }
      }
   }
   if (i == 0)
      return NULL;
   // add remaining arguments in case -- option was given
   for (i; i < arguments.size(); ++i) {
      new_arguments.push_back(arguments[i]);
   }
   arguments.resize(0);
   for (int m = 0; m < new_arguments.size(); ++m)
      arguments.push_back(new_arguments[m]);
   return NULL;
}

size_t options::arguments_removed() {
   return args_removed;
}

size_t options::options_found() {
   return opts_found;
}
