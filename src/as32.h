#ifndef AS32_DEF
#define AS32_DEF

#include <assert.h>
#include <poll.h>
#include <sys/time.h>
#include <termios.h>
#include "options.h"
#include "gpio.h"
#include "uart.h"
#include "list.h"

/*
 The as32 has a TX buffer of 512 bytes but how the implemented it's usage
 with the AUX pin makes things difficult. I've noticied if you write 512
 bytes to it and then wait for AUX to go high, then write another 512 bytes
 you will overwrite most of the 512 bytes. For example maybe the first 64
 bytes would get transmitted then the next would be overwritten. However,
 this seems to happen on the event the buffer is empty originally as if you
 send 512 continuously the first 512 are dropped but the remaining would be
 sent. This is a potential improvement but needs more consideration. The poor
 documentation of the datasheet and how AUX is implemented makes figuring this
 out quite difficult. For now we'll have to use 58 until a better solution
 can be devised.
*/
#define AS32_MAX_PACKET_LENGTH 58

#define TX_BUF_BYTES 512
#define RX_BUF_BYTES 512

enum AS32_mode
{
  NORMAL,
  WAKE_UP,
  POWER_SAVE,
  SLEEP
};

enum AS32_state
{
  IDLE,
  RX,
  TX
};

struct AS32
{
  enum AS32_state state;
  int verbose;
  int fd_gpio_m0;
  int fd_gpio_m1;
  int fd_gpio_aux;
  int uart_fd;
  struct termios tty;
  int isatty;
  int prev_mode;
  int mode;
  uint8_t version[50];
  uint8_t settings[6];
  int frequency_mhz;
  int ver;
  int features;
  int power_down_save;
  int addh;
  int addl;
  int parity;
  int uart_baud;
  int air_data_rate;
  int option;
  int channel;
  int transmission_mode;
  int io_drive;
  int wireless_wakeup_time;
  int fec;
  int tx_power_dbm;
  struct List *socket_list;
};

int
as32_init(struct AS32 *dev, struct options *opts);

int
as32_deinit(struct AS32 *dev, struct options *opts);

int
as32_set_mode(struct AS32 *dev, int mode);

int
as32_get_mode(struct AS32 *dev);

int
as32_cmd_read_settings(struct AS32 *dev);

void
as32_print_settings(struct AS32 *dev);

int
as32_cmd_read_operating(struct AS32 *dev);

int
as32_cmd_read_version(struct AS32 *dev);

void
as32_print_version(struct AS32 *dev);

int
as32_cmd_reset(struct AS32 *dev);

int
as32_cmd_write_settings(struct AS32 *dev, uint8_t *settings);

int
as32_cmd_write_encryption(struct AS32 *dev, uint8_t *encryption);

ssize_t
as32_transmit(struct AS32 *dev, uint8_t *buf, size_t buf_len);

int
as32_receive(struct AS32 *dev, uint8_t *buf, size_t buf_len);

size_t
as32_poll(struct AS32 *dev, struct options *opts);

#endif
