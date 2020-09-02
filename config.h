/*
 * config.h: script configuration
 */

/* Header text displayed at top of screen next to progress bar */
static char GREETING[] = BLUE "Welcome to " GREEN "sinit v" VER
#ifdef IS_DEBUG_BUILD
  MAGENTA " (debug)"
#endif
  BLUE " running on " YELLOW "Alpine Linux 3.12";

static int GREETING_LEN = 
#ifdef IS_DEBUG_BUILD
  60;
#else
  52;
#endif
