//
// system.cpp
//
// Created by Daniel Kozitza
//

#include <csignal>
#include <cstdlib>
#include <dirent.h>
#include "../tools.hpp"

// 
// SIGABRT: 6
// SIGFPE: 8
// SIGILL: 4
// SIGINT: 2
// SIGSEGV: 11
// SIGTERM: 15
//
vector<void (*)(int)> SigHandlers[16];

// signals_callback_handler
//
// the signal handler for all signal handlers. runs all the signal handlers in 
// the SigHandlers vector associated with that signal. use the signals function
// to add handlers to the SigHandlers array of vectors.
//
// function to be called when ctrl-c (SIGINT) signal is sent to process
//
void tools::signals_callback_handler(int signum) {

   cout << "\ntools::signals_callback_handler: caught signal `";
   cout << signum << "`\n";

   if (signum < 0 || signum > 15) {
      cout << "   signal number out of bounds!!!\n";
      exit(signum);
   }

   // run all other handlers
   // this doesn't work if the handler exits
   for (const auto handler : SigHandlers[signum])
      handler(signum);

   // Terminate program
   exit(signum);
}

// signals
//
// replacement for signal that allows multiple callback handlers to be used for
// each signal. If a callback handler supplied to signals calls `exit`, all
// uncalled handlers in the SigHandlers vector will not be called.
// 
// signal(SIGINT, signals_callback_handler) must be called for program to exit
// on interrupt.
//
void tools::signals(int sig, void (*callback_func)(int)) {
   SigHandlers[sig].push_back(callback_func);
}

// require
//
// overloaded function for exiting on failure with optional message.
// TODO: have this throw an exception instead.
//
// exit if sys_exit_val is not zero.
//
bool tools::require(int sys_exit_val, string msg)
{
   if (sys_exit_val != 0) {
      if (msg != "") {
         cout << msg << "\n";
      }
      cout << "tools::require: got exit value `" << sys_exit_val << "`, ";
      cout << "exiting.\n";
      exit(EXIT_FAILURE);
   }
}

// require
//
// exit if func_return_val is false
//
bool tools::require(bool func_return_val, string msg) {
   if (func_return_val == false) {
      if (msg != "") {
         cout << msg << "\n";
      }
      cout << "tools::require: got return value `false`, ";
      cout << "exiting.\n";
      exit(EXIT_FAILURE);
   }
}

// dir_exists
//
// Returns true if the dir_name is a directory false otherwise.
//
bool tools::dir_exists(string dir_name) {
   DIR *dir;
   struct dirent *ent;
   if ((dir = opendir(dir_name.c_str())) != NULL) {
      closedir(dir);
      return true;
   }
   else {
      return false;
   }
}

// list_dir
//
// Takes a directory name and a reference to a vector of strings. adds file
// and directory names found in 'list_dir' to the contents vector. Returns true
// if the dir_name can be opened false otherwise.
//
bool tools::list_dir(string dir_name, vector<string>& contents) {
   DIR *dir;
   struct dirent *ent;
   if ((dir = opendir(dir_name.c_str())) != NULL) {
      // add all the files and directories within directory to contents
      while ((ent = readdir (dir)) != NULL) {
         contents.push_back(ent->d_name);
      }
      closedir(dir);
      return true;
   }
   else {
      // could not open directory
      cout << "tools::list_dir: could not open directory: `" << dir_name;
      cout << "`\n";
      return false;
   }
}

// list_dir_r
//
// Same as list_dir but does not include . or .. directories and it goes into
// each directory recursively.
//
bool tools::list_dir_r(string dir_name, vector<string>& contents) {
   bool ret = list_dir_r(dir_name, contents, "");
   if (!ret) {
      cout << "tools::list_dir_r: could not open directory: `" << dir_name;
      cout << "`\n";
   }
   return ret;
}
bool tools::list_dir_r(
      string dir_name,
      vector<string>& contents,
      string prefix) {
   DIR *dir;
   struct dirent *ent;

   // remove trailing / from dir_name
   if (dir_name.size() >= 3 && dir_name[dir_name.size() - 1] == '/')
      dir_name.erase(dir_name.size() - 1);

   if ((dir = opendir(dir_name.c_str())) != NULL) {

      // add all the files and directories within directory to contents
      while ((ent = readdir(dir)) != NULL) {
         string new_dir_name = ent->d_name;
         string entry_name;

         // ignore . and .. directories
         if (new_dir_name == "." || new_dir_name == "..")
            continue;

         entry_name = new_dir_name;

         if (prefix != "") {
            if (prefix == "/")
               entry_name = prefix + entry_name;
            else
               entry_name = prefix + "/" + entry_name;
         }

         contents.push_back(entry_name);

         if (dir_name == "/")
            new_dir_name = dir_name + new_dir_name;
         else
            new_dir_name = dir_name + "/" + new_dir_name;

         list_dir_r(new_dir_name, contents, entry_name);
      }
      closedir(dir);
      return true;
   }
   else {
      // could not open directory
      return false;
   }
}
