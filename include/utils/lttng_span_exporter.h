#pragma once

#include <opentelemetry/common/spin_lock_mutex.h>
#include <opentelemetry/nostd/type_traits.h>
#include <opentelemetry/sdk/trace/exporter.h>
#include <opentelemetry/sdk/trace/span_data.h>
#include <opentelemetry/version.h>

#include <iostream>
#include <map>
#include <sstream>

namespace sdk_trace = opentelemetry::sdk::trace;

/**
 * The LttngSpanExporter exports span data through LTTng
 */
class LttngSpanExporter final : public sdk_trace::SpanExporter {
public:
	explicit LttngSpanExporter() noexcept = default;

	std::unique_ptr<opentelemetry::sdk::trace::Recordable>
	MakeRecordable() noexcept override;

	opentelemetry::sdk::common::ExportResult
	Export(const opentelemetry::nostd::span<
	       std::unique_ptr<opentelemetry::sdk::trace::Recordable>>
	           &spans) noexcept override;

	bool Shutdown(std::chrono::microseconds timeout =
	                  std::chrono::microseconds::max()) noexcept override;
};
