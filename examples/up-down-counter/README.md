# up-down-counter example

In this example, the application:

- Initializes and registers a metrics provider that exports metrics every second.
- Create an Int64 UpDown counter.
- Repeat these action 60 times :
  - Generate a random number as the current counter value.
  - Sleeps for a random duration between 0 and 1 second.
  
  In the background thread, every second, the current value of the counter will be exported.
- Destroys the counter.

Read more about up down counters [in the specification](https://opentelemetry.io/docs/reference/specification/metrics/api/#updowncounter).
