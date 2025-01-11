
#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERROR_FILE_NOT_FOUND "s21_grep: %s: No such file or directory\n"

typedef struct {
  int pattern;             // e
  int ignoreCase;          // i
  int invertMatch;         // v
  int countOnlyMatched;    // c
  int onlyMatchedFiles;    // l
  int lineNumber;          // n
  int noFileName;          // h
  int suppressErrors;      // s
  int filePattern;         // f
  int outputMatchedParts;  // o
  int countFiles;
  int numberLine;
  int countMatches;
} Options;

int grep_rd_flags(int argc, char **argv, Options *config, char **template);
void grep(char *name, Options config, char *tmpl);

void conf_f(Options *config, char **template, char *optarg);
void conf_e(Options *config, char **template, char *optarg);
void conf_o(regex_t compiledRegex, char *str, Options config);
void prt_main_info(char *str, Options *options, char *tmpl, char *name);
void prt_aux_info(Options config, char *path);
void *incr_str_len(void *str, int size);
int create_tmpl(char **str, char *optarg);
void setup_conf(Options *config, int argc);
void add_tmpl(char **str, char *optarg);
