#ifndef OPTIONS_H
#define OPTIONS_H
#include "config.h"
#include <getopt.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include "error.h"

extern int use_syslog;

struct options
{
  int help;
  int reset;
  int test;
  int verbose;
  int status;
  int gpio_m0;
  int gpio_m1;
  int gpio_aux;
  int mode;
  int daemon;
  int input_standard;
  int output_standard;
  int aux_transition_additional_delay;
  char tty_name[64];
  uint8_t settings_write_input[6];
  uint8_t setting_write_encryption[16];
  FILE* input_file;
  FILE* output_file;
  struct sockaddr_in socket_udp_dest;
  int fd_socket_unix_data, fd_socket_unix_control;
  struct sockaddr_un socket_unix_data, socket_unix_control;
};

void
usage(char *progname);

void
options_init(struct options *opts);

int
options_deinit(struct options *opts);

int
options_parse(struct options *opts, int argc, char *argv[]);

int
options_parse_settings(struct options *opts, char *settings);

int
options_parse_encryption(struct options *opts, char *encryption);

void
options_print(struct options *opts);

#endif
