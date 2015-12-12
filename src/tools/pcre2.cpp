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

bool tools::pmatches(string results[], string s, string str_re, bool get_res) {
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
         results[i] = "";
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

bool tools::replace_first(string &s, string str_re, string rpl) {
   return true;
}

bool tools::replace_all(string &s, string str_re, string rpl) {
   return true;
}
