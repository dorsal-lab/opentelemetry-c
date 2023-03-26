# client-server-socket example

This is a simple client/server application using ZeroMQ.

The client :

- Initializes and registers a tracing provider.
- Create a tracer.
- Create a REQ socket and connected to the server REP socket on port 5555.
- Create 5 spans. Each span:
  - Send its SpanContext to the server.
  - Send a simple "Hello" message.
  - Receive a response from the server.
- Destroy the socket and the tracer.

The server :

- Initializes and registers a tracing provider.
- Create a tracer.
- Create a REP socket and binds to port 5555.
- Receive 5 SpanContext messages:
  - For each message, a new span is created taking into account the remote SpanContext received.
  - The "Hello" message is received.
  - The program sleeps for 5 seconds.
  - Send a simple "World!" response message.
- Destroy the socket and the tracer.
