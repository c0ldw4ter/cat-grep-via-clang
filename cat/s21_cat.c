#include "cat.h"

int cat_no_args(int file_desc) {
  char buffer[SIZE];
  int bytes_was_read = 0;
  bytes_was_read = read(file_desc, buffer, SIZE);
  while (bytes_was_read > 0) {
    printf("%.*s", bytes_was_read, buffer);
    bytes_was_read = read(file_desc, buffer, SIZE);
  }
  return bytes_was_read;
}

Options cat_read_flags(int argc, char *argv[]) {
  Options flags = {0, 0, 0, 0, 0, 0};
  const struct option long_options[] = {
      {"number-nonblank", no_argument, &flags.num_nonblank_lines, 1},
      {"number", no_argument, &flags.num_all_out_lines, 1},
      {"squeeze-blank", no_argument, &flags.squeeze, 1},
      {NULL, 0, NULL, 0}};

  int current_flag = getopt_long(argc, argv, "bevEnstT", long_options, NULL);
  while (current_flag != -1) {
    switch (current_flag) {
      case 'b':
        flags.num_nonblank_lines = 1;
        break;
      case 'e':
        flags.display_endoffile = 1;
        flags.print_non_printable = 1;
        break;
      case 'v':
        flags.print_non_printable = 1;
        break;
      case 'E':
        flags.display_endoffile = 1;
        break;
      case 'n':
        flags.num_all_out_lines = 1;
        break;
      case 's':
        flags.squeeze = 1;
        break;
      case 't':
        flags.print_non_printable = 1;
        flags.display_tabs = 1;
        break;
      case 'T':
        flags.display_tabs = 1;
    }
    current_flag = getopt_long(argc, argv, "bevEnstT", long_options, NULL);
  }

  if ((flags.num_all_out_lines == 1) && (flags.num_nonblank_lines == 1)) {
    flags.num_all_out_lines = 0;
  }

  return flags;
}

void print_content(int argc, char **argv, Options flags) {
  int is_current_line_emp = 0, is_prev_line_emp = 0, counter = 0;
  char prev_char = '\n';

  for (int i = optind; i < argc; i++) {
    if (file_exist(argv[i])) {
      FILE *file = fopen(argv[i], "r");
      int current_char = fgetc(file);
      while (current_char != EOF) {
        if (current_char == '\n' && prev_char == '\n') {
          is_current_line_emp = 1;
        } else {
          is_current_line_emp = 0;
        }

        if (!(flags.squeeze && is_prev_line_emp && is_current_line_emp)) {
          if (prev_char == '\n') {
            if (flags.num_all_out_lines ||
                (flags.num_nonblank_lines && !is_current_line_emp)) {
              printf("%6d\t", ++counter);
            }
          }
          if (flags.display_endoffile && current_char == '\n') {
            printf("$\n");
          } else if (flags.display_tabs && current_char == '\t') {
            printf("^I");
          } else if (flags.print_non_printable && current_char < 32 &&
                     current_char != '\n' && current_char != '\t') {
            printf("^%c", current_char + 64);
          } else if (flags.print_non_printable && current_char == 127) {
            printf("^?");
          } else {
            printf("%c", current_char);
          }
        }
        prev_char = current_char;
        is_prev_line_emp = is_current_line_emp;
        current_char = fgetc(file);
      }
    } else {
      fprintf(stderr, "s21_cat: %s No such file or directory\n", argv[i]);
    }
  }
}

int file_exist(char *filename) {
  int result = 0;
  FILE *file = NULL;
  file = fopen(filename, "r");
  if (file != NULL) {
    result = 1;
    fclose(file);
  }
  return result;
}

int main(int argc, char **argv) {
  if (argc == 1)
    cat_no_args(STDIN_FILENO);
  else {
    Options flags = cat_read_flags(argc, argv);
    print_content(argc, argv, flags);
  }
  return 0;
}