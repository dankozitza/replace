//
// tools.hpp
//
// utility functions for mc
//
// Created by Daniel Kozitza
//

#ifndef _TOOLS
#define _TOOLS

#include <iostream>
#include <map>
#include <stdio.h>
#include <stdarg.h>
#include <vector>

using namespace std;

namespace tools {

   // utils.cpp
   bool get_vfnmkmc_conf(map<string, string>& config);
   bool save_vfnmkmc_conf(map<string, string>& config);
   string get_src_files(string src_dir);

   // system.cpp
   typedef char const * Error;
   Error error(string message = "");
   Error errorf(char const * format, ...);
   void signals_callback_handler(int signum);
   void signals(int sig, void (*callback_func)(int));
   bool require(int sys_exit_val, string msg = "");
   bool require(bool func_return_val, string msg = "");
   bool dir_exists(string dir_name);
   bool list_dir(string dir_name, vector<string>& contents);
   bool list_dir_r(string dir_name, vector<string>& contents);
   bool list_dir_r(
         string dir_name,
         vector<string>& contents,
         string prefix);

   // strings.cpp
   string fold(int indent_width, int max_line_width, string s);
   //bool find_in_file(string str_re, string fname);
   //void test_matches();
   //void test_replace();

   // pcre_utils.cpp
   bool pmatches(string s, string str_re);
   bool pmatches(string results[], string s, string str_re);
   bool pmatches(vector<string> &results, string s, string str_re);
   bool pmatches(string results[], string s, string str_re, bool get_res);
   bool pmatches(
         vector<string> &results,
         string s,
         string str_re,
         bool get_res);
   void test_pmatches();
   bool replace_first(string &s, string str_re, string rpl);
   size_t replace_all(string &s, string str_re, string rpl);


   // vectors.cpp
   istream& operator>>(istream& is, vector<string>& v);
   ostream& operator<<(ostream& os, vector<string>& v);
   void vectors_test();
}

#endif
