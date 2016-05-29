/*
    $Id$

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include "misc.h"
#include <string.h>
#include "64tass.h"
#include "opcodes.h"
#include "getopt.h"
#include "section.h"
#include "longjump.h"
#include "encoding.h"
#include "file.h"
#include "eval.h"
#include "variables.h"
#include "ternary.h"
#include "unicode.h"
#include "error.h"
#include "values.h"

#include "codeobj.h"
#include "namespaceobj.h"

struct arguments_s arguments = {true,true,true,false,true,true,false,false,false,false,false,0x20,"a.out",&c6502,NULL,NULL,NULL,NULL, OUTPUT_CBM, 8, LABEL_64TASS};

/* --------------------------------------------------------------------------- */
int str_hash(const str_t *s) {
    size_t l = s->len;
    const uint8_t *s2 = s->data;
    unsigned int h;
    if (l == 0) return 0;
    h = *s2 << 7;
    while ((l--) != 0) h = (1000003 * h) ^ *s2++;
    h ^= s->len;
    return h & ((~(unsigned int)0) >> 1);
}

int str_cmp(const str_t *s1, const str_t *s2) {
    if (s1->len != s2->len) return s1->len - s2->len;
    if (s1->data == s2->data) return 0;
    return memcmp(s1->data, s2->data, s1->len);
}

void str_cfcpy(str_t *s1, const str_t *s2) {
    size_t i, l;
    const uint8_t *d;
    static str_t cache;
    if (s2 == NULL) {
        if (s1 != NULL) {
            if (s1->len != cache.len) {
                s1->data = (uint8_t *)reallocx((uint8_t *)s1->data, s1->len);
            }
        } else free((uint8_t *)cache.data);
        memset(&cache, 0, sizeof(cache));
        return;
    }
    l = s2->len; d = s2->data;
    if (arguments.caseinsensitive == 0) {
        for (i = 0; i < l; i++) {
            if ((d[i] & 0x80) != 0) {
                unfkc(&cache, s2, 0);
                s1->len = cache.len;
                s1->data = cache.data;
                return;
            }
        }
        s1->len = l;
        s1->data = d;
        return;
    }
    for (i = 0; i < l; i++) {
        uint8_t *s, ch = d[i];
        if (ch < 'A' || (ch > 'Z' && ch < 0x80)) continue;
        if ((ch & 0x80) != 0) {
            unfkc(&cache, s2, 1);
            s1->len = cache.len;
            s1->data = cache.data;
            return;
        }
        if (l > cache.len) {
            cache.data = (uint8_t *)reallocx((uint8_t *)cache.data, l);
            cache.len = l;
        }
        s = (uint8_t *)cache.data;
        if (i != 0) memcpy(s, d, i);
        s1->data = s;
        for (; i < l; i++) {
            ch = d[i];
            if (ch < 'A') {
                s[i] = ch;
                continue;
            }
            if (ch <= 'Z') {
                s[i] = ch | 0x20;
                continue;
            }
            if ((ch & 0x80) != 0) {
                unfkc(&cache, s2, 1);
                s1->len = cache.len;
                s1->data = cache.data;
                return;
            }
            s[i] = ch;
        }
        s1->len = l;
        return;
    }
    s1->len = l;
    s1->data = d;
}

void str_cpy(str_t *s1, const str_t *s2) {
    s1->len = s2->len;
    if (s2->data != NULL) {
        uint8_t *s = (uint8_t *)mallocx(s2->len);
        memcpy(s, s2->data, s2->len);
        s1->data = s;
    } else s1->data = NULL;
}

linecpos_t calcpos(const uint8_t *line, size_t pos, bool utf8) {
    size_t s, l;
    if (utf8) return pos + 1;
    s = l = 0;
    while (s < pos) {
        if (line[s] == 0) break;
        s += utf8len(line[s]);
        l++;
    }
    return l + 1;
}

void tfree(void) {
    destroy_eval();
    destroy_variables();
    destroy_section();
    destroy_longjump();
    destroy_file();
    err_destroy();
    destroy_encoding();
    destroy_values();
    destroy_namespacekeys();
    destroy_ternary();
    unfc(NULL);
    unfkc(NULL, NULL, 0);
    str_cfcpy(NULL, NULL);
}

void tinit(const char *prgname) {
    init_values();
    objects_init();
    err_init(prgname);
    init_section();
    init_file();
    init_variables();
    init_eval();
    init_ternary();
}

/* ------------------------------------------------------------------ */
static const char *short_options = "wqnbfXaTCBicxtel:L:I:M:msV?o:D:E:";

static const struct option long_options[] = {
    {"no-warn"          , no_argument      , NULL, 'w'},
    {"quiet"            , no_argument      , NULL, 'q'},
    {"nonlinear"        , no_argument      , NULL, 'n'},
    {"nostart"          , no_argument      , NULL, 'b'},
    {"flat"             , no_argument      , NULL, 'f'},
    {"long-address"     , no_argument      , NULL, 'X'},
    {"atari-xex"        , no_argument      , NULL,  0x107},
    {"apple-ii"         , no_argument      , NULL,  0x108},
    {"intel-hex"        , no_argument      , NULL,  0x10e},
    {"s-record"         , no_argument      , NULL,  0x10f},
    {"ascii"            , no_argument      , NULL, 'a'},
    {"tasm-compatible"  , no_argument      , NULL, 'T'},
    {"case-sensitive"   , no_argument      , NULL, 'C'},
    {"long-branch"      , no_argument      , NULL, 'B'},
    {"m65xx"            , no_argument      , NULL,  0x101},
    {"m6502"            , no_argument      , NULL, 'i'},
    {"m65c02"           , no_argument      , NULL, 'c'},
    {"m65ce02"          , no_argument      , NULL,  0x106},
    {"m65816"           , no_argument      , NULL, 'x'},
    {"m65dtv02"         , no_argument      , NULL, 't'},
    {"m65el02"          , no_argument      , NULL, 'e'},
    {"mr65c02"          , no_argument      , NULL,  0x104},
    {"mw65c02"          , no_argument      , NULL,  0x105},
    {"m4510"            , no_argument      , NULL,  0x111},
    {"labels"           , required_argument, NULL, 'l'},
    {"output"           , required_argument, NULL, 'o'},
    {"error"            , required_argument, NULL, 'E'},
    {"vice-labels"      , no_argument      , NULL,  0x10b},
    {"dump-labels"      , no_argument      , NULL,  0x10d},
    {"shadow-check"     , no_argument      , NULL,  0x10c},
    {"list"             , required_argument, NULL, 'L'},
    {"verbose-list"     , no_argument      , NULL,  0x110},
    {"no-monitor"       , no_argument      , NULL, 'm'},
    {"no-source"        , no_argument      , NULL, 's'},
    {"no-caret-diag"    , no_argument      , NULL,  0x10a},
    {"tab-size"         , required_argument, NULL,  0x109},
    {"version"          , no_argument      , NULL, 'V'},
    {"usage"            , no_argument      , NULL,  0x102},
    {"help"             , no_argument      , NULL,  0x103},
    {NULL, no_argument, NULL, 0}
};

int testarg(int argc,char *argv[], struct file_s *fin) {
    int opt, longind, tab;
    size_t max_lines = 0, fp = 0;

    while ((opt = getopt_long(argc, argv, short_options, long_options, &longind)) != -1) {
        switch (opt) {
            case 'w':arguments.warning = false;break;
            case 'q':arguments.quiet = false;break;
            case 'X':arguments.longaddr = true;break;
            case 'n':arguments.output_mode = OUTPUT_NONLINEAR;break;
            case 0x107:arguments.output_mode = OUTPUT_XEX;break;
            case 0x108:arguments.output_mode = OUTPUT_APPLE;break;
            case 0x10e:arguments.output_mode = OUTPUT_IHEX;break;
            case 0x10f:arguments.output_mode = OUTPUT_SREC;break;
            case 'b':arguments.output_mode = OUTPUT_RAW;break;
            case 'f':arguments.output_mode = OUTPUT_FLAT;break;
            case 'a':arguments.toascii = true;break;
            case 'T':arguments.tasmcomp = true;break;
            case 'o':arguments.output = optarg;break;
            case 0x10a:arguments.caret = false;break;
            case 'D':
                {
                    size_t len = strlen(optarg) + 1;

                    if (fin->lines >= max_lines) {
                        max_lines += 1024;
                        if (/*max_lines < 1024 ||*/ max_lines > SIZE_MAX / sizeof(fin->line[0])) err_msg_out_of_memory(); /* overflow */
                        fin->line = (size_t *)reallocx(fin->line, max_lines * sizeof(fin->line[0]));
                    }
                    fin->line[fin->lines++] = fp;

                    if (len < 1 || fp + len < len) err_msg_out_of_memory();
                    if (fp + len > fin->len) {
                        fin->len = fp + len + 1024;
                        if (fin->len < 1024) err_msg_out_of_memory();
                        fin->data = (uint8_t*)reallocx(fin->data, fin->len);
                    }
                    memcpy(fin->data + fp, optarg, len);
                    fp += len;
                }
                break;
            case 'B': arguments.longbranch = true;break;
            case 0x101: arguments.cpumode = &c6502;break;
            case 'i': arguments.cpumode = &c6502i;break;
            case 'c': arguments.cpumode = &c65c02;break;
            case 0x106: arguments.cpumode = &c65ce02;break;
            case 'x': arguments.cpumode = &w65816;break;
            case 't': arguments.cpumode = &c65dtv02;break;
            case 'e': arguments.cpumode = &c65el02;break;
            case 0x104: arguments.cpumode = &r65c02;break;
            case 0x105: arguments.cpumode = &w65c02;break;
            case 0x111: arguments.cpumode = &c4510;break;
            case 'l': arguments.label = optarg;break;
            case 0x10b: arguments.label_mode = LABEL_VICE; break;
            case 0x10c: arguments.shadow_check = true; break;
            case 0x10d: arguments.label_mode = LABEL_DUMP; break;
            case 'E': arguments.error = optarg;break;
            case 'L': arguments.list = optarg;break;
            case 'M': arguments.make = optarg;break;
            case 'I': include_list_add(optarg);break;
            case 'm': arguments.monitor = false;break;
            case 's': arguments.source = false;break;
            case 'C': arguments.caseinsensitive = 0;break;
            case 0x110: arguments.verbose = true;break;
            case 0x109:tab = atoi(optarg); if (tab > 0 && tab <= 64) arguments.tab_size = tab; break;
            case 0x102:puts(
             /* 12345678901234567890123456789012345678901234567890123456789012345678901234567890 */
               "Usage: 64tass [-abBCfnTqwWcitxmse?V] [-D <label>=<value>] [-o <file>]\n"
               "        [-E <file>] [-I <path>] [-l <file>] [-L <file>] [-M <file>] [--ascii]\n"
               "        [--nostart] [--long-branch] [--case-sensitive] [--flat] [--atari-xex]\n"
               "        [--apple-ii] [--intel-hex] [--s-record] [--nonlinear]\n"
               "        [--tasm-compatible] [--quiet] [--no-warn] [--long-address] [--m65c02]\n"
               "        [--m6502] [--m65xx] [--m65dtv02] [--m65816] [--m65el02] [--mr65c02]\n"
               "        [--mw65c02] [--m65ce02] [--m4510] [--labels=<file>] [--vice-labels]\n"
               "        [--dump-labels] [--shadow-check] [--list=<file>] [--no-monitor]\n"
               "        [--no-source] [--tab-size=<value>] [--verbose-list] [--errors=<file>]\n"
               "        [--output=<file>] [--help] [--usage] [--version] SOURCES");
                   return 0;

            case 'V':puts("64tass Turbo Assembler Macro V" VERSION);
                     return 0;
            case 0x103:
            case '?':if (optopt == '?' || opt == 0x103) { puts(
               "Usage: 64tass [OPTIONS...] SOURCES\n"
               "64tass Turbo Assembler Macro V" VERSION "\n"
               "\n"
               "  -a, --ascii           Source is not in PETASCII\n"
               "  -B, --long-branch     Automatic bxx *+3 jmp $xxxx\n"
               "  -C, --case-sensitive  Case sensitive labels\n"
               "  -D <label>=<value>    Define <label> to <value>\n"
               "  -E, --error=<file>    Place errors into <file>\n"
               "  -I <path>             Include search path\n"
               "  -M <file>             Makefile dependencies to <file>\n"
               "  -q, --quiet           Do not output summary and header\n"
               "  -T, --tasm-compatible Enable TASM compatible mode\n"
               "  -w, --no-warn         Suppress warnings\n"
               "      --no-caret-diag   Suppress source line display\n"
               "      --shadow-check    Check symbol shadowing\n"
               "\n"
               " Output selection:\n"
               "  -o, --output=<file>   Place output into <file>\n"
               "  -b, --nostart         Strip starting address\n"
               "  -f, --flat            Generate flat output file\n"
               "  -n, --nonlinear       Generate nonlinear output file\n"
               "  -X, --long-address    Use 3 byte start/len address\n"
               "      --atari-xex       Output Atari XEX file\n"
               "      --apple-ii        Output Apple II file\n"
               "      --intel-hex       Output Intel HEX file\n"
               "      --s-record        Output Motorola S-record file\n"
               "\n"
               " Target CPU selection:\n"
               "      --m65xx           Standard 65xx (default)\n"
               "  -c, --m65c02          CMOS 65C02\n"
               "      --m65ce02         CSG 65CE02\n"
               "  -e, --m65el02         65EL02\n"
               "  -i, --m6502           NMOS 65xx\n"
               "  -t, --m65dtv02        65DTV02\n"
               "  -x, --m65816          W65C816\n"
               "      --mr65c02         R65C02\n"
               "      --mw65c02         W65C02\n"
               "      --m4510           CSG 4510\n"
               "\n"
               " Source listing and labels:\n"
               "  -l, --labels=<file>   List labels into <file>\n"
               "      --vice-labels     Labels in VICE format\n"
               "      --dump-labels     Dump for debugging\n"
               "  -L, --list=<file>     List into <file>\n"
               "  -m, --no-monitor      Don't put monitor code into listing\n"
               "  -s, --no-source       Don't put source code into listing\n"
               "      --tab-size=<n>    Override the default tab size (8)\n"
               "      --verbose-list    List unused lines as well\n"
               "\n"
               " Misc:\n"
               "  -?, --help            Give this help list\n"
               "      --usage           Give a short usage message\n"
               "  -V, --version         Print program version\n"
               "\n"
               "Mandatory or optional arguments to long options are also mandatory or optional\n"
               "for any corresponding short options.\n"
               "\n"
               "Report bugs to <soci" "\x40" "c64.rulez.org>.");
               return 0;
            }
            default:fputs("Try '64tass --help' or '64tass --usage' for more information.\n", stderr);
                    return -1;
        }
    }

    switch (arguments.output_mode) {
    case OUTPUT_RAW:
    case OUTPUT_NONLINEAR:
    case OUTPUT_CBM: all_mem2 = arguments.longaddr ? 0xffffff : 0xffff; break;
    case OUTPUT_IHEX:
    case OUTPUT_SREC:
    case OUTPUT_FLAT: all_mem2 = 0xffffffff; break;
    case OUTPUT_APPLE: 
    case OUTPUT_XEX: all_mem2 = 0xffff; break;
    }
    if (dash_name(arguments.output)) arguments.quiet = false;
    if (fin->lines != max_lines) {
        fin->line = (size_t *)reallocx(fin->line, fin->lines * sizeof(fin->line[0]));
    }
    closefile(fin);
    if (fp != fin->len) {
        fin->len = fp;
        if (fin->len != 0) {
            fin->data = (uint8_t*)reallocx(fin->data, fin->len);
        }
    }
    fin->coding = E_UTF8;
    if (argc <= optind) {
        fputs("Usage: 64tass [OPTIONS...] SOURCES\n"
              "Try '64tass --help' or '64tass --usage' for more information.\n", stderr);
        return -1;
    }
    return optind;
}
