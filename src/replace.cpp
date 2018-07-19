//
// replace.cpp
//
// Perform search-and-replace using pcre++ on file names matching file_regex.
//
// Created by Daniel Kozitza
//

#include <csignal>
#include <cstdlib>
#include <dirent.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include "options.hpp"
#include "tools.hpp"

using namespace tools;

void help(string prog_name);
void more_info();

int main(int argc, char *argv[]) {
   string file_regex, match_regex, replacement;
   string dir_path     = ".";
   string prog_name    = string(argv[0]);
   string m[10];
   options opt;
   bool all            = false;
   bool recursive      = false;
   bool quiet          = false;
   bool test           = false;
   bool file_list      = false;
   bool show_help      = false;
   long total_replaced = 0;
   vector<string> args;
   vector<string> files;
   vector<string> new_files;
   Error e;

   signal(SIGINT, signals_callback_handler);

   opt.handle('a', all);
   opt.handle('r', recursive);
   opt.handle('q', quiet);
   opt.handle('f', file_list);
   opt.handle('h', show_help);
   opt.handle('t', test);

   for (size_t i = 1; i < argc; ++i)
      args.push_back(argv[i]);

   if (pmatches(m, prog_name, "^.*/([^/]+)$"))
      prog_name = m[1];

   e = opt.evaluate(args);
   if (e != NULL) {
      help(prog_name);
      cout << prog_name << "::" << e << "\n";
      return 0;
   }

   if (show_help) {
      help(prog_name);
      more_info();
      return 0;
   }

   if (!file_list && args.size() != 3 || file_list && args.size() < 3) {
      cout << prog_name << ": Invalid argument sequence.\n";
      help(prog_name);
      return 0;
   }

   // get arguments
   match_regex = args[args.size() - 2];
   replacement = args[args.size() - 1];
   if (!file_list) {
      file_regex = args[args.size() - 3];
   }
   else {
      for (size_t i = 0; i < args.size() - 2; ++i)
         files.push_back(args[i]);
   }

   // get dir_path by searching for the last / in file_regex
   // get the new file_regex by removing dir_path from the original file_regex
   if (!file_list && pmatches(m, file_regex, "^(.*/)")) {
      dir_path = m[1];
      file_regex = file_regex.substr(dir_path.size());
   }

   if (!quiet) {
      if (test || !file_list)
         cout << "\n";
      if (test)
         cout << prog_name << ": test mode: No files will be modified.\n";
      if (!file_list) {
         cout << prog_name << ": searching directory: " << dir_path;
         cout << " for file_regex: " << file_regex << endl;
      }
   }

   if (!file_list) {
      if (recursive) {
         if (!list_dir_r(dir_path, files)) {
            cerr << prog_name << ": Error! exiting.\n\n";
            return 1;
         }
      }
      else if (!list_dir(dir_path, files)) {
         cerr << prog_name << ": Error! exiting.\n\n";
         return 1;
      }

      // remove non-matching file names
      new_files.clear();
      for (size_t i = 0; i < files.size(); ++i) {
         string tmp = files[i];
         // remove directory path
         if (pmatches(m, tmp, "^.*/([^/]+)$"))
            tmp = m[1];
         if (pmatches(tmp, file_regex))
            new_files.push_back(files[i]);
      }
      files = new_files;

      // TODO: ignore binary files
      // loop through files removing directories and prefixing dir_path to file
      // names in files.
      new_files.clear();
      for (size_t i = 0; i < files.size(); ++i) {
         if (dir_path != ".") {
            if (!dir_exists(dir_path + files[i]))
               new_files.push_back(dir_path + files[i]);
         }
         else {
            if (!dir_exists(files[i]))
               new_files.push_back(files[i]);
         }
      }
      files = new_files;
   }

   // loop through files finding match_regex matches and replacing them with
   // replacement on each line of each file.
   bool wrote_col_names = false;
   for (size_t i = 0; i < files.size(); ++i) {
      vector<string> contents; // used to store replacement contents of files
      long replaced = 0; // number of lines replaced

      // open files[i] for reading
      ifstream ifh;
      ifh.open(files[i].c_str());
      if (!ifh.is_open()) {
         cerr << prog_name << ": Error! Could not open `" << files[i];
         cerr << "` for reading.\n\n";
         return 1;
      }

      // replace all of the matches on each line or replace the first match
      // in the file
      while (ifh.peek() != EOF) {
         string line;
         getline(ifh, line);

         if (all) {
            replaced += replace_all(line, match_regex, replacement);
         }
         else if (replaced == 0) {
            if (replace_first(line, match_regex, replacement))
               replaced++;
         }

         contents.push_back(line);
      }
      ifh.close();

      if (!quiet && !wrote_col_names) {
         cout << "\n                  file name";
         cout << "                          matches replaced\n";
         wrote_col_names = true;
      }
      if (!quiet) {
         cout << left << setw(60) << files[i];
         cout << " " << replaced << "\n";
      }

      total_replaced += replaced;

      // skip writing over files[i] if there are no changes or if in test mode
      if (replaced == 0 || test)
         continue;

      // open files[i] for writing
      ofstream ofh;
      ofh.open(files[i].c_str());
      if (!ofh.is_open()) {
         cerr << prog_name << ": Could not open `" << files[i];
         cerr << "` for writing.\n\n";
         continue;
      }
      for (size_t i = 0; i < contents.size(); ++i) {
         contents[i] += "\n";
         ofh.write(contents[i].c_str(), contents[i].size());
      }
      ofh.close();
   } // end of files loop

   if (!quiet) {
      if (wrote_col_names) {
         cout << "\n" << prog_name << ": Total matches replaced: ";
         cout << "                            " << total_replaced << "\n\n";
      }
      else {
         cout << prog_name << ": No files match!\n\n";
      }
   }
   return 0;
}

void help(string p_name) {
   cout << "\n";
   cout << fold(0, 80, p_name +
      " is a tool that performs search-and-replace using pcre++ on file "
      "names matching file_regex.");
   cout << "\n\nUsage:\n\n   " << p_name;
   cout << " [-option] file_regex match_regex replacement\n\n";
   cout << "Options:\n\n";
   cout << "   a - Replace all matches in the files that match file_regex.\n";
   cout << "   r - Search recursively for files that match file_regex.\n";
   cout << "   q - Quiet mode. Nothing will be printed to stdout.\n";
   cout << "   t - Test mode. No files will be modified.\n";
   cout << "   f - The file(s) will be input as a list.\n";
   cout << "   h - Print more information.\n\n";
}

void more_info() {
   cout << "Arguments:\n\n";
   cout << "   file";
   cout << fold(
         7,
         80,
         "_regex  - A Perl regular expression to match a file in the current "
         "working directory. A directory path can be prepended to the regular "
         "expression. the / character is not allowed in the regular expression "
         "part.");
   cout << "\n\n   ma";
   cout << fold(
         7,
         80,
         "tch_regex - A Perl regular expression to be used for search and "
         "replace.");

   cout << "\n\n   rep";
   cout << fold(
         7,
         80,
         "lacement - A string that will replace whatever matches match_regex. "
         "This string can contain the special variables #1, #2, #3, etc. These "
         "contain the backreferences.");
   cout << "\n\n";
}
