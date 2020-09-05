/*
 * config.h: script configuration
 */

/* script version */
#define VER "0.1.0"

/* Header text displayed at top of screen next to progress bar
 *
 * If you would like to define your own greeting text which is determined at compile-time,
 *  define DO_NOT_DEFINE_GREETING, GREETING, and GREETING_LEN on the command line --
 *  refer to https://github.com/Cubified/fpinit/blob/master/Makefile for an example
 */
#ifndef DO_NOT_DEFINE_GREETING
#  ifdef IS_DEBUG_BUILD
#    define GREETING BLUE "script " GREEN "v" VER MAGENTA " (debug)"
#    define GREETING_LEN  21
#  else
#    define GREETING BLUE "script " GREEN "v" VER
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

#define COLOR_FAILURE RED
#define COLOR_WARNING YELLOW
#define COLOR_SUCCESS GREEN

#define COLOR_ACCENT_WEAK_1   BLUE
#define COLOR_ACCENT_WEAK_2   CYAN
#define COLOR_ACCENT_STRONG   MAGENTA

#define COLOR_PROGRESS_COMPLETE   GREEN
#define COLOR_PROGRESS_INCOMPLETE CYAN

/* Characters */
#define CHAR_PROGRESS_COMPLETE '='
#define CHAR_PROGRESS_INCOMPLETE '.'

#define CHAR_JOB_SPACER '.'
#define CHAR_JOB_BRACKET_LEFT '['
#define CHAR_JOB_BRACKET_RIGHT ']'

/* Strings */
#define TEXT_HEADER_LEFTCAP  "<= "
#define TEXT_HEADER_RIGHTCAP ">"
#define LEN_HEADER_LEFTCAP 3
#define LEN_HEADER_RIGHTCAP 1

#define TEXT_JOB_STATUS_RUNNING "  "
#define TEXT_JOB_STATUS_SUCCESS "OK"
#define TEXT_JOB_STATUS_FAILURE "!!"
#define LEN_JOB_STATUS_TEXT 2
