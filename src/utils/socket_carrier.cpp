#include "utils/socket_carrier.h"
#include "utils/map.h"

SocketTextMapCarrier::SocketTextMapCarrier(const std::string &context)
    : context_map_(deserialize_map(context)) {}

opentelemetry::nostd::string_view SocketTextMapCarrier::Get(
    opentelemetry::nostd::string_view key) const noexcept {
	std::string key_to_compare = key.data();
	auto it = context_map_.find(key_to_compare);
	if (it != context_map_.end()) {
		return it->second;
	}
	return "";
}

void SocketTextMapCarrier::Set(
    opentelemetry::nostd::string_view key,
    opentelemetry::nostd::string_view value) noexcept {
	context_map_.insert(std::pair<std::string, std::string>(
	    std::string(key), std::string(value)));
}

std::string SocketTextMapCarrier::Serialize() const noexcept {
	return serialize_map(context_map_);
}
