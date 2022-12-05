#include "config.h"
#include <stdio.h>

#include <signal.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "become_daemon.h"
#include "error.h"
#include "as32.h"
#include "gpio.h"
#include "options.h"

struct options opts;
struct AS32 dev;

static
void signal_handler(int sig)
{
  int exit_status;

  if(opts.daemon)
    info_output("daemon stopping pid=%d sig=%d", getpid(), sig);

  options_deinit(&opts);
  exit_status = as32_deinit(&dev, &opts);
  exit(exit_status);
}

int
main(int argc, char *argv[])
{
  int err = 0;

  if (signal(SIGINT, signal_handler) == SIG_ERR)
    err_output("installing SIGNT signal handler");
  if (signal(SIGTERM, signal_handler) == SIG_ERR)
    err_output("installing SIGTERM signal handler");

  options_init(&opts);
  err = options_parse(&opts, argc, argv);
  if(err || opts.help)
  {
    usage(argv[0]);
    return err;
  }

  if(opts.verbose)
  {
    options_print(&opts);
  }

  err = as32_init(&dev, &opts);
  if(err)
  {
    err_output("unable to initialize the as32");
    goto cleanup;
  }

  if(opts.mode != -1)
  {
    err |= as32_set_mode(&dev, opts.mode);
    goto cleanup;
  }

  if(opts.test)
  {
    uint8_t buf[512];
    for(int i=0; i<512; i++) buf[i] = i;
    as32_transmit(&dev, buf, 512);
    goto cleanup;
  }
  if(opts.reset)
  {
    err |= as32_set_mode(&dev, SLEEP);
    err |= as32_cmd_reset(&dev);
    err |= as32_set_mode(&dev, NORMAL);
    goto cleanup;
  }

  /* must be in sleep mode to read or write settings */
  if(opts.status || opts.settings_write_input[0] || opts.setting_write_encryption[0])
  {
    if(as32_set_mode(&dev, SLEEP))
    {
      err_output("unable to go to sleep mode\n");
      goto cleanup;
    }
  }

  if(opts.status)
  {
    if(as32_cmd_read_version(&dev))
    {
      err_output("unable to read version\n");
      goto cleanup;
    }
    if(as32_cmd_read_settings(&dev))
    {
      err_output("unable to read settings\n");
      goto cleanup;
    }
    as32_print_version(&dev);
    as32_print_settings(&dev);
    goto cleanup;
  }

  if(opts.settings_write_input[0])
  {
    err |= as32_cmd_write_settings(&dev, opts.settings_write_input);
  }

  if(opts.setting_write_encryption[0])
  {
    err |= as32_cmd_write_encryption(&dev, opts.setting_write_encryption);
  }

  /* switch back to normal mode for tx/rx */
  if(as32_set_mode(&dev, NORMAL))
  {
    err_output("unable to go to normal mode\n");
    goto cleanup;
  }

  if(opts.daemon)
  {
    err = become_daemon();
    if(err)
    {
      err_output("mail: error becoming daemon: %d\n", err);
      goto cleanup;
    }
    if(write_pidfile("/run/as32.pid"))
    {
      errno_output("unable to write pid file\n");
    }
    info_output("daemon started pid=%ld", getpid());
  }

  err |= as32_poll(&dev, &opts);
  if(err)
    err_output("error polling %d", err);
cleanup:
  err |= as32_deinit(&dev, &opts);

  return err;
}
