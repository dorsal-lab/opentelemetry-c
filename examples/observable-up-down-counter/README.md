# observable-up-down-counter example

In this example, the application:

- Initializes and registers a metrics provider that exports metrics every second.
- Create an Int64 Asynchronous UpDown counter.
- Register a counter callback. This callback will be called every second to get the updated value for the metric in a background thread.
- Sleeps for 60 seconds. We expect metrics to be exported around 60 times.
- Cancels the registration and destroys the counter.

Read more about asynchronous up down counters [in the specification](https://opentelemetry.io/docs/reference/specification/metrics/api/#asynchronous-updowncounter).
