#include "bundler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *fileToChar(char *filename) {
  char *buffer = NULL;
  long length;

  FILE *f = fopen(filename, "rb");
  if (f == NULL) {
    perror("Error opening file");
    return NULL;
  }

  if (fseek(f, 0, SEEK_END) != 0) {
    perror("Error seeking file");
    fclose(f);
    return NULL;
  }

  length = ftell(f);
  if (length == -1) {
    perror("Error getting file size");
    fclose(f);
    return NULL;
  }

  // Allocate memory for the file contents
  buffer = malloc(length);
  if (buffer == NULL) {
    perror("Error allocating memory");
    fclose(f);
    return NULL;
  }

  if (fseek(f, 0, SEEK_SET) != 0) {
    perror("Error rewinding file");
    free(buffer);
    fclose(f);
    return NULL;
  }

  size_t bytesRead = fread(buffer, 1, length, f);
  if (bytesRead != length) {
    perror("Error reading file");
    free(buffer);
    fclose(f);
    return NULL;
  }

  fclose(f);
  return buffer;
}

// https://stackoverflow.com/questions/779875/what-function-is-to-replace-a-substring-from-a-string-in-c
char *replace_with(char *orig, char *rep, char *with) {
  char *result;  // the return string
  char *ins;     // the next insert point
  char *tmp;     // varies
  int len_rep;   // length of rep (the string to remove)
  int len_with;  // length of with (the string to replace rep with)
  int len_front; // distance between rep and end of last rep
  int count;     // number of replacements

  // sanity checks and initialization
  if (!orig || !rep)
    return NULL;
  len_rep = strlen(rep);
  if (len_rep == 0)
    return NULL; // empty rep causes infinite loop during count
  if (!with)
    with = "";
  len_with = strlen(with);

  // count the number of replacements needed
  ins = orig;
  for (count = 0; (tmp = strstr(ins, rep)); ++count) {
    ins = tmp + len_rep;
  }

  tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

  if (!result)
    return NULL;

  // first time through the loop, all the variable are set correctly
  // from here on,
  //    tmp points to the end of the result string
  //    ins points to the next occurrence of rep in orig
  //    orig points to the remainder of orig after "end of rep"
  while (count--) {
    ins = strstr(orig, rep);
    len_front = ins - orig;
    tmp = strncpy(tmp, orig, len_front) + len_front;
    tmp = strcpy(tmp, with) + len_with;
    orig += len_front + len_rep; // move to next "end of rep"
  }
  strcpy(tmp, orig);
  return result;
}
