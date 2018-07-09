#ifndef HEX_DUMP_H
#define HEX_DUMP_H

#include <stdio.h> // printf, getc
#include <ctype.h> // isalpha, isdigit, ispunct
#include <stdlib.h> // exit
#include <stdint.h> // pentru int64_t

#include "config.h"
#include "colors.h"

void show_help_messages(void);
void handle_user(char *argv[], int argc);
void show_header(int offset);
void dump(char *path, int offset);
static int64_t get_file_size(FILE *fp);

#endif