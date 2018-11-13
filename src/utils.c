/** @file       utils.c
 *  @brief      Utilities.
 *
 *  @author     t-kenji <protect.2501@gmail.com>
 *  @date       2018-11-24 create new.
 *  @copyright  Copyright © 2018 t-kenji
 *
 *  This code is licensed under the MIT License.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#include "debug.h"
#include "utils.h"

#define PAT_ESCAPED (0x100)

static bool internal_string_match(const char *pat, const char *str, bool pat_escaped, bool str_escaped, bool case_sensitive)
{
    int pat_c = case_sensitive ? *pat : toupper(*pat);
    int str_c = case_sensitive ? *str : toupper(*str);
    bool str_esc = (*str == '\\');

    switch (pat_c | (pat_escaped ? PAT_ESCAPED : 0)) {
    case '\0':
//DEBUG("pat: '%s' %d, str: '%s' %d", pat, pat_escaped, str, str_escaped);
        return str_c == '\0';
    case '*':
//DEBUG("pat: '%s' %d, str: '%s' %d", pat, pat_escaped, str, str_escaped);
        return internal_string_match(pat + 1, str, false, str_escaped, case_sensitive)
               || ((str_c != '\0') && internal_string_match(pat, str + 1, false, str_esc, case_sensitive));
    case '*' | PAT_ESCAPED:
//DEBUG("pat: '%s' %d, str: '%s' %d", pat, pat_escaped, str, str_escaped);
        return (str_c == pat_c)
               && internal_string_match(pat + 1, str + 1, false, str_esc, case_sensitive);
    case '+':
//DEBUG("pat: '%s' %d, str: '%s' %d", pat, pat_escaped, str, str_escaped);
        return internal_string_match(pat + 1, str, false, str_escaped, case_sensitive)
               || ((str_c != '\0') && ((*(pat + 1) != str_c) || str_escaped)
                   && internal_string_match(pat, str + 1, false, str_esc, case_sensitive));
    case '+' | PAT_ESCAPED:
//DEBUG("pat: '%s' %d, str: '%s' %d", pat, pat_escaped, str, str_escaped);
        return (str_c == pat_c)
               && internal_string_match(pat + 1, str + 1, false, str_esc, case_sensitive);
    case '?':
//DEBUG("pat: '%s' %d, str: '%s' %d", pat, pat_escaped, str, str_escaped);
        return (str_c != '\0')
               && internal_string_match(pat + 1, str + 1, false, str_esc, case_sensitive);
    case '\\':
//DEBUG("pat: '%s' %d, str: '%s' %d", pat, pat_escaped, str, str_escaped);
        return internal_string_match(pat + 1, str, true, str_esc, case_sensitive);
    default:
//DEBUG("pat: '%s' %d, str: '%s' %d", pat, pat_escaped, str, str_escaped);
        return (str_c == pat_c) && (str_escaped == pat_escaped)
               && internal_string_match(pat + 1, str + 1, false, str_esc, case_sensitive);
    }
}

bool string_match(const char *pat, const char *str)
{
    bool case_sensitive = false;

    for (const char *_pat = pat; *_pat != '\0'; ++_pat) {
        if (isupper(*_pat)) {
            case_sensitive = true;
            break;
        }
    }

    return internal_string_match(pat, str, false, false, case_sensitive);
}
