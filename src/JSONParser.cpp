#include "JSONParser.h"
#include <algorithm>

namespace json {
	JSONParser::JSONParser(){}

	std::string JSONParser::removeSpaces(std::string& data) {
		std::string result;
		bool state = true;
		for (size_t i = 0; i < data.size(); i++) {
			if (data[i] == '\"' && i > 0 && data[i - 1] != '\\') state = !state;
			if (state && (data[i] == ' ' || data[i] == '\t' || data[i] == '\n')) continue;
			result += data[i];
		}
		return result;
	}

	JSONParser::type JSONParser::getType(std::string& data) {
		if (data[0] == '[' && data[data.size() - 1] == ']') return type::Array;
		else if (data[0] == '{' && data[data.size() - 1] == '}') return type::Object;
		else if (data[0] == '\"' && data[data.size() - 1] == '\"') return type::String;
		else if (data == "false" || data == "true") return type::Boolean;
		else if (data.find_first_not_of("0123456789") == std::string::npos) return type::Int;
		else if (data.find_first_not_of("0123456789.") == std::string::npos && std::count(data.begin(), data.end(), '.') == 1) return type::Double;
		else throw UnkownType(data);
	}

	std::vector<std::string> JSONParser::smartSplit(std::string& data, char delimiter) {
		std::vector<std::string> result;
		std::string carry = "";

		bool state = true;
		int depth = 0;
		for (size_t i = 0; i < data.size(); i++) {
			if (data[i] == '\"' && i > 0 && data[i - 1] != '\\') state = !state;

			if (data[i] == '{' || data[i] == '[') depth++;
			else if (data[i] == '}' || data[i] == ']') depth--;

			if (depth == 0 && data[i] == delimiter) {
				result.push_back(carry);
				carry = "";
			}
			else {
				carry += data[i];
			}
		}

		if (carry.size() > 0) result.push_back(carry);

		return result;
	}

	std::string JSONParser::trimOneChar(std::string& s) {
		return s.substr(1, s.size() - 2);
	}

	int JSONParser::toInt(std::string& data) {
		return std::stoi(data);
	}

	double JSONParser::toDouble(std::string& data) {
		return std::stod(data);
	}

	bool JSONParser::toBool(std::string& data) {
		return data == "true";
	}

	JSONObject JSONParser::parseJsonObject(std::string& jsonString) {
		jsonString = removeSpaces(jsonString);
		if (getType(jsonString) != type::Object) throw JsonTypeMismatch();
		jsonString = trimOneChar(jsonString); // delte the prefix and suffix '{' and '}'

		std::vector<std::string> objectMembers = smartSplit(jsonString, ',');
		JSONObject result;
		for (std::string& member : objectMembers) {
			std::string key = smartSplit(member, ':')[0];
			std::string value = member.substr(key.size() + 1);
			key = trimOneChar(key);

			switch (getType(value)) {
			case type::Int:
				result.put(key, toInt(value));
				break;
			case type::Double:
				result.put(key, toDouble(value));
				break;
			case type::Boolean:
				result.put(key, toBool(value));
				break;
			case type::String:
				result.put(key, trimOneChar(value));
				break;
			case type::Object:
				result.put(key, parseJsonObject(value));
				break;
			case type::Array:
				result.put(key, parseJsonArray(value));
				break;
			}
		}
		return result;
	}
	JSONArray JSONParser::parseJsonArray(std::string& jsonString) {
		jsonString = removeSpaces(jsonString);

		if (getType(jsonString) != type::Array) throw JsonTypeMismatch();
		jsonString = trimOneChar(jsonString); // delte the prefix and suffix '[' and ']'
		std::vector<std::string> items = smartSplit(jsonString, ',');
		JSONArray result;
		for (std::string& item : items) {
			switch (getType(item)) {
			case type::Int:
				result.add(toInt(item));
				break;
			case type::Double:
				result.add(toDouble(item));
				break;
			case type::Boolean:
				result.add(toBool(item));
				break;
			case type::String:
				result.add(trimOneChar(item));
				break;
			case type::Object:
				result.add(parseJsonObject(item));
				break;
			case type::Array:
				result.add(parseJsonArray(item));
				break;
			}
		}
		return result;
	}
	JSONParser::~JSONParser(){}
};