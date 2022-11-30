#undef LTTNG_UST_TRACEPOINT_PROVIDER
#define LTTNG_UST_TRACEPOINT_PROVIDER opentelemetry

#undef LTTNG_UST_TRACEPOINT_INCLUDE
#define LTTNG_UST_TRACEPOINT_INCLUDE                                           \
	"./utils/lttng_span_exporter_lttng_tracepoint.h"

#if !defined(_LTTNG_SPAN_EXPORTER_LTTNG_TRACEPOINT_H) ||                       \
    defined(LTTNG_UST_TRACEPOINT_HEADER_MULTI_READ)
#	define _LTTNG_SPAN_EXPORTER_LTTNG_TRACEPOINT_H

#	include <lttng/tracepoint.h>

/*
 * Use LTTNG_UST_TRACEPOINT_EVENT(), LTTNG_UST_TRACEPOINT_EVENT_CLASS(),
 * LTTNG_UST_TRACEPOINT_EVENT_INSTANCE(), and
 * LTTNG_UST_TRACEPOINT_LOGLEVEL() here.
 */

LTTNG_UST_TRACEPOINT_EVENT(
    /* Tracepoint provider name */
    opentelemetry,
    /* Tracepoint class name */
    otlp_recordable,
    /* Input arguments */
    LTTNG_UST_TP_ARGS(const char *, span, const char *, ressource, const char *,
                      instrumentation_scope),
    /* Output event fields */
    LTTNG_UST_TP_FIELDS(lttng_ust_field_string(
        span_field, span) lttng_ust_field_string(ressource_field, ressource)
                            lttng_ust_field_string(instrumentation_scope_field,
                                                   instrumentation_scope)))

LTTNG_UST_TRACEPOINT_EVENT(
    /* Tracepoint provider name */
    opentelemetry,
    /* Tracepoint class name */
    otlp_recordable_debug,
    /* Input arguments */
    LTTNG_UST_TP_ARGS(const char *, span, const char *, ressource, const char *,
                      instrumentation_scope),
    /* Output event fields */
    LTTNG_UST_TP_FIELDS(lttng_ust_field_string(
        span_field, span) lttng_ust_field_string(ressource_field, ressource)
                            lttng_ust_field_string(instrumentation_scope_field,
                                                   instrumentation_scope)))

#endif /* _LTTNG_SPAN_EXPORTER_LTTNG_TRACEPOINT_H */

#include <lttng/tracepoint-event.h>
