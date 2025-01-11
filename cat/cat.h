#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define SIZE 2048

typedef struct {
  int num_nonblank_lines;   // b
  int display_endoffile;    // e
  int num_all_out_lines;    // n
  int squeeze;              // s
  int display_tabs;         // t
  int print_non_printable;  // v
} Options;

int cat_no_args(int file_desc);
Options cat_read_flags(int argc, char *argv[]);
void print_content(int argc, char **argv, Options flags);
int file_exist(char *filename);