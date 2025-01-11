#include "grep.h"

int grep_rd_flags(int argc, char **argv, Options *config, char **template) {
  for (int currentFlag = 0;
       (currentFlag = getopt(argc, argv, "e:ivclnhsf:o")) != (-1);) {
    switch (currentFlag) {
      case 'i':
        config->ignoreCase = 1;
        break;
      case 'v':
        config->invertMatch = 1;
        break;
      case 'c':
        config->countOnlyMatched = 1;
        break;
      case 'l':
        config->onlyMatchedFiles = 1;
        break;
      case 'n':
        config->lineNumber = 1;
        break;
      case 'h':
        config->noFileName = 1;
        break;
      case 's':
        config->suppressErrors = 1;
        break;
      case 'o':
        config->outputMatchedParts = 1;
        break;
      case 'e':
        conf_e(config, template, optarg);
        break;
      case 'f':
        conf_f(config, template, optarg);
        break;
    }
  }
  if ((config->pattern || config->filePattern) == 0) {
    create_tmpl(template, argv[optind]);
    optind++;
  }
  setup_conf(config, argc);
  return optind;
}

void grep(char *path, Options config, char *template) {
  FILE *file = fopen(path, "r");
  if (file != NULL) {
    for (char character = '0'; (character = getc(file)) != EOF;) {
      char *line = calloc(256, 1);
      config.numberLine++;
      int length = 0;

      for (line[length] = '\0'; character != EOF && character != '\n';
           character = getc(file)) {
        line[length] = character;
        line[++length] = '\0';
        if (length % 255 == 0) line = incr_str_len(line, length + 256);
      }
      prt_main_info(line, &config, template, path);
      free(line);
    }
    prt_aux_info(config, path);
    fclose(file);
  } else if (config.suppressErrors == 0) {
    fprintf(stderr, ERROR_FILE_NOT_FOUND, path);
  }
}

void conf_f(Options *config, char **template, char *optarg) {
  FILE *file = fopen(optarg, "r");
  if (file != NULL) {
    for (char character = '0'; (character = getc(file)) != EOF;) {
      int length = 0;
      char *line = calloc(256, 1);

      for (line[length] = '\0'; character != EOF && character != '\n';
           character = getc(file)) {
        line[length] = character;
        line[++length] = '\0';
        if (length % 255 == 0) line = (char *)incr_str_len(line, length + 256);
      }

      if (!(config->pattern || config->filePattern))
        config->filePattern = create_tmpl(template, line);
      else
        add_tmpl(template, line);
      free(line);
    }
    fclose(file);
  } else if (config->suppressErrors == 0) {
    fprintf(stderr, ERROR_FILE_NOT_FOUND, optarg);
  }
}

void conf_e(Options *config, char **template, char *optarg) {
  if (!(config->pattern || config->filePattern))
    config->pattern = create_tmpl(template, optarg);
  else
    add_tmpl(template, optarg);
}

void prt_aux_info(Options config, char *path) {
  if (config.countOnlyMatched) {
    if (config.onlyMatchedFiles) {
      config.countFiles > 1 ? printf("%s:1\n", path) : printf("1\n");
    } else {
      if (config.countFiles > 1) printf("%s:", path);
      printf("%i\n", config.countMatches);
    }
  }
  if (config.onlyMatchedFiles && config.countMatches) printf("%s\n", path);
}

void prt_main_info(char *line, Options *config, char *template, char *path) {
  regex_t regex;
  if (regcomp(&regex, template,
              config->ignoreCase ? REG_ICASE : REG_EXTENDED) == 0) {
    if (regexec(&regex, line, 0, NULL, 0) == config->invertMatch) {
      config->countMatches++;
      if ((config->countOnlyMatched || config->onlyMatchedFiles) == 0) {
        if (config->countFiles > 1 && !(config->noFileName))
          printf("%s:", path);
        if (config->lineNumber) printf("%i:", config->numberLine);
        if (!config->outputMatchedParts)
          printf("%s\n", line);
        else
          conf_o(regex, line, *config);
      }
    }
    regfree(&regex);
  }
}

void conf_o(regex_t regex, char *line, Options config) {
  while (regexec(&regex, line, 0, NULL, 0) == config.invertMatch) {
    char *aux = (char *)calloc(strlen(line) + 1, 1);
    strcpy(aux, line);
    int end = strlen(line);
    while (regexec(&regex, aux, 0, NULL, 0) == config.invertMatch) {
      end--;
      aux[strlen(aux) - 1] = 0;
    }
    aux[strlen(aux)] = line[strlen(aux)];
    int start = 0;
    while (regexec(&regex, aux, 0, NULL, 0) == config.invertMatch &&
           strlen(aux) > 0) {
      start++;
      int j = 0;
      while (aux[j] != 0) {
        aux[j] = aux[j + 1];
        j++;
      }
    }
    start--;
    int i = strlen(aux);
    while (i != 0) {
      aux[i] = aux[i - 1];
      i--;
    }
    aux[0] = line[start];
    printf("%s\n", aux);
    free(aux);
    i = start + 1;
    while (line[i] != 0) {
      line[i - start - 1] = line[i];
      i++;
    }
    line[i - start - 1] = 0;
  }
}

void setup_conf(Options *config, int argc) {
  if (config->outputMatchedParts &&
      (config->onlyMatchedFiles || config->invertMatch ||
       config->countOnlyMatched))
    config->outputMatchedParts = 0;
  config->countFiles = argc - optind;
}

int create_tmpl(char **str, char *optarg) {
  *str = calloc(strlen(optarg) + 1, 1);
  if (*str) strcpy(*str, optarg);
  return str ? 1 : 0;
}

void add_tmpl(char **str, char *optarg) {
  *str = incr_str_len(*str, strlen(*str) + strlen(optarg) + 2);
  if (*str) strcat(strcat(*str, "|"), optarg);
}

void *incr_str_len(void *str, int size) {
  char *aux = realloc(str, size);
  return aux;
}
int main(int argc, char **argv) {
  if (argc > 1) {
    char *tmpl = NULL;
    Options config = {0};

    for (int i = grep_rd_flags(argc, argv, &config, &tmpl); i < argc; i++)
      grep(argv[i], config, tmpl);

    if (tmpl != NULL) free(tmpl);
  }

  return 0;
}