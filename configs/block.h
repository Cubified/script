/*
 * block.h: block-based progress bars
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

#define COLOR_PROGRESS_COMPLETE   "\x1b[42m"
#define COLOR_PROGRESS_INCOMPLETE "\x1b[0m"

/* Characters */
#define CHAR_PROGRESS_COMPLETE ' '
#define CHAR_PROGRESS_INCOMPLETE ' '

#define CHAR_JOB_SPACER ' '
#define CHAR_JOB_BRACKET_LEFT '['
#define CHAR_JOB_BRACKET_RIGHT ']'

/* Strings */
#define TEXT_HEADER_LEFTCAP  ""
#define TEXT_HEADER_RIGHTCAP ""
#define LEN_HEADER_LEFTCAP 0
#define LEN_HEADER_RIGHTCAP 0

#define TEXT_JOB_STATUS_RUNNING "██"
#define TEXT_JOB_STATUS_SUCCESS "██"
#define TEXT_JOB_STATUS_FAILURE "██"
#define LEN_JOB_STATUS_TEXT 2
