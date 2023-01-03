#ifndef OPENTELEMETRY_C_H
#define OPENTELEMETRY_C_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////
// Tracing initialisation and tracers
//////////////////////////////////////////////////////////////////////////

/**
 * @brief Initialize the tracer provider and context propagator
 *
 * Read more on :
 * https://opentelemetry.io/docs/reference/specification/trace/api/#tracerprovider
 * and
 * https://opentelemetry.io/docs/reference/specification/context/api-propagators/#textmap-propagator
 *
 * @param service_name The name of the service we are tracing
 * @param service_version The version of the service
 * @param service_namespace The service namespace
 * @param service_instance_id The host instance id
 */
void init_tracer_provider(const char *service_name, const char *service_version,
                          const char *service_namespace,
                          const char *service_instance_id);

/**
 * @brief Get the tracer object
 * The tracer object help in the future to create spans
 *
 * Read more on :
 * https://opentelemetry.io/docs/reference/specification/trace/api/#tracer
 *
 * @return void*
 */
void *get_tracer();

/**
 * @brief Deallocate all resources used by the tracer
 *
 * @param tracer The tracer we want to destroy
 */
void destroy_tracer(void *tracer);

//////////////////////////////////////////////////////////////////////////
// Attributes map
//////////////////////////////////////////////////////////////////////////

void *create_attr_map();
/**
 * @brief Create a map of attribute (std::map<std::string,
 * opentelemetry::common::AttributeValue>).
 *
 * See https://opentelemetry.io/docs/reference/specification/common/#attribute
 */
void set_bool_attr(void *attr_map, const char *key, int boolean_value);

/**
 * See https://opentelemetry.io/docs/reference/specification/common/#attribute
 */
void set_int32_t_attr(void *attr_map, const char *key, int32_t value);

/**
 * See https://opentelemetry.io/docs/reference/specification/common/#attribute
 */
void set_int64_t_attr(void *attr_map, const char *key, int64_t value);

/**
 * See https://opentelemetry.io/docs/reference/specification/common/#attribute
 */
void set_uint64_t_attr(void *attr_map, const char *key, uint64_t value);

/**
 * See https://opentelemetry.io/docs/reference/specification/common/#attribute
 */
void set_double_attr(void *attr_map, const char *key, double value);

/**
 * See https://opentelemetry.io/docs/reference/specification/common/#attribute
 */
void set_str_attr(void *attr_map, const char *key, const char *value);

/**
 * @brief Deallocate map memory
 */
void destroy_attr_map(void *attr_map);

//////////////////////////////////////////////////////////////////////////
// Spans
//////////////////////////////////////////////////////////////////////////

/**
 * @brief Define the SpanKind
 *
 * See https://opentelemetry.io/docs/reference/specification/trace/api/#spankind
 */
typedef enum { // NOLINTBEGIN
  SPAN_KIND_INTERNAL,
  SPAN_KIND_SERVER,
  SPAN_KIND_CLIENT,
  SPAN_KIND_PRODUCER,
  SPAN_KIND_CONSUMER
} span_kind_t; // NOLINTEND

/**
 * @brief Create a new span
 *
 * Read more on :
 * https://opentelemetry.io/docs/reference/specification/trace/api/#span-creation
 *
 * @param tracer The tracer
 * @param span_name The name of the span we are creating
 * @param span_kind The SpanKind
 * @param remote_context The remote context serialized. This argument is
 * optional when creating nested spans in the same thread
 * @return void* The span
 */
void *start_span(void *tracer, const char *span_name, span_kind_t span_kind,
                 const char *remote_context);

/**
 * @brief Extract the context from the current active span
 *
 * Use this method to get the context to pass around requests
 *
 * The function allocates memory for the return value. This memory must be freed
 * later.
 *
 * For now the context is a string. In future, it could be in a
 * binary format (See
 * https://github.com/open-telemetry/opentelemetry-specification/issues/437)
 *
 * @param span The span
 * @return char* The context serialized
 */
char *extract_context_from_current_span(void *span);

/**
 * @brief Define a span status
 *
 * See
 * https://opentelemetry.io/docs/reference/specification/trace/api/#set-status
 */
typedef enum { // NOLINTBEGIN
  SPAN_STATUS_CODE_UNSET,
  SPAN_STATUS_CODE_OK,
  SPAN_STATUS_CODE_ERROR
} span_status_code_t; // NOLINTEND

/**
 * @brief Set the status of an span
 *
 * Read more on :
 * https://opentelemetry.io/docs/reference/specification/trace/api/#span-operations
 *
 * @param span The span
 * @param code The status code
 * @param description A description. The description matters only for the error
 * status code
 */
void set_span_status(void *span, span_status_code_t code,
                     const char *description);

/**
 * @brief Set the span attributes
 *
 * Read more on :
 * https://opentelemetry.io/docs/reference/specification/trace/api/#span-operations
 *
 * @param span The span
 * @param attr_map The map with all attributes
 */
void set_span_attrs(void *span, void *attr_map);

/**
 * @brief Add a span event
 *
 * Read more on :
 * https://opentelemetry.io/docs/reference/specification/trace/api/#span-operations
 *
 * @param span The span
 * @param event_name The event name
 * @param attr_map The map with all event attributes
 */
void add_span_event(void *span, const char *event_name, void *attr_map);

/**
 * @brief Ends a deallocated memory relating to a span
 *
 * @param span The span we want to end
 */
void end_span(void *span);

//////////////////////////////////////////////////////////////////////////
// Metrics
//////////////////////////////////////////////////////////////////////////

/**
 * @brief Initialize the Meter Provider and Meter reader
 *
 * Read more on
 * https://opentelemetry.io/docs/reference/specification/metrics/sdk/#meterprovider
 * and
 * https://opentelemetry.io/docs/reference/specification/metrics/sdk/#metricreader
 *
 * @param service_name The name of the service we are tracing
 * @param service_version The version of the service
 * @param service_namespace The service namespace
 * @param service_instance_id The host instance id
 * @param export_interval_millis The time interval in milliseconds between two
 * consecutive exports
 * @param export_timeout_millis How long the export can run before it is
 * cancelled
 */
void init_metrics_provider(const char *service_name,
                           const char *service_version,
                           const char *service_namespace,
                           const char *service_instance_id,
                           int64_t export_interval_millis,
                           int64_t export_timeout_millis);

/**
 * @brief Create a int64 up down counter
 *
 * Read more on :
 * https://opentelemetry.io/docs/reference/specification/metrics/api/#updowncounter
 *
 * @param name Counter name
 * @param description A description of what the counter does
 * @return void* The counter
 */
void *create_int64_up_down_counter(const char *name, const char *description);

/**
 * @brief Increment or decrement the UpDownCounter by a fixed amount
 *
 * https://opentelemetry.io/docs/reference/specification/metrics/api/#add-1
 *
 * @param counter The counter
 * @param value The increment or decrement
 */
void int64_up_down_counter_add(void *counter, int64_t value);

/**
 * @brief Deallocate all resources used by the counter
 *
 * @param counter The counter
 */
void destroy_up_down_counter(void *counter);

/**
 * @brief Create a int64 asynchronous up down counter
 *
 * Read more on :
 * https://opentelemetry.io/docs/reference/specification/metrics/api/#asynchronous-updowncounter
 *
 * @param name Counter name
 * @param description A description of what the counter does
 * @return void* The counter
 */
void *create_int64_observable_up_down_counter(const char *name,
                                              const char *description);

/**
 * @brief Register an asynchronous up down counter callback
 *
 * Read more on :
 * https://opentelemetry.io/docs/reference/specification/metrics/api/#asynchronous-updowncounter-operations
 *
 * @param counter The counter
 * @param callback The callback is a function returning the increment to apply
 * to the counter
 */
void int64_observable_up_down_counter_register_callback(void *counter,
                                                        int64_t (*callback)());

/**
 * @brief Deallocate all resources used by the counter
 *
 * @param counter The counter
 */
void destroy_observable_up_down_counter(void *counter);

#ifdef __cplusplus
}
#endif

#endif // !OPENTELEMETRY_C_H
