# replace

replace is a tool that performs search-and-replace using pcre2 on any file names
matching file_regex.

###Usage:

```bash
replace [-option] file_regex match_regex replacement
```

###Options:

```text
   a - Replace all matches in the files that match file_regex.
   r - Search recursively for files that match file_regex.
   q - Quiet mode. Nothing will be printed to stdout.
   t - Test mode. No files will be modified.
   f - The file(s) will be input as a list.
   h - Print more information.
```

###Arguments:

```text
   file_regex

       A Perl regular expression to match a file in the current working
       directory. A directory path can be prepended to the regular expression.
       The / character is not allowed in the regular expression part.

   match_regex

       A Perl regular expression to be used for search and replace.

   replacement

       A string that will replace whatever matches match_regex. This string
       can contain the special variables #1, #2, #3, etc. These contain
       the groups set in the match_regex string.
```

###Install:

```bash
make install
```

###Notes:

   In the replacement string the # and 0 - 9 characters can be escaped with \
   to represent literal # and 0 - 9 characters.

```bash
replace ^file$ "(\d)(\d+)" "#1\00 \# #2"
```

   The -f option allows a list of file names to be input instead of using a
   perl regular expression to search for the file.

```bash
replace -f file_1 file_2 file_3... "abc" "def"
```
