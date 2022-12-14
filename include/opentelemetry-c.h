#ifndef OPENTELEMETRY_C_H
#define OPENTELEMETRY_C_H

#ifdef __cplusplus
extern "C" {
#endif

// Tracing initialisation and tracers
void init_tracing(const char *service_name, const char *service_version,
                  const char *service_namespace,
                  const char *service_instance_id);
void *get_tracer();
void destroy_tracer(void *tracer);

// Spans
typedef enum { // NOLINTBEGIN
	SPAN_KIND_INTERNAL,
	SPAN_KIND_SERVER,
	SPAN_KIND_CLIENT,
	SPAN_KIND_PRODUCER,
	SPAN_KIND_CONSUMER
} span_kind_t; // NOLINTEND
void *start_span(void *tracer, const char *span_name, span_kind_t span_kind,
                 const char *remote_context);
char *extract_context_from_current_span();
typedef enum { // NOLINTBEGIN
	SPAN_STATUS_CODE_UNSET,
	SPAN_STATUS_CODE_OK,
	SPAN_STATUS_CODE_ERROR
} status_code_t; // NOLINTEND
void set_span_status(void *span, status_code_t code, const char *description);
void end_span(void *span);

#ifdef __cplusplus
}
#endif

#endif // !OPENTELEMETRY_C_H
