/*
 * truecolor.h: true color (16 million color) demo
 */

/* script version */
#define VER truecolor("50", "200", "0") "0" truecolor("100", "150", "0") "." truecolor("150", "100", "0") "1" truecolor("200", "50", "0") "." truecolor("255", "0", "0") "0"

/* truecolor escape sequence */
#define truecolor(r, g, b) ESC "38;2;" r ";" g ";" b "m"

/* Header text displayed at top of screen next to progress bar
 *
 * If you would like to define your own greeting text which is determined at compile-time,
 *  define DO_NOT_DEFINE_GREETING, GREETING, and GREETING_LEN on the command line --
 *  refer to https://github.com/Cubified/fpinit/blob/master/Makefile for an example
 */
#ifndef DO_NOT_DEFINE_GREETING
#  ifdef IS_DEBUG_BUILD
#    define GREETING truecolor("0", "0", "255") "script " truecolor("0", "255", "0") "v" VER truecolor("255", "0", "255") " (debug)"
#    define GREETING_LEN  21
#  else
#    define GREETING truecolor("0", "0", "255") "s" truecolor("0", "55", "200") "c" truecolor("0", "100", "100") "r" truecolor("0", "150", "50") "i" truecolor("0", "200", "0") "p" truecolor("0", "255", "0") "t " truecolor("0", "255", "0") "v" VER
#    define GREETING_LEN  13
#  endif /* IS_DEBUG_BUILD */
#endif /* DO_NOT_DEFINE_GREETING */

/*
 * Disable color output
 */
// #define NO_COLOR

/*
 * UI contextual definitions
 */

/* Colors */
#define COLOR_DEFAULT RESET

#define COLOR_FAILURE truecolor("255", "0", "0")
#define COLOR_WARNING truecolor("255", "255", "0")
#define COLOR_SUCCESS truecolor("0", "255", "0")

#define COLOR_ACCENT_WEAK_1   truecolor("0", "0", "255")
#define COLOR_ACCENT_WEAK_2   truecolor("0", "255", "255")
#define COLOR_ACCENT_STRONG   truecolor("255", "0", "255")

#define COLOR_PROGRESS_COMPLETE   COLOR_SUCCESS
#define COLOR_PROGRESS_INCOMPLETE COLOR_ACCENT_WEAK_1

/* Characters */
#define CHAR_PROGRESS_COMPLETE '_'
#define CHAR_PROGRESS_INCOMPLETE '_'

#define CHAR_JOB_SPACER '_'
#define CHAR_JOB_BRACKET_LEFT '<'
#define CHAR_JOB_BRACKET_RIGHT '>'

/* Strings */
#define TEXT_HEADER_LEFTCAP  "> "
#define TEXT_HEADER_RIGHTCAP "_"
#define LEN_HEADER_LEFTCAP 2
#define LEN_HEADER_RIGHTCAP 1

#define TEXT_JOB_STATUS_RUNNING "wait"
#define TEXT_JOB_STATUS_SUCCESS "good"
#define TEXT_JOB_STATUS_FAILURE "fail"
#define LEN_JOB_STATUS_TEXT 4
