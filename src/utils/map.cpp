#include <string>

#include "utils/map.h"

std::size_t read_size(const std::string &s, std::size_t &index) {
	std::string buffer;
	for (; index < s.size(); index++) {
		if (s[index] == ' ') {
			break;
		}
		buffer += s[index];
	}
	index++;
	return std::stoi(buffer);
}

std::string read_string(const std::string &s, std::size_t &index,
                        std::size_t size) {
	std::string str = s.substr(index, size);
	index += size + 1;
	return str;
}

/**
 * @brief Serialize a map
 * The serialization pattern is :
 * <map length>( <entry key length> <entry key> <entry value length> <entry
 * value>)+ Let suppose we have this map : {"name": "Oliver", "age": "19"} The
 * serialize string will be "2 4 name 6 Oliver 3 age 2 19"
 *
 * @param map The map we want to serialize
 * @return std::string The serialized map
 */
std::string serialize_map(const std::map<std::string, std::string> &map) {
	std::string map_s;
	map_s += std::to_string(map.size());
	for (const auto &elt : map) {
		map_s += " " + std::to_string(elt.first.size()) + " " + elt.first;
		map_s += " " + std::to_string(elt.second.size()) + " " + elt.second;
	}
	return map_s;
}

/**
 * @brief Deserialize a map
 * The serialization pattern is :
 * <map length>( <entry key length> <entry key> <entry value length> <entry
 * value>)+ Let suppose we have this map : {"name": "Oliver", "age": "19"} The
 * serialize string will be "2 4 name 6 Oliver 3 age 2 19"
 *
 * @param map_s The string we want to deserialize
 * @return std::map<std::string, std::string> The deserialized map
 */
std::map<std::string, std::string> deserialize_map(const std::string &map_s) {
	size_t index = 0;
	std::size_t map_size = read_size(map_s, index);

	std::map<std::string, std::string> map;
	for (size_t i = 0; i < map_size; i++) {
		std::size_t key_size = read_size(map_s, index);
		std::string key = read_string(map_s, index, key_size);

		std::size_t value_size = read_size(map_s, index);
		std::string value = read_string(map_s, index, value_size);

		map[key] = value;
	}
	return map;
}
