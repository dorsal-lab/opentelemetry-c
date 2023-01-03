# opentelemetry-c

This project is a static library wrapper arround the official [opentelemetry-cpp](https://github.com/open-telemetry/opentelemetry-cpp) library. Understanding [OpenTelemetry specification](https://opentelemetry.io/docs/reference/specification/) is a prerequisite.

All telemetry data generated by `opentelemetry-cpp` library are described using [protocol buffers](https://developers.google.com/protocol-buffers). Those protocol buffers are serialized and exported in binary format to [LTTng](https://lttng.org/). LTTng logs all these binary data to [CTF files](https://diamon.org/ctf/). The [otel-replayer](https://github.com/augustinsangam/otel-replayer) project can be used to read the CTF files and export them to various observability backends (e.g. Jaeger, Prometheus). 


| Type of <br>telemetry data | Description | LTTng tracepoint | Status |
|---|---|---|---|
| traces<br>(spans) | A Span represents a unit of work or operation. <br>It tracks specific operations that a request makes, <br>painting a picture of what happened during the time in which that operation was executed. | opentelemetry:resource_spans<br>See [Ressources spans proto](https://github.com/open-telemetry/opentelemetry-proto/blob/v0.19.0/opentelemetry/proto/trace/v1/trace.proto#L48) and [OTLP ExportTraceServiceRequest](https://github.com/open-telemetry/opentelemetry-proto/blob/v0.19.0/opentelemetry/proto/collector/trace/v1/trace_service.proto#L42) | Implemented |
| logs | A log is a timestamped message emitted by services or other components | opentelemetry:resource_logs<br>See [Ressources logs proto](https://github.com/open-telemetry/opentelemetry-proto/blob/v0.19.0/opentelemetry/proto/logs/v1/logs.proto#L48) and [OTLP ExportLogsServiceRequest](https://github.com/open-telemetry/opentelemetry-proto/blob/v0.19.0/opentelemetry/proto/collector/logs/v1/logs_service.proto#L42) | Work in progress |
| metrics | Metrics are aggregations over a period of time of numeric data about your infrastructure or application | opentelemetry:resource_metrics<br>See [Ressources metrics proto](https://github.com/open-telemetry/opentelemetry-proto/blob/v0.19.0/opentelemetry/proto/metrics/v1/metrics.proto#L48) and [OTLP ExportMetricsServiceRequest](https://github.com/open-telemetry/opentelemetry-proto/blob/v0.19.0/opentelemetry/proto/collector/metrics/v1/metrics_service.proto#L42) | Int64 UpDown counter and Int64 Observable UpDown implemented |

# Use the wrapper 
Examples of usage can be found in [the examples folder](examples/). Refer to the [Dockerfile](Dockerfile) for project dependencies.

To execute an example :
```sh
./run.sh <example>
```
