#define TRACEPOINT_CREATE_PROBES
#define TRACEPOINT_DEFINE

#include "utils/lttng_span_exporter.h"
#include "utils/lttng_span_exporter_lttng_tracepoint.h"

#include <opentelemetry/exporters/otlp/otlp_recordable.h>
#include <opentelemetry/proto/trace/v1/trace.pb.h>
#include <opentelemetry/sdk/trace/span_data.h>

#include <iostream>
#include <memory>
#include <string>

namespace otlp_exporter = opentelemetry::exporter::otlp;
namespace sdk_trace = opentelemetry::sdk::trace;

std::unique_ptr<sdk_trace::Recordable>
LttngSpanExporter::MakeRecordable() noexcept {
	return std::unique_ptr<sdk_trace::Recordable>(
	    new otlp_exporter::OtlpRecordable);
}

opentelemetry::sdk::common::ExportResult LttngSpanExporter::Export(
    const opentelemetry::nostd::span<std::unique_ptr<sdk_trace::Recordable>>
        &spans) noexcept {
	for (auto &recordable_span : spans) {
		auto recordable = std::unique_ptr<otlp_exporter::OtlpRecordable>(
		    static_cast<otlp_exporter::OtlpRecordable *>(
		        recordable_span.release()));
		std::string span_serialized = recordable->span().SerializeAsString();
		lttng_ust_tracepoint(
		    opentelemetry, otlp_recordable,
		    recordable->span().SerializeAsString().c_str(),
		    recordable->ProtoResource().SerializeAsString().c_str(),
		    recordable->GetProtoInstrumentationScope()
		        .SerializeAsString()
		        .c_str());
		// std::cout << "Debug span :" << recordable->span().DebugString()
		//           << std::endl;
		// std::cout << "Debug ressource :" << recordable->span().DebugString()
		//           << std::endl;
		// std::cout << "Debug instrumentation scope :"
		//           << recordable->span().DebugString() << std::endl;
		// lttng_ust_tracepoint(
		//     opentelemetry, otlp_recordable_debug,
		//     recordable->span().DebugString().c_str(),
		//     recordable->ProtoResource().DebugString().c_str(),
		//     recordable->GetProtoInstrumentationScope().DebugString().c_str());
	}
	return opentelemetry::sdk::common::ExportResult::kSuccess;
}

bool LttngSpanExporter::Shutdown(
    std::chrono::microseconds /*timeout*/) noexcept {
	return true;
}
