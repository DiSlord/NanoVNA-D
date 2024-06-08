/*
 * Copyright (c) 2019-2024, Dmitry (DiSlord) dislordlive@gmail.com
 * Based on TAKAHASHI Tomohiro (TTRFTECH) edy555@gmail.com
 * All rights reserved.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * The software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */
#include <stdint.h>
#include <stdbool.h>

// Use macro, std isdigit more big
#define _isdigit(c) (c >= '0' && c <= '9')
// Rewrite universal standart str to value functions to more compact
//
// Convert string to int32
int32_t my_atoi(const char *p) {
  int32_t value = 0;
  uint32_t c;
  bool neg = false;

  if (*p == '-') {neg = true; p++;}
  if (*p == '+') p++;
  while ((c = *p++ - '0') < 10)
    value = value * 10 + c;
  return neg ? -value : value;
}

// Convert string to uint32
//  0x - for hex radix
//  0o - for oct radix
//  0b - for bin radix
//  default dec radix
uint32_t my_atoui(const char *p) {
  uint32_t value = 0, radix = 10, c;
  if (*p == '+') p++;
  if (*p == '0') {
    switch (p[1]) {
      case 'x': radix = 16; break;
      case 'o': radix =  8; break;
      case 'b': radix =  2; break;
      default:  goto calculate;
    }
    p+=2;
  }
calculate:
  while (1) {
    c = *p++ - '0';
    // c = to_upper(*p) - 'A' + 10
    if (c >= 'A' - '0') c = (c&(~0x20)) - ('A' - '0') + 10;
    if (c >= radix) return value;
    value = value * radix + c;
  }
}

float my_atof(const char *p) {
  int neg = false;
  if (*p == '-')
    neg = true;
  if (*p == '-' || *p == '+')
    p++;
  float x = my_atoi(p);
  while (_isdigit((int)*p))
    p++;
  if (*p == '.' || *p == ',') {
    float d = 1.0f;
    p++;
    while (_isdigit((int)*p)) {
      d *= 1e-1f;
      x += d * (*p - '0');
      p++;
    }
  }
  if (*p) {
    int exp = 0;
    if (*p == 'e' || *p == 'E') exp = my_atoi(&p[1]);
    else if (*p == 'G') exp =  9; // Giga
    else if (*p == 'M') exp =  6; // Mega
    else if (*p == 'k') exp =  3; // kilo
    else if (*p == 'm') exp = -3; // milli
    else if (*p == 'u') exp = -6; // micro
    else if (*p == 'n') exp = -9; // nano
    else if (*p == 'p') exp =-12; // pico
    if (exp > 0) do {x*= 1e+1f;} while (--exp);
    if (exp < 0) do {x*= 1e-1f;} while (++exp);
  }
  return neg ? -x : x;
}

static char to_lower(char c) {return (c >='A' && c <= 'Z') ? c - 'A' + 'a' : c;}
bool strcmpi(const char *t1, const char *t2) {
  int i = 0;
  while (1) {
    char ch1 = to_lower(t1[i]), ch2 = to_lower(t2[i]);
    if (ch1 != ch2) return false;
    if (ch1 ==   0) return true;
    i++;
  }
}

static inline char* _strpbrk(char *s1, const char *s2) {
  do {
    const char *s = s2;
    do {
      if (*s == *s1) return s1;
      s++;
    } while (*s);
    s1++;
  } while(*s1);
  return s1;
}

//
// Function used for search substring v in list
// Example need search parameter "center" in "start|stop|center|span|cw" getStringIndex return 2
// If not found return -1
// Used for easy parse command arguments
int get_str_index(const char *v, const char *list) {
  int i = 0;
  while (1) {
    const char *p = v;
    while (1) {
      char c = *list;
      if (c == '|') c = 0;
      if (c == *p++) {
        // Found, return index
        if (c == 0) return i;
        list++;    // Compare next symbol
        continue;
      }
      break;  // Not equal, break
    }
    // Set new substring ptr
    while (1) {
      // End of string, not found
      if (*list == 0) return -1;
      if (*list++ == '|') break;
    }
    i++;
  }
  return -1;
}

/*
 * Split line by arguments, return arguments count
 */
int parse_line(char *line, char* args[], int max_cnt) {
  char *lp = line, c;
  const char *brk;
  uint16_t nargs = 0;
  while ((c = *lp) != 0) {                   // While not end
    if (c != ' ' && c != '\t') {             // Skipping white space and tabs.
      if (c == '"') {lp++; brk = "\""; }     // string end is next quote or end
      else          {      brk = " \t";}     // string end is tab or space or end
      if (nargs < max_cnt) args[nargs] = lp; // Put pointer in args buffer (if possible)
      nargs++;                               // Substring count
      lp = _strpbrk(lp, brk);                // search end
      if (*lp == 0) break;                   // Stop, end of input string
      *lp = 0;                               // Set zero at the end of substring
    }
    lp++;
  }
  return nargs;
}
