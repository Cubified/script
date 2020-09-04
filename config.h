/*
 * config.h: script configuration
 */

/* script version */
#define VER "0.1.0"

/* Header text displayed at top of screen next to progress bar */
#ifndef DO_NOT_DEFINE_GREETING
#  ifdef IS_DEBUG_BUILD
#    define GREETING BLUE "script " GREEN "v" VER MAGENTA " (debug)"
#    define GREETING_LEN  21
#  else
#    define GREETING BLUE "script " GREEN "v" VER
#    define GREETING_LEN  13
#  endif /* IS_DEBUG_BUILD */
#endif /* DO_NOT_DEFINE_GREETING */

/* Disable color output */
/* #define NO_COLOR */
