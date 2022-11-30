#ifndef SOCKET_CARRIER_H
#define SOCKET_CARRIER_H

#include "opentelemetry/context/propagation/text_map_propagator.h"

class SocketTextMapCarrier
    : public opentelemetry::context::propagation::TextMapCarrier {
public:
	explicit SocketTextMapCarrier(const std::string &context);

	SocketTextMapCarrier() = default;

	opentelemetry::nostd::string_view
	Get(opentelemetry::nostd::string_view key) const noexcept override;

	void Set(opentelemetry::nostd::string_view key,
	         opentelemetry::nostd::string_view value) noexcept override;

	std::string Serialize() const noexcept;

private:
	std::map<std::string, std::string> context_map_;
};

#endif // !SOCKET_CARRIER_H
