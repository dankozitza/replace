//
// strings.cpp
//
// Created by Daniel Kozitza
//

#include "../tools.hpp"
#include <fstream>

// fold
//
// a function that takes an indent width, a max line width, and a string. The
// string is broken at max line width with a newline and the next line is
// indented with indent_width spaces. The first line is expected to be indented
// outside of this function so the first line broken will be broken at
// max_line_width - indent_width.
//
string tools::fold(int indent_width, int max_line_width, string s) {
   string indent;
   string word;
   for (int i = 0; i < indent_width; ++i)
      indent += ' ';

   // loop through each character in s
   int char_cnt = indent_width;
   string new_s;
   for (int i = 0; i < s.size(); ++i) {
      if (s[i] == '\n') {
         new_s += '\n' + indent + word + " ";
         char_cnt = indent_width + word.size() + 1;
         word = "";
      }
      else {

         if (s[i] != ' ') {
            // if s[i] is not a space add it to the current word
            word.push_back(s[i]);
         }
         else if (word.size() + char_cnt > max_line_width) {

            new_s += '\n' + indent;

            // add the word to the new line
            char_cnt = indent_width + word.size();
            new_s += word;
            word = "";

            // append a space if there is room
            if (char_cnt + 1 < max_line_width) {
               new_s += ' ';
               char_cnt++;
            }
         }
         else {
            // add the word to the current line
            char_cnt += word.size();
            new_s += word;
            word = "";
            // append a space if there is room
            if (char_cnt + 1 < max_line_width) {
               new_s += ' ';
               char_cnt++;
            }
         }
      }
   }

   // add the last word
   if (word.size() + char_cnt >= max_line_width)
      new_s += "\n" + indent;
   new_s += word;

   return new_s;
}

//
//// matches
////
//// overloaded function that implements regex_match()
////
//bool tools::matches(string s, string str_re) {
//   smatch sm;
//   return matches(sm, s, str_re);
//}
//
//// matches
////
//// in case i don't want to include regex
////
//bool tools::matches(string results[], string s, string str_re) {
//   smatch sm;
//   bool ret = matches(sm, s, str_re);
//   if (!ret)
//      return false;
//   for (unsigned i=0; i<sm.size(); i++) {
//      results[i] = sm[i];
//   }
//   return true;
//}
//bool tools::matches(vector<string>& results, string s, string str_re) {
//   smatch sm;
//   bool ret = matches(sm, s, str_re);
//   if (!ret)
//      return false;
//   for (unsigned i=0; i<sm.size(); i++) {
//      results.push_back(sm[i]);
//   }
//   return true;
//}
//
//// matches
////
//// saves the work of creating a regex object
////
//// use R"(raw_characters)" for 'str_re' to avoid double escaping regex
////
//bool tools::matches(smatch& sm, string s, string str_re) {
//
//   sm = smatch();
//   regex re;
//   try {
//      regex rete (str_re.c_str());//, regex_constants::basic);
//      re.assign(rete);
//   }
//   catch (regex_error& e){
//      cout << "tools::matches: regex returned error code: `";
//      cout << e.code() << "` when evaluating expression: `" << str_re << "`\n";
//      throw e;
//   }
//
//   try {
//      //if (!regex_match(s, sm, re))
//      //   return false;
//      if (!regex_search(s, sm, re))
//         return false;
//
//   }
//   catch (regex_error& e){
//      cout << "tools::matches: regex_match returned error code: `";
//      cout << e.code() << "` when called with s: `" << s;
//      cout << "` and expression: `" << str_re << "`\n";
//      return false;
//   }
//
//   return true;
//}
//
//bool tools::replace_first(string &s, string str_re, string fmt) {
//   return replace_all(s, str_re, fmt, regex_constants::format_first_only);
//}
//
//bool tools::replace_all(
//      string &s,
//      string str_re,
//      string fmt,
//      regex_constants::match_flag_type mf) {
//
//   regex re;
//   try {
//      regex rete (str_re.c_str());//regex_constants::format_no_copy);
//      re.assign(rete);
//   }
//   catch (regex_error& e){
//      cout << "tools::replace_all: regex returned error code: `";
//      cout << e.code() << "` when evaluating expression: `" << str_re << "`\n";
//      throw e;
//   }
//
//   try {
//      s = regex_replace(s, re, fmt, mf);
//   }
//   catch (regex_error& e){
//      cout << "tools::replace_all: regex_match returned error code: `";
//      cout << e.code() << "` when called with s: `" << s;
//      cout << "` and expression: `" << str_re << "`\n";
//      return false;
//   }
//
//   return true;
//}
//
//bool tools::find_in_file(string str_re, string fname) {
//
//   cout << "tools::find_in_file: opening file `" << fname << "`.\n";
//
//   ifstream ifh;
//   ifh.open(fname, ifstream::in);
//   if (!ifh.is_open()) {
//      cout << "tools::find_in_file: couldn't open `" << fname << "`.\n";
//      return false;
//   }
//
//   while (ifh.peek() != EOF) {
//      string line;
//      getline(ifh, line);
//
//      if (matches(line, str_re)) {
//         ifh.close();
//         return true;
//      }
//   }
//
//   ifh.close();
//   return false;
//}
//
//// test_matches
////
//// A test for all the matches functions.
////
//void tools::test_matches() {
//
//   string am[10];
//     if (matches(am, "subject", "(sub)(.*)") == false)
//      cout << "test failed!, first matches call returned false!\n";
//
//   if (am[0] != "subject")
//      cout << "test failed!, match 0 [" << am[0] << "] did not match!\n";
//
//   if (am[1] != "sub")
//      cout << "test failed!, match 1 [" << am[1] << "] did not match!\n";
//
//   if (am[2] != "ject")
//      cout << "test failed!, match 2 [" << am[2] << "] did not match!\n";
//
//   if (matches(am, "a   ", "^(\\w+)\\s+(\\w*)$") == false)
//      cout << "test failed!, second matches call returned false!\n";
//
//   if (am[0] != "a   ")
//      cout << "test failed!, match 0 [" << am[0] << "] did not match!\n";
//
//   if (am[1] != "a")
//      cout << "test failed!, match 1 [" << am[1] << "] did not match!\n";
//
//   if (am[2] != "")
//      cout << "test failed!, match 2 [" << am[2] << "] did not match!\n";
//
//   vector<string> vm;
//     if (matches(vm, "subject", "(sub)(.*)") == false)
//      cout << "test failed!, third matches call returned false!\n";
//
//   if (vm[0] != "subject")
//      cout << "test failed!, match 0 [" << vm[0] << "] did not match!\n";
//
//   if (vm[1] != "sub")
//      cout << "test failed!, match 1 [" << vm[1] << "] did not match!\n";
//
//   if (vm[2] != "ject")
//      cout << "test failed!, match 2 [" << vm[2] << "] did not match!\n";
//
//   if (matches(vm, "a   ", "^(\\w+)\\s+(\\w*)$") == false)
//      cout << "test failed!, fourth matches call returned false!\n";
//
//   if (vm[0] != "a   ")
//      cout << "test failed!, match 0 [" << vm[0] << "] did not match!\n";
//
//   if (vm[1] != "a")
//      cout << "test failed!, match 1 [" << vm[1] << "] did not match!\n";
//
//   if (vm[2] != "")
//      cout << "test failed!, match 2 [" << vm[2] << "] did not match!\n";
//
//
//   smatch sm;
//     if (matches(sm, "subject", "(sub)(.*)") == false)
//      cout << "test failed!, fifth matches call returned false!\n";
//
//   if (sm[0] != "subject")
//      cout << "test failed!, match 0 [" << sm[0] << "] did not match!\n";
//
//   if (sm[1] != "sub")
//      cout << "test failed!, match 1 [" << sm[1] << "] did not match!\n";
//
//   if (sm[2] != "ject")
//      cout << "test failed!, match 2 [" << sm[2] << "] did not match!\n";
//
//   if (matches(sm, "a   ", "^(\\w+)\\s+(\\w*)$") == false)
//      cout << "test failed!, sixth matches call returned false!\n";
//
//   if (sm[0] != "a   ")
//      cout << "test failed!, match 0 [" << sm[0] << "] did not match!\n";
//
//   if (sm[1] != "a")
//      cout << "test failed!, match 1 [" << sm[1] << "] did not match!\n";
//
//   if (sm[2] != "")
//      cout << "test failed!, match 2 [" << sm[2] << "] did not match!\n";
//
//   //map<string, string> m;
//     //if (matches(m, "subject", "(sub)(.*)") == false)
//   //   cout << "test failed!, seventh matches call returned false!\n";
//
//   //if (m["0"] != "subject")
//   //   cout << "test failed!, match 0 [" << m["0"] << "] did not match!\n";
//
//   //if (m["1"] != "sub")
//   //   cout << "test failed!, match 1 [" << m["1"] << "] did not match!\n";
//
//   //if (m["2"] != "ject")
//   //   cout << "test failed!, match 2 [" << m["2"] << "] did not match!\n";
//
//   //if (matches(m, "a   ", "^(\\w+)\\s+(\\w*)$") == false)
//   //   cout << "test failed!, eighth matches call returned false!\n";
//
//   //if (m["0"] != "a   ")
//   //   cout << "test failed!, match 0 [" << m["0"] << "] did not match!\n";
//
//   //if (m["1"] != "a")
//   //   cout << "test failed!, match 1 [" << m["1"] << "] did not match!\n";
//
//   //if (m["2"] != "")
//   //   cout << "test failed!, match 2 [" << m["2"] << "] did not match!\n";
//}
//
//// test_replace
////
//// A test for all the replace functions.
////
//void tools::test_replace() {
//
//   string s = "subject";
//     replace_all(s, R"((a|e|i|o|u))", "[$&]");
//
//   if (s != "s[u]bj[e]ct")
//      cout << "test 1 failed!, `" << s << "` should be `s[u]bj[e]ct`!\n";
//
//   replace_all(s, R"((\[|\]))", "");
//
//   if (s != "subject")
//      cout << "test 2 failed!, `" << s << "` should be `subject`!\n";
//
//   replace_first(s, R"(\w)", "S");
//
//   if (s != "Subject")
//      cout << "test 3 failed!, `" << s << "` should be `Subject`!\n";
//
//   s = "sub\nject";
//
//   replace_all(s, R"(\n)", "");
//   if (s != "subject")
//      cout << "test 4 failed!, `" << s << "` should be `subject`!\n";
//}
