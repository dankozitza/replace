//
// replace.cpp
//
// replaces one or many strings that match a given regular expression in any
// files that match the file regular expression.
//
// Created by Daniel Kozitza
//

#include <csignal>
#include <cstdlib>
#include <dirent.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sys/ioctl.h>
#include "tools.hpp"
#include "sorters.hpp"

using namespace tools;

//typedef map<string, string>::iterator map_iter;

void help(string prog_name);

int main(int argc, char *argv[]) {
   string file_regex, match_regex, replacement;
   string dir_path     = ".";
   string prog_name    = string(argv[0]);
   bool all            = false;
   bool recursive      = false;
   bool quiet          = false;
   long total_replaced = 0;

   replace_first(prog_name, "^.*/", "");

   if (argc < 4) {
      help(prog_name);
      return 0;
   }

   signal(SIGINT, signals_callback_handler);

   struct winsize ws;
   ioctl(0, TIOCGWINSZ, &ws);

   // get options
   int total_opts = 0;
   for (int i = 0; i < argc - 3; ++i) {
      if (argv[i][0] == '-') {
         int opt_cnt = 1;
         while (argv[i][opt_cnt] != '\0') {
            switch (argv[i][opt_cnt]) {
               case 'a': all       = true; break;
               case 'r': recursive = true; break;
               case 'q': quiet     = true; break;
            }
            opt_cnt++;
         }
         total_opts++;
      }
   }

   if (total_opts + 4 != argc) {
      cout << prog_name << ": invalid argument sequence\n";
      cout << "total_opts: " << total_opts << " argc: " << argc << "\n";
      help(prog_name);
      return 0;
   }

   // get arguments
   file_regex  = string(argv[argc - 3]);
   match_regex = string(argv[argc - 2]);
   replacement = string(argv[argc - 1]);

   // get dir_path
   // dir_path is the directory of the file which matches file_regex. If
   // file_regex begins with a / then the path is ^(.*/)(real_regex)$.
   string m[3];
   if (file_regex[0] == '/') {
      if (pmatches(m, file_regex, "^(.*/)([^/]+)$")) {
         dir_path = m[1];
         file_regex = m[2];
      }
      else {
         cerr << prog_name << ": Internal error! Could not find directory path";
         cerr << ".\n";
         return 1;
      }
   }
   else if (pmatches(m, file_regex, "^(.*/)([^/]+)$")) {
      dir_path = m[1];
      file_regex = m[2];
   }

   if (!quiet) {
      cout << "\n" << prog_name << ": searching directory: " << dir_path;
      cout << " for file_regex: " << file_regex << "\n\n";
   }

   vector<string> files;
   if (recursive) {
      if (!list_dir_r(dir_path, files)) {
         cerr << prog_name << ": Could not open directory path `" << dir_path;
         cerr << "`.\n";
         return 1;
      }
   }
   else {
      if (!list_dir(dir_path, files)) {
         cerr << prog_name << ": Could not open directory path `" << dir_path;
         cerr << "`.\n";
         return 1;
      }
   }

   // TODO: ignore binary files
   //
   // loop through files removing directories and prefixing dir_path to file
   // names in files.
   vector<string> new_files;
   for (size_t i = 0; i < files.size(); ++i)
      if (!dir_exists(dir_path + files[i]))
         new_files.push_back(dir_path + files[i]);
   files = new_files;

   if (!quiet) {
      cout << "                   file name";
      cout << "                          lines replaced\n";
   }

   // loop through files finding match_regex matches and replacing them with
   // replacement on each line of each file.
   for (size_t i = 0; i < files.size(); ++i) {
      vector<string> contents; // used to store replacement contents of files

      //if (!quiet)
      //   cout << "replace: opening file `" << files[i] << "` for reading.\n";
      ifstream ifh;
      ifh.open(files[i].c_str());
      if (!ifh.is_open()) {
         cerr << "replace: couldn't open `" << files[i] << "` for reading.\n";
         return 1;
      }

      // replace all of the matches on each line or replace the first match in
      // the file
      long replaced = 0;
      while (ifh.peek() != EOF) {   
         string line;
         getline(ifh, line);

         if (all) {
            if (replace_all(line, match_regex, replacement))
               replaced++;
         }
         else if (replaced == 0) {
            if (replace_first(line, match_regex, replacement))
               replaced++;
         }

         contents.push_back(line);
      }
      ifh.close();

      if (!quiet) {
         cout << left << setw(60) << files[i];
         cout << " " << replaced << "\n";
      }

      // skip writing over files[i] if there are no changes
      if (replaced == 0)
         continue;

      //if (!quiet)
      //   cout << "replace: opening file `" << files[i] << "_tmp` for writing.\n";
      ofstream ofh;
      string tmpfilename = files[i];
      tmpfilename += "_tmp";
      ofh.open(tmpfilename.c_str());
      if (!ofh.is_open()) {
         cerr << "replace: couldn't open `" << tmpfilename;
         cerr << "` for writing.\n";
         return 1;
      }
      for (size_t i = 0; i < contents.size(); ++i) {
         contents[i] += "\n";
         ofh.write(contents[i].c_str(), contents[i].size());
      }
      ofh.close();
      total_replaced += replaced;
   } // end of files loop

   if (!quiet) {
      cout << "\n" << prog_name << ": total lines replaced: ";
      cout << total_replaced << "\n\n";
   }
   return 0;
}

void help(string p_name) {
   cout << "\n" << p_name;
   cout << " is a tool for replacing strings with other strings";
   cout << " in one or more files.\n\n";
   cout << "Usage:\n\n   replace [-ar] file_regex match_regex replacement\n\n";
}

//void cnt() {
//   map<string, string> vfnconf;
//   vector<string> contents;
//   unsigned int total_lines = 0;
//   string src_dir;
//
//   if (get_vfnmkmc_conf(vfnconf))
//      src_dir = vfnconf["src_directory"];
//   else
//      src_dir = ".";
//
//   if (!list_dir_r(src_dir, contents)) {
//      cerr << "mc::cnt: vfnmkmc src_directory `" + src_dir;
//      cerr << "` does not exist\n";
//      return;
//   }
//
//   vector<string> new_contents;
//   for (int i = 0; i < contents.size(); ++i) {
//      if (pmatches(contents[i], "(\\.cpp|\\.c|\\.hpp|\\.h)$")) {
//         new_contents.push_back(contents[i]);
//      }
//   }
//   contents = new_contents;
//   new_contents.clear();
//
//   int longest = 0;
//   for (int i = 0; i < contents.size(); ++i) {
//      string fname = src_dir + "/" + contents[i];
//      if (fname.size() > longest)
//         longest = fname.size() + 1;
//   }
//
//   sorters::radix(contents);
//
//   ifstream fh;
//   for (int i = 0; i < contents.size(); ++i) {
//      string fname = src_dir + "/" + contents[i];
//
//      fh.open(fname.c_str(), ifstream::in);
//      if (!fh.is_open()) {
//         cout << "mc::cnt: could not open file: `" << fname << "`\n";
//         continue;
//      }
//
//      char c;
//      int file_lines = 0;
//      while (fh.peek() != EOF) {
//         fh.get(c);
//         if (c == '\n')
//            ++file_lines;
//      }
//      fh.close();
//      total_lines += file_lines;
//
//      fname += ":";
//      cout << left << setw(longest) << fname;
//      cout << " " << file_lines << endl;
//   }
//
//   cout << endl << left << setw(longest) << "total_loc:";
//   cout << " " << total_lines << "\n\n";
//}
