#ifndef MAP_UTILS
#define MAP_UTILS

#include <map>
#include <string>

std::string serialize_map(const std::map<std::string, std::string> &map);
std::map<std::string, std::string> deserialize_map(const std::string &map_s);

#endif // !MAP_UTILS
