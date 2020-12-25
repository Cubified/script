/*
 * alt1.h: alternative script configuration 1
 */

/* script version */
#define VER "0.1.0"

/* Header text displayed at top of screen next to progress bar
 *
 * If you would like to define your own greeting text which is determined at runtime,
 *  define DO_NOT_DEFINE_GREETING and GREETING_CMD on the command line
 *
 * Refer to https://github.com/Cubified/fpinit/blob/master/Makefile for an example
 */
#ifndef DO_NOT_DEFINE_GREETING
#  ifdef IS_DEBUG_BUILD
#    define GREETING_CMD "printf '" BLUE "script " GREEN "v" VER MAGENTA " (debug)'"
#  else
#    define GREETING_CMD "printf '" BLUE "script " GREEN "v" VER "'"
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
#define COLOR_PROGRESS_INCOMPLETE BLUE

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
