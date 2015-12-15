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
   t - Test mode. No files will be modified.
   h - Print more information.
```

###Arguments:

```text
   file_regex  - A Perl regular expression to match a file in the current
       working directory. A directory path can be prepended to the regular
       expression. the / character is not allowed in the regular expression
       part.
   match_regex - A Perl regular expression to be used for search and replace.
   replacement - A string that will replace whatever matches match_regex. This
       string can contain the special variables #1, #2, #3, etc. These contain
       the groups set in the match_regex string. ex match_regex "(.)"
       replacement "#1" replaces each character in the file.
```
