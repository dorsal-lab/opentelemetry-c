// Inspiration
// https://github.com/booksbyus/zguide/blob/master/examples/C/zhelpers.h
#ifndef Z_HELPERS_H
#define Z_HELPERS_H

#include <zmq.h>

#include <stdlib.h>
#include <string.h>

// Receive 0MQ string from socket and convert into C string
// Caller must free returned string.  Returns NULL if context is being
// terminated.
static char *s_recv(void *socket) {
  zmq_msg_t message;
  zmq_msg_init(&message);
  int size = zmq_msg_recv(&message, socket, 0);
  if (size == -1)
    return NULL;
  char *string = malloc(size + 1);
  memcpy(string, zmq_msg_data(&message), size);
  zmq_msg_close(&message);
  string[size] = 0;
  return string;
}

// Convert C string to 0MQ string and send to socket
static int s_send(void *socket, const char *string) {
  zmq_msg_t message;
  zmq_msg_init_size(&message, strlen(string));
  memcpy(zmq_msg_data(&message), string, strlen(string));
  int size = zmq_msg_send(&message, socket, 0);
  zmq_msg_close(&message);
  return size;
}

// Sends string as 0MQ string, as multipart non-terminal
static int s_sendmore(void *socket, const char *string) {
  zmq_msg_t message;
  zmq_msg_init_size(&message, strlen(string));
  memcpy(zmq_msg_data(&message), string, strlen(string));
  int size = zmq_msg_send(&message, socket, ZMQ_SNDMORE);
  zmq_msg_close(&message);
  return size;
}

#endif // !Z_HELPERS_H
