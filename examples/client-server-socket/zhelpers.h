// Inspiration
// https://github.com/booksbyus/zguide/blob/master/examples/C/zhelpers.h
#ifndef Z_HELPERS_H
#define Z_HELPERS_H

#include <string.h>
#include <zmq.h>

static const int DEFAULT_BUFFER_SIZE = 1024;

// Receive 0MQ string from socket and convert into C string
// Caller must free returned string. Returns NULL if the context
// is being terminated.
// Remember that the strdup family of functions use malloc/alloc for space
// for the new string.  It must be manually freed when you are done with it.
// Failure to do so will allow a heap attack.
static inline char *s_recv(void *socket) {
  char buffer[DEFAULT_BUFFER_SIZE];
  int size = zmq_recv(socket, buffer, DEFAULT_BUFFER_SIZE - 1, 0);
  buffer[size < DEFAULT_BUFFER_SIZE ? size : DEFAULT_BUFFER_SIZE - 1] = '\0';
#if (defined(WIN32))
  return strdup(buffer);
#else
  return strndup(buffer, sizeof(buffer) - 1);
#endif
}

// Convert C string to 0MQ string and send to socket
static inline int s_send(void *socket, char *string) {
  return zmq_send(socket, string, strlen(string), 0);
}

// Sends string as 0MQ string, as multipart non-terminal
static inline int s_sendmore(void *socket, char *string) {
  return zmq_send(socket, string, strlen(string), ZMQ_SNDMORE);
}

#endif // !Z_HELPERS_H
