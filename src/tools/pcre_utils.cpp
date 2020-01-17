//
// pcre_utils.cpp
//
// Created by Daniel Kozitza

#include "../tools.hpp"
#include <cassert>
#include <regex>

bool tools::pmatches(string s, string str_re) {
   vector<string> placeholder(0);
   return pmatches(placeholder, s, str_re, false);
}

bool tools::pmatches(string results[], string s, string str_re) {
   return pmatches(results, s, str_re, true);
}

bool tools::pmatches(vector<string> &results, string s, string str_re) {
   return pmatches(results, s, str_re, true);
}

bool tools::pmatches(string results[], string s, string str_re, bool get_res) {
   vector<string> m(0);
   if (!pmatches(m, s, str_re, get_res))
      return false;

   for (size_t i = 0; i < m.size(); ++i) {
      results[i] = m[i];
   }
   return true;
}

bool tools::pmatches(
      vector<string> &results,
      string s,
      string str_re,
      bool get_res) {

   results.clear();

   try {
      std::regex re(str_re.c_str());
      std::smatch match;

      if (std::regex_search(s, match, re)) {
         results.resize(match.size());
         results[0] = s;
         for (int i = 1; i < match.size(); i++) {
            results[i] = match.str(i);
         }
         return true;
      }
      else {
         return false;
      }
   } catch (std::regex_error& e) {
     // Syntax error in the regular expression
     cout << "tools::pcre_utils: Syntax error in regular expression `"
          << str_re.c_str() << "`: " << e.what() << endl;
     return false;
   }

   // pcre++ code
   //pcrepp::something re(str_re);

   //if (re.search(s) == true) {
   //   if (get_res && re.matches() > 0) {
   //      results.resize(re.matches() + 1);
   //      results[0] = s;
   //      for (int i = 0; i < re.matches(); i++) {
   //         results[i+1] = re[i];
   //      }
   //   }
   //   return true;
   //}
   //return false;
}

void tools::test_pmatches() {
   string am[10];
   if (pmatches(am, "subject", "(sub)(.*)") == false)
      cout << "test failed!, first matches call returned false!\n";

   if (am[0] != "subject")
      cout << "test failed!, match 0 [" << am[0] << "] did not match!\n";

   if (am[1] != "sub")
      cout << "test failed!, match 1 [" << am[1] << "] did not match!\n";

   if (am[2] != "ject")
      cout << "test failed!, match 2 [" << am[2] << "] did not match!\n";

   if (pmatches(am, "a   ", "^(\\w+)\\s+(\\w*)$") == false)
      cout << "test failed!, second matches call returned false!\n";

   if (am[0] != "a   ")
      cout << "test failed!, match 0 [" << am[0] << "] did not match!\n";

   if (am[1] != "a")
      cout << "test failed!, match 1 [" << am[1] << "] did not match!\n";

   if (am[2] != "")
      cout << "test failed!, match 2 [" << am[2] << "] did not match!\n";

   // test syntax error
   pmatches(am, "subject", "(");
}

struct ResultParts {
   string part;
   size_t m_index;
};

bool tools::replace_first(string &s, string str_re, string rpl) {
   vector<string> m;
   vector<ResultParts> result_parts;
   string scratch = rpl;

   while(tools::pmatches(m, scratch, "^(.*)\\\\#(.*)$")) {
      scratch = m[1] + "ph" + m[2];
   }

   size_t rpl_i = 0;
   size_t scr_i = 0;
   while (scr_i < scratch.size()) {
      // get the the backreference
      if (scratch[scr_i] == '#') {
         ResultParts rp;
         if (scr_i > 0)
            rp.part = rpl.substr(rpl_i, scr_i);
         else
            rp.part = "";

         rp.m_index = 0;
         scr_i++;
         while (scratch[scr_i] >= '0' && scratch[scr_i] <= '9') {
            rp.m_index = rp.m_index * 10 + scratch[scr_i] - '0';
            scr_i++;
         }

         // #+ variable holds the last backreference
         //if (scratch[scr_i] == '+') {

         // #& variable holds the entire match_regex
         //if (scratch[scr_i] == '&') {

         result_parts.push_back(rp);
         scratch = scratch.substr(scr_i);
         rpl_i += scr_i;
         scr_i = 0;
      }
      else {
         scr_i++;
      }
   }
   ResultParts last_rp;
   last_rp.part = rpl.substr(rpl_i);
   last_rp.m_index = 0;
   result_parts.push_back(last_rp);

   // replace \# with # and \0 - \9 with 0 - 9
   for (size_t i = 0; i < result_parts.size(); ++i) {
      while (pmatches(m, result_parts[i].part, "^(.*)\\\\#(.*)$")) {
         result_parts[i].part = m[1] + "#" + m[2];
      }
      while (pmatches(m, result_parts[i].part, "^(.*)\\\\(\\d)(.*)$")) {
         result_parts[i].part = m[1] + m[2] + m[3];
      }
   }

   if (pmatches(m, s, "^(.*?)" + str_re + "(.*)$")) {
      s = m[1];
      for (size_t i = 0; i < result_parts.size(); ++i) {

         if (m.size() - 2 <= result_parts[i].m_index) {
            cerr << "tools::replace_all: Number of variables in match regex: ";
            cerr << "`" << str_re << "` does not match variable named in ";
            cerr << "replacement string: `" << rpl << "`.\n";
            exit(1);
         }

         s += result_parts[i].part;
         if (result_parts[i].m_index > 0)
            s += m[result_parts[i].m_index + 1];
      }
      s += m[m.size() - 1];
      return true;
   }
   return false;
}

// rpl may contain the special variables #1, #2, #3, etc. which will be
// set to the groups in the str_re in their respective order.
// digits in rpl can be escaped to allow for rpl strings like '$1\23'
// this will result in '1_contents23'
size_t tools::replace_all(string &s, string str_re, string rpl) {
   vector<string> m;
   vector<ResultParts> result_parts;
   string scratch = rpl;
   size_t count = 0;

   while(tools::pmatches(m, scratch, "^(.*)\\\\#(.*)$")) {
      scratch = m[1] + "ph" + m[2];
   }

   size_t rpl_i = 0;
   size_t scr_i = 0;
   while (scr_i < scratch.size()) {
      // get the the backreference
      if (scratch[scr_i] == '#') {
         ResultParts rp;
         if (scr_i > 0)
            rp.part = rpl.substr(rpl_i, scr_i);
         else
            rp.part = "";

         rp.m_index = 0;
         scr_i++;
         while (scratch[scr_i] >= '0' && scratch[scr_i] <= '9') {
            rp.m_index = rp.m_index * 10 + scratch[scr_i] - '0';
            scr_i++;
         }

         // #+ variable holds the last backreference
         //if (scratch[scr_i] == '+') {

         // #& variable holds the entire match_regex
         //if (scratch[scr_i] == '&') {

         result_parts.push_back(rp);
         scratch = scratch.substr(scr_i);
         rpl_i += scr_i;
         scr_i = 0;
      }
      else {
         scr_i++;
      }
   }
   ResultParts last_rp;
   last_rp.part = rpl.substr(rpl_i);
   last_rp.m_index = 0;
   result_parts.push_back(last_rp);

   // replace \# with # and \0 - \9 with 0 - 9
   for (size_t i = 0; i < result_parts.size(); ++i) {
      while (pmatches(m, result_parts[i].part, "^(.*)\\\\#(.*)$")) {
         result_parts[i].part = m[1] + "#" + m[2];
      }
      while (pmatches(m, result_parts[i].part, "^(.*)\\\\(\\d)(.*)$")) {
         result_parts[i].part = m[1] + m[2] + m[3];
      }
   }

   size_t at_index = 0;
   while (pmatches(m, s.substr(at_index), "^(.*?)" + str_re + "(.*)$")) {

      s = s.substr(0, at_index) + m[1];
      for (size_t i = 0; i < result_parts.size(); ++i) {

         if (m.size() - 2 <= result_parts[i].m_index) {
            cerr << "tools::replace_all: Number of variables in match regex: ";
            cerr << "`" << str_re << "` does not match variable named in ";
            cerr << "replacement string: `" << rpl << "`.\n";
            exit(1);
         }

         s += result_parts[i].part;
         if (result_parts[i].m_index > 0)
            s += m[result_parts[i].m_index + 1];
      }
      at_index = s.size();
      s += m[m.size() - 1];
      count++;
   }

   return count;
}
