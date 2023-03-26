# basic example

In this example, the application:

- Initializes and registers a tracing provider.
- Create a tracer.
- Create a global span and create 3 nested spans inside the same global span. Each nested span:
  - Set a SpanAttribute for the loop iteration number.
  - Log a SpanEvent that will mark the beginning of the work.
  - Sleep between 0 and 1 second (sleep).
  - Log a SpanEvent that will mark the end of the work.
- Destroy the tracer.

All the span operations are documented [in the specification](https://opentelemetry.io/docs/reference/specification/trace/api/#span-operations).
