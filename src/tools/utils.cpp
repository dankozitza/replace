//
// utils.cpp
//
// utility functions for mc
//
// Created by Daniel Kozitza
//

#include <fstream>
#include "../sorters.hpp"
#include "../tools.hpp"

bool CalledAtexit = false;

// get_vfnmkmc_conf
//
// Reads vnfmake.conf variables into 'config'.
//
bool tools::get_vfnmkmc_conf(map<string, string>& config) {

   ifstream fh;
   fh.open("vfnmkmc.conf", ifstream::in);
   if (!fh.is_open()) {
      cout << "tools::get_vfnmkmc_conf: couldn't open vfnmkmc.conf.\n";
      return false;
   }
   while(fh.peek() != EOF) {
      string line;
      getline(fh, line);

      string m[3];
      if (pmatches(m, line, R"(^\s*([^:]*):\s*(.*)$)"))
         config[m[1]] = m[2];
   }

   fh.close();
   return true;
}

bool tools::save_vfnmkmc_conf(map<string, string>& config) {
   FILE * pFile;
   pFile = fopen("vfnmkmc.conf", "w");
   for (auto& pair : config) {
      string tmp = pair.first + ":";
      fprintf(pFile, "%-20s%s\n", tmp.c_str(), pair.second.c_str());
   }

   fclose(pFile);
   return true;
}

vector<string> Targets;

// this function can be handed to atexit. It will remove all the file names
// in the targets vector.
void destroy_targets() {
   for (const auto fname : Targets) {
      cout << "tools::destroy_targets: removing `" << fname << "`\n";
      remove(fname.c_str());
   }
}

string tools::get_src_files(string src_dir) {
   string src_files;
   vector<string> files;
   require(list_dir_r(src_dir, files));

   sorters::radix(files);
   if (files.size() > 0) {
      for (int i = 0; i < files.size() - 1; ++i) {
         if (pmatches(files[i], R"((\.cpp|\.c|\.hpp|\.h)$)"))
            src_files += files[i] + " ";
      }
      if (pmatches(files[files.size() - 1], R"((\.cpp|\.c|\.hpp|\.h)$)"))
         src_files += files[files.size() - 1];
   }

   return src_files;
}
