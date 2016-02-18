// 
// options.hpp
//
// This object is designed to be the first pass over a vector of arguments which
// identifies and removes all options. An option is identified as any argument
// that begins with '-'. The "--" option can be given to end option evaluation.
// Options can be boolean or they can have a set number of arguments. Multiple
// options can be given in the same argument (ex: "-abc"). Options with
// arguments must be the last character in the argument.
//
// Example client code:
//
//    options opt;
//    bool help = false;
//    bool file = false;
//    bool two  = false;
//    string file_name;
//    vector<string> two_args(2);
//
//    opt.handle('h', help);
//    opt.handle('f', file, file_name);
//    opt.handle('t', two, two_args);
//
//    vector<string> args;
//    for (int i = 1; i < argc; ++i)
//       args.push_back(argv[i]);
//
//    tools::Error e; // this is just char const *
//    e = opt.eval(args);
//    if (e != NULL)
//       cout << "got error: " << e << endl;
//
//    if (help)
//       cout << "help message!\n";
//    if (file)
//       cout << "file_name: " << file_name << endl;
//    if (two) {
//       cout << "two_args[0]: " << two_args[0] << endl;
//       cout << "two_args[1]: " << two_args[1] << endl;
//    }
//
//    for (int i = 0; i < args.size(); ++i)
//       cout << "args[" << i << "]: " << args[i] << endl;
//
// Example call and output:
//
//    $ ./example -hf file_1 -t one two args_0 args_1
//      help message!
//      file_name: file_1
//      two_args[0]: one
//      two_args[1]: two
//      args[0]: args_0
//      args[1]: args_1
//
// Created by Daniel Kozitza
//

#ifndef _OPTIONS
#define _OPTIONS

#include <map>
#include <string>
#include <vector>
#include "tools.hpp"

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
      // arguments. Stop for -* and eol.
      void handle(
            char option_entry,
            bool& option_set,
            vector<string>& option_args);

      tools::Error evaluate(vector<string>& arguments);
      size_t arguments_removed();
      size_t options_found();
};

#endif
