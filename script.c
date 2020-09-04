/*
 * script.c: general script dispatcher
 *
 * Usage: script [dir]
 *   Where dir is a folder containing init scripts to be run,
 *   such as test_scripts/
 * 
 * TODO:
 *  - Properly handle overflow (i.e. when more lines are printed than are present onscreen)
 *  - stat() files to check for executable bit
 *  - pthreads support
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <dirent.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>

//////////////////////////////
// PREPROCESSOR
//
#define ESC "\x1b["

#define RED     ESC "31m"
#define GREEN   ESC "32m"
#define YELLOW  ESC "33m"
#define BLUE    ESC "34m"
#define MAGENTA ESC "35m"
#define CYAN    ESC "36m"
#define RESET   ESC "0m"

#define IS_NOT_DIR(name) ((name[0] != '.' && name[1] != '\0') && \
                          (name[0] != '.' && name[1] != '.' && name[2] != '\0'))

#define IS_NOT_TITLE(name) strcmp(name, "title") != 0

#define IS_IN_RUNLEVEL(name) (runlevel <= 9 ? name[0] == '0'+runlevel : (name[0]-'0' > 9 || name[0]-'0' < 0))

//////////////////////////////
// CONFIG
//
#include "config.h"

#ifdef NO_COLOR
#  undef  RED
#  undef  GREEN
#  undef  YELLOW
#  undef  BLUE
#  undef  MAGENTA
#  undef  CYAN
#  undef  RESET

#  define RED     ""
#  define GREEN   ""
#  define YELLOW  ""
#  define BLUE    ""
#  define MAGENTA ""
#  define CYAN    ""
#  define RESET   ""
#endif

//////////////////////////////
// ENUMS AND TYPEDEFS
//
enum script_state {
  STATE_RUNNING=-1,
  STATE_SUCCESS=0,
  STATE_FAILURE=1
};

typedef struct child {
  char *name;
  pid_t pid;
  int cur_y;
} child;

//////////////////////////////
// STATIC DEFINITIONS
//
void sighandler(int signo);
void esc_clear();
void esc_set_cursor(int x, int y);
void esc_toggle_cursor(int state);
void ui_init();
void ui_fill(int len, char c, char *color);
void ui_header();
void ui_job(char *title, int state);
void ui_runlevel(int runlevel);
void pid_init();
void pid_add(char *name, pid_t pid);
child *pid_find(pid_t pid);
char *dir_title(char *filename);
int dir_scan(char *dir);

//////////////////////////////
// GLOBAL VARIABLES
//
int ui_w, ui_h,
    ui_x, ui_y;
int job_count = 0,
    job_count_runlevel = 0,
    job_ind = 0;
volatile sig_atomic_t job_compl = 0;
int n_failures = 0;
char *title_buf;
double progress;
clock_t start;

child **active_pids;

//////////////////////////////
// SIGNALS
//
void sighandler(int signo){
  pid_t pid;
  int status;
  child *proc;

  if(signo == SIGINT ||
     signo == SIGTERM ||
     signo == SIGQUIT){
    free(active_pids);
    free(title_buf);
    esc_toggle_cursor(1);

    printf(COLOR_WARNING "\n\nHalting prematurely after " COLOR_ACCENT_STRONG "%f" COLOR_WARNING " seconds with " COLOR_ACCENT_WEAK_1 "%i/%i" COLOR_WARNING " completed jobs.\n" COLOR_DEFAULT, ((double)(clock()-start))/CLOCKS_PER_SEC, job_compl, job_count);

    exit(0);
  } else if((pid = waitpid(-1, &status, WNOHANG)) != -1){
    if((proc=pid_find(pid)) != NULL){
      job_compl++;
      if(status != STATE_SUCCESS){
        n_failures++;
      }

      ui_header();
      esc_set_cursor(ui_x, proc->cur_y);
      ui_job(proc->name, status!=STATE_SUCCESS);
      esc_set_cursor(ui_x, ui_y);
      
      free(proc->name);
      free(proc);
    }
  }
}

//////////////////////////////
// ESCAPE SEQUENCES
//
void esc_clear(){
  puts(ESC "2J");
}

void esc_set_cursor(int x, int y){
  printf(ESC "%i;%iH", y, x);
}

void esc_toggle_cursor(int state){
  printf(ESC "?25%c", (state == 1 ? 'h' : 'l'));
}

//////////////////////////////
// UI
//
void ui_init(){
  struct winsize s;

  esc_clear();
  esc_toggle_cursor(0);

  ioctl(1, TIOCGWINSZ, &s);
  ui_w = s.ws_col;
  ui_h = s.ws_row;

  ui_x = 0;
  ui_y = 1;

  progress = 0;
}

void ui_fill(int len, char c, char *color){
  int i;
  printf("%s", color);
  for(i=0;i<len;i++){
    printf("%c", c);
  }
  printf("%s", COLOR_DEFAULT);
}

void ui_header(){
  int new_w, prog_len;

  progress = (double)job_compl/(double)(job_count==0?1:job_count);
  new_w = ui_w-GREETING_LEN-LEN_HEADER_LEFTCAP-LEN_HEADER_RIGHTCAP-1;
  prog_len = round(new_w*progress);

  esc_set_cursor(0, 0);

  printf(COLOR_ACCENT_WEAK_2 TEXT_HEADER_LEFTCAP GREETING COLOR_ACCENT_WEAK_2 " ");
  ui_fill(prog_len, CHAR_PROGRESS_COMPLETE, COLOR_PROGRESS_COMPLETE);
  ui_fill(new_w-prog_len, CHAR_PROGRESS_INCOMPLETE, COLOR_PROGRESS_INCOMPLETE);
  printf("%s" TEXT_HEADER_RIGHTCAP COLOR_DEFAULT, progress==1?COLOR_PROGRESS_COMPLETE:COLOR_PROGRESS_INCOMPLETE);
}

void ui_job(char *title, int state){
  char *state_text;

  printf(" %s%s ", (state == STATE_FAILURE ? COLOR_FAILURE : (state == STATE_SUCCESS ? COLOR_SUCCESS : COLOR_DEFAULT)), title);
  ui_fill(ui_w-strlen(title)-LEN_JOB_STATUS_TEXT-7, CHAR_JOB_SPACER, COLOR_ACCENT_WEAK_2);

  switch(state){
    case STATE_RUNNING:
      state_text = "  ";
      ui_y++;
      break;
    case STATE_SUCCESS:
      state_text = COLOR_SUCCESS TEXT_JOB_STATUS_SUCCESS COLOR_DEFAULT;
      break;
    case STATE_FAILURE:
      state_text = COLOR_FAILURE TEXT_JOB_STATUS_FAILURE COLOR_DEFAULT;
      break;
    default:
      return;
  }

  printf(COLOR_ACCENT_WEAK_1 " %c %s " COLOR_ACCENT_WEAK_1 "%c\n" COLOR_DEFAULT, CHAR_JOB_BRACKET_LEFT, state_text, CHAR_JOB_BRACKET_RIGHT);
}

void ui_runlevel(int runlevel){
  printf(COLOR_WARNING "\n\nEntering runlevel %i\n" COLOR_DEFAULT, runlevel);
  ui_y += 2;
}

//////////////////////////////
// CHILD PROCESSES
//
void pid_init(){
  active_pids = malloc(job_count*sizeof(child*));
}

void pid_add(char *name, pid_t pid){
  active_pids[job_ind] = malloc(sizeof(child));
  active_pids[job_ind]->name = name;
  active_pids[job_ind]->pid = pid;
  active_pids[job_ind++]->cur_y = ui_y;
  job_count_runlevel++;
}

child *pid_find(pid_t pid){
  int i;
  for(i=0;i<job_count;i++){
    if(active_pids[i] != NULL &&
       active_pids[i]->pid == pid){
      return active_pids[i];
    }
  }
  return NULL;
}

char *dir_title(char *filename){
  char *substr, out[256];
  
  substr = strstr(title_buf, filename);

  if(substr != NULL &&
     sscanf(substr+strlen(filename)+2, "%[^\n]", out) > 0){
    return strdup(out);
  } else {
    return strdup(filename);
  }
}

int dir_scan(char *dir){
  DIR *dp;
  FILE *fp;
  int fd, i,
      runlevel = 0,
      has_printed_runlevel = 0;
  pid_t pid;
  char buf[512], *tmp;
  struct dirent *ep;
  dp = opendir(dir);

  if(dp != NULL){
    start = clock();

    while((ep = readdir(dp))){
      if(IS_NOT_DIR(ep->d_name) &&
         IS_NOT_TITLE(ep->d_name)){
        job_count++;
      } else if(!(IS_NOT_TITLE(ep->d_name))){
        sprintf(buf, "%s/%s", dir, ep->d_name);
        fp = fopen(buf, "r");
        if(fp == NULL){
          printf(COLOR_FAILURE "Error: failed to open file \"" COLOR_ACCENT_STRONG "title" COLOR_FAILURE "\" for reading.\n");
          return 1;
        }
        fseek(fp, 0, SEEK_END);
        i = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        title_buf = malloc(i+1);
        fread(title_buf, i, 1, fp);
        fclose(fp);
      }
    }
    closedir(dp);
    pid_init();

    while(runlevel <= 10){
      while(job_compl < job_count_runlevel);

      dp = opendir(dir);
      while((ep = readdir(dp))){
        if(IS_NOT_DIR(ep->d_name) &&
           IS_NOT_TITLE(ep->d_name) &&
           IS_IN_RUNLEVEL(ep->d_name)){
          if((pid=fork()) == 0){
            sprintf(buf, "%s/%s", dir, ep->d_name);

#ifndef IS_DEBUG_BUILD
            fd = open("/dev/null", O_WRONLY);
            dup2(fd, 1);
            dup2(fd, 2);
            close(fd);
#endif

            execv(buf, NULL);
          } else {
            if(!has_printed_runlevel){
              ui_runlevel(runlevel);
              has_printed_runlevel = 1;
            }
            tmp = dir_title(ep->d_name);
            ui_job(tmp, STATE_RUNNING);
            pid_add(tmp, pid);
          }
        }
      }
      closedir(dp);

      runlevel++;
      has_printed_runlevel = 0;
    }

    while(job_compl < job_count);

    free(active_pids);
    free(title_buf);

    esc_toggle_cursor(1);

    printf(COLOR_ACCENT_WEAK_2 "\n\nDone in " COLOR_ACCENT_STRONG "%f" COLOR_ACCENT_WEAK_2 " seconds with %s%i" COLOR_ACCENT_WEAK_2 " failed job%s.\n" COLOR_DEFAULT, ((double)(clock()-start))/CLOCKS_PER_SEC, (n_failures==0?COLOR_SUCCESS:COLOR_FAILURE), n_failures, (n_failures==1?"":"s"));

    return 0;
  } else {
    printf(COLOR_FAILURE "Unable to open directory \"" COLOR_ACCENT_STRONG "%s" COLOR_FAILURE "\" for reading.\n" COLOR_DEFAULT, dir);
    return 1;
  }
}

//////////////////////////////
// MAIN
//
int main(int argc, char **argv){
  if(argc < 2){
    printf(COLOR_ACCENT_WEAK_2 "script: a generic script dispatcher\n" COLOR_WARNING "Usage:" COLOR_SUCCESS " script " COLOR_ACCENT_STRONG "[directory]\n" COLOR_ACCENT_WEAK_1 "  Runs every executable file in " COLOR_ACCENT_STRONG "[directory]" COLOR_ACCENT_WEAK_1 ", grouped by runlevel.\n" COLOR_ACCENT_WEAK_2 "  Runlevel is determined by the first character of each file's name.\n" COLOR_DEFAULT);
    return 0;
  }

  signal(SIGCHLD, sighandler);
  signal(SIGINT,  sighandler);
  signal(SIGTERM, sighandler);
  signal(SIGQUIT, sighandler);

  ui_init();
  ui_header();

  return dir_scan(argv[1]);
}
