#undef LTTNG_UST_TRACEPOINT_PROVIDER
#define LTTNG_UST_TRACEPOINT_PROVIDER opentelemetry

#undef LTTNG_UST_TRACEPOINT_INCLUDE
#define LTTNG_UST_TRACEPOINT_INCLUDE                                           \
  "./utils/lttng_opentelemetry_exporter_tracepoints.h"

#if !defined(_LTTNG_OPENTELEMETRY_EXPORTER_TRACEPOINTS_H) ||                   \
    defined(LTTNG_UST_TRACEPOINT_HEADER_MULTI_READ)
#define _LTTNG_OPENTELEMETRY_EXPORTER_TRACEPOINTS_H

#include <lttng/tracepoint.h>

/*
 * Use LTTNG_UST_TRACEPOINT_EVENT(), LTTNG_UST_TRACEPOINT_EVENT_CLASS(),
 * LTTNG_UST_TRACEPOINT_EVENT_INSTANCE(), and
 * LTTNG_UST_TRACEPOINT_LOGLEVEL() here.
 */
LTTNG_UST_TRACEPOINT_EVENT(
    /* Tracepoint provider name */
    opentelemetry,
    /* Tracepoint class name */
    resource_spans,
    /* Input arguments */
    LTTNG_UST_TP_ARGS(const uint8_t *, resource_spans, unsigned int,
                      resource_spans_size),
    /* Output event fields */
    LTTNG_UST_TP_FIELDS(lttng_ust_field_sequence(uint8_t, resource_spans,
                                                 resource_spans, unsigned int,
                                                 resource_spans_size)))

LTTNG_UST_TRACEPOINT_EVENT(
    /* Tracepoint provider name */
    opentelemetry,
    /* Tracepoint class name */
    resource_metrics,
    /* Input arguments */
    LTTNG_UST_TP_ARGS(const uint8_t *, resource_metrics, unsigned int,
                      resource_metrics_size),
    /* Output event fields */
    LTTNG_UST_TP_FIELDS(lttng_ust_field_sequence(uint8_t, resource_metrics,
                                                 resource_metrics, unsigned int,
                                                 resource_metrics_size)))

#endif /* _LTTNG_OPENTELEMETRY_EXPORTER_TRACEPOINTS_H */

#include <lttng/tracepoint-event.h>
