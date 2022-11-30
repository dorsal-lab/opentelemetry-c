#include "opentelemetry-c.h"

#include "utils/lttng_span_exporter.h"
#include "utils/map.h"
#include "utils/socket_carrier.h"

#include <opentelemetry/context/propagation/global_propagator.h>
#include <opentelemetry/context/propagation/text_map_propagator.h>
#include <opentelemetry/context/runtime_context.h>
#include <opentelemetry/exporters/ostream/span_exporter.h>
#include <opentelemetry/nostd/shared_ptr.h>
#include <opentelemetry/sdk/trace/simple_processor.h>
#include <opentelemetry/sdk/trace/simple_processor_factory.h>
#include <opentelemetry/sdk/trace/tracer_provider.h>
#include <opentelemetry/sdk/trace/tracer_provider_factory.h>
#include <opentelemetry/trace/context.h>
#include <opentelemetry/trace/propagation/http_trace_context.h>
#include <opentelemetry/trace/provider.h>
#include <opentelemetry/trace/scope.h>
#include <opentelemetry/trace/span.h>
#include <opentelemetry/trace/tracer.h>

#include <cstddef>
#include <memory>
#include <string>

namespace context = opentelemetry::context;
namespace trace = opentelemetry::trace;
namespace trace_sdk = opentelemetry::sdk::trace;
namespace nostd = opentelemetry::nostd;

struct SpanAndScope {
	nostd::shared_ptr<trace::Span> span;
	trace::Scope scope;
};

void init_tracing() {
	auto exporter =
	    std::unique_ptr<trace_sdk::SpanExporter>(new LttngSpanExporter);
	auto processor =
	    trace_sdk::SimpleSpanProcessorFactory::Create(std::move(exporter));
	std::shared_ptr<opentelemetry::trace::TracerProvider> provider =
	    trace_sdk::TracerProviderFactory::Create(std::move(processor));
	// Set the global trace provider
	trace::Provider::SetTracerProvider(provider);
	// set global propagator
	context::propagation::GlobalTextMapPropagator::SetGlobalPropagator(
	    nostd::shared_ptr<context::propagation::TextMapPropagator>(
	        new trace::propagation::HttpTraceContext()));
}

void *get_tracer(char *service_name, char *service_version) {
	auto provider = opentelemetry::trace::Provider::GetTracerProvider();
	return new nostd::shared_ptr<trace::Tracer>(provider->GetTracer(
	    std::string(service_name), std::string(service_version)));
}

void destroy_tracer(void *tracer) {
	delete static_cast<nostd::shared_ptr<trace::Tracer> *>(tracer);
}

void *start_span(void *tracer, char *span_name, char *remote_context) {
	// Span options
	trace::StartSpanOptions options;
	// TODO(augustinsangam): Give ability to change
	options.kind = trace::SpanKind::kInternal;

	auto prop =
	    context::propagation::GlobalTextMapPropagator::GetGlobalPropagator();
	auto ctx = context::RuntimeContext::GetCurrent();
	// Context extraction
	if (remote_context != NULL) { // NOLINT
		const SocketTextMapCarrier carrier((std::string(remote_context)));
		auto new_ctx = prop->Extract(carrier, ctx);
		options.parent = trace::GetSpan(new_ctx)->GetContext();
	}

	// Create the span
	auto *tracer_p = static_cast<nostd::shared_ptr<trace::Tracer> *>(tracer);
	auto span = (*tracer_p)->StartSpan(span_name, options);
	return new SpanAndScope{span, (*tracer_p)->WithActiveSpan(span)};
}

char *extract_context_from_current_span() {
	auto ctx = context::RuntimeContext::GetCurrent();
	auto span = trace::GetSpan(ctx);
	auto prop =
	    context::propagation::GlobalTextMapPropagator::GetGlobalPropagator();
	SocketTextMapCarrier carrier;
	prop->Inject(carrier, ctx);
	std::string ctx_s = carrier.Serialize();
	const auto size = ctx_s.size();
	char *buffer = new char[size + 1]; // NOLINT
	memcpy(buffer, ctx_s.c_str(), size + 1);
	return buffer;
}

void end_span(void *span) {
	auto *span_and_scope = static_cast<SpanAndScope *>(span);
	(*span_and_scope).span->End();
	delete span_and_scope;
}
