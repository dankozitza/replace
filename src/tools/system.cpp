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


// error
//
// Creates a static Error string and returns a pointer to it.
//
// TODO: May want to allocate these differently in case the caller
//       decides to ignore the returned errors.
//
tools::Error tools::error(string message) {
   static string tmp;
   if (message == "")
      tmp = "Generic error.";
   else
      tmp = message;

   return tmp.c_str();
}

//  errorf
//
//  Takes a format string of the form "%i %f %c %s", where the character
//  following % specifies the type of the argument in that position.
//
//  i = int
//  f = float
//  c = char
//  s = string (char *)
//
//  Following the format specification is a variable 
//  list of arguments. Each argument corresponds to 
//  a format character in the format string.
//
tools::Error tools::errorf(char const * format, ...) {
   va_list vl;
   string msg;

   //  format is the last argument specified; you must access 
   //  all others using the variable-argument macros.
   va_start(vl, format);

   // build the string as we go
   for(int i = 0; format[i] != '\0'; ++i ) {

      // skip escaped escape
      if (format[i] == '\\' && format[i+1] == '\\') {
         msg += "\\";
         i++;
         continue;
      }

      // skip escaped %
      if (format[i] == '\\' && format[i+1] == '%') {
         msg += "%";
         i++;
         continue;
      }

      if (format[i] != '%') {
         msg += format[i];
         continue;
      }

      // format[i] is %. get the type and increment i
      char t = format[++i];
      // TODO: maybe this should be dynamic
      char buff[100];
      buff[0] = '\0';

      union Printable_t {
         long    i;
         float   f;
         char    c;
         char   *s;
      } Printable;

      switch( t ) {   // Type to expect.
         case 'i':
            Printable.i = va_arg( vl, long );
            sprintf(buff, "%i", Printable.i);
            msg += buff;
         break;

         case 'f':
            Printable.f = va_arg( vl, double );
            sprintf(buff, "%f", Printable.f);
            msg += buff;
         break;

         case 'c':
            Printable.c = va_arg( vl, int );
            sprintf(buff, "%c", Printable.c);
            cout << "buff: " << buff << endl;
            msg += buff;
         break;

         case 's':
            Printable.s = va_arg( vl, char * );
            sprintf(buff, "%s", Printable.s);
            msg += buff;
         break;

         default:
            msg += '%' + t;
         break;
      }
   }

   va_end( vl );
   return tools::error(msg);
}

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

struct DirInfo {
   string new_dir_name;
   string entry_name;
};

bool tools::list_dir_r(
      string dir_name,
      vector<string>& contents,
      string prefix) {
   DIR *dir;
   struct dirent *ent;
   vector<DirInfo> dir_info;

   // remove trailing / from dir_name
   if (dir_name.size() >= 3 && dir_name[dir_name.size() - 1] == '/')
      dir_name.erase(dir_name.size() - 1);

   if ((dir = opendir(dir_name.c_str())) != NULL) {

      // add all the files and directories within directory to dir_info
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
         if (dir_name == "/")
            new_dir_name = dir_name + new_dir_name;
         else
            new_dir_name = dir_name + "/" + new_dir_name;

         DirInfo tmp;
         tmp.new_dir_name = new_dir_name;
         tmp.entry_name = entry_name;
         dir_info.push_back(tmp);
      }
      closedir(dir);

      // add all the files and directories within dir_info to contents
      for (size_t i = 0; i < dir_info.size(); ++i) {
         contents.push_back(dir_info[i].entry_name);
         list_dir_r(dir_info[i].new_dir_name, contents, dir_info[i].entry_name);
      }
      return true;
   }
   else {
      // could not open directory
      return false;
   }
}
