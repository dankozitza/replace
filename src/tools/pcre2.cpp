//
// pcre2.cpp
//
// Created by Daniel Kozitza

#define PCRE2_CODE_UNIT_WIDTH 8

#include "../tools.hpp"
#include <cassert>
#include <pcre2.h>

bool tools::pmatches(string s, string str_re) {
   string *placeholder;
   return pmatches(placeholder, s, str_re, false);
}

bool tools::pmatches(string results[], string s, string str_re) {
   return pmatches(results, s, str_re, true);
}

bool tools::pmatches(vector<string> &results, string s, string str_re) {
   return pmatches(results, s, str_re, true);
}

bool tools::pmatches(string results[], string s, string str_re, bool get_res) {
   vector<string> m;
   if (!pmatches(m, s, str_re, get_res))
      return false;

   for (size_t i = 0; i < m.size(); ++i)
      results[i] = m[i];
   return true;
}

bool tools::pmatches(
      vector<string> &results,
      string s,
      string str_re,
      bool get_res) {

   results.clear();
   pcre2_code *re;

   // PCRE2_SPTR is a pointer to unsigned code units of
   // the appropriate width (8, 16, or 32 bits).
   PCRE2_SPTR pattern;
   PCRE2_SPTR subject;
   PCRE2_SPTR name_table;

   PCRE2_SIZE erroroffset;
   PCRE2_SIZE *ovector;

   size_t subject_length;
   pcre2_match_data *match_data;

   pattern = (PCRE2_SPTR)str_re.c_str();
   subject = (PCRE2_SPTR)s.c_str();

   int errornumber;
   int rc;

   // compile the regular expression pattern
   re = pcre2_compile(
         pattern,               // the pattern
         PCRE2_ZERO_TERMINATED, // indicates the pattern is zero-terminated
         0,                     // default options
         &errornumber,
         &erroroffset,
         NULL);                 // use the default compile context

   if (re == NULL) {
      // compilation failed
      PCRE2_UCHAR buffer[256];
      pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
      cerr << "tools::pmatch: PCRE2 compilation failed at offset ";
      cerr << (int)erroroffset << ": " << buffer << "\n";
      return false;
   }

   // create match data
   match_data = pcre2_match_data_create_from_pattern(re, NULL);

   // do the match
   rc = pcre2_match(
         re,         // the compiled pattern
         subject,    // the subject string
         s.size(),   // the length of the subject
         0,          // start at offset 0 in the subject
         0,          // default options
         match_data, // block for storing the result
         NULL);      // use default match context

   if (rc < 0) {
      // matching failed
      if (rc != PCRE2_ERROR_NOMATCH) {
         cerr << "tools::pmatch: matching error " << rc << ".\n";
      }
      pcre2_match_data_free(match_data);
      pcre2_code_free(re);
      return false;
   }

   // match succeded.

   if (get_res) {
      // populate the results array.
      // get a pointer to the output vector, where string offsets are stored.
      ovector = pcre2_get_ovector_pointer(match_data);

      // The output vector wasn't big enough. This should not happen, because we
      // used pcre2_match_data_create_from_pattern() above.
      assert(rc != 0);

      for (int i = 0; i < rc; ++i) {
         results.push_back("");
         PCRE2_SPTR substring_start = subject + ovector[2*i];
         size_t substring_length = ovector[2*i+1] - ovector[2*i];
         for (int j = 0; j < substring_length; ++j)
            results[i].push_back(substring_start[j]);
      }
   }

   return true;
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
}

struct ResultParts {
   string part;
   size_t m_index;
};

bool tools::replace_first(string &s, string str_re, string rpl) {
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
   for (long i = 0; i < scratch.size(); ++i) {
      if (scratch[i] == '#') {
         ResultParts rp;
         if (i > 0)
            rp.part = rpl.substr(rpl_i, i);
         else
            rp.part = "";

         rp.m_index = 0;
         i++;
         while (scratch[i] >= '0' && scratch[i] <= '9') {
            rp.m_index = rp.m_index * 10 + scratch[i] - '0';
            i++;
         }

         result_parts.push_back(rp);
         scratch = scratch.substr(i);
         rpl_i += i;
         i = -1;
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
