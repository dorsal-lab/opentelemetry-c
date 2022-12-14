#include <bits/c++config.h>
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
		auto rec = std::unique_ptr<otlp_exporter::OtlpRecordable>(
		    static_cast<otlp_exporter::OtlpRecordable *>(
		        recordable_span.release()));

		opentelemetry::proto::trace::v1::ResourceSpans ressource_spans;
		*ressource_spans.mutable_resource() = rec->ProtoResource();

		auto *scope_spans = ressource_spans.add_scope_spans();
		*scope_spans->mutable_scope() = rec->GetProtoInstrumentationScope();
		*scope_spans->add_spans() = std::move(rec->span());
		scope_spans->set_schema_url(rec->GetInstrumentationLibrarySchemaURL());

		ressource_spans.set_schema_url(rec->GetResourceSchemaURL());

		std::string ressource_spans_str = ressource_spans.SerializeAsString();
		std::size_t length = ressource_spans_str.size();

		auto *ressource_spans_bytes = new uint8_t[length];
		memcpy(ressource_spans_bytes, ressource_spans_str.c_str(), length);
		lttng_ust_tracepoint(opentelemetry, resource_spans,
		                     ressource_spans_bytes, length);

		std::cout << "Debug ressource spans :" << ressource_spans.DebugString()
		          << std::endl;

		delete[] ressource_spans_bytes;
	}
	return opentelemetry::sdk::common::ExportResult::kSuccess;
}

bool LttngSpanExporter::Shutdown(
    std::chrono::microseconds /*timeout*/) noexcept {
	return true;
}
