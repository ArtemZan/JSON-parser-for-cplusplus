#include "JSONParser.h"

std::ostream& undefined_ns::undefined::operator<<(std::ostream& stream)
{
	return stream << "undefined";
}


undefined_ns::undefined undefined;

std::ostream& operator<<(std::ostream& stream, JSONType& json)
{
	switch (json.m_type)
	{
	case JSONType::TYPE::ARRAY: {
		JSONType::array_t& arr = *static_cast<JSONType::array_t*>(json.m_data.get());
		for (int i = 0; i < arr.size(); i++)
		{
			stream << arr[i];

			if (i + 1 < arr.size())
			{
				stream << ", ";
			}
		}
		break;
	}
	case JSONType::TYPE::OBJECT: {
		JSONType::object_t& obj = *static_cast<JSONType::object_t*>(json.m_data.get());
		stream << '{';
		for (JSONType::object_t::iterator i = obj.begin(); i != obj.end(); i++)
		{
			if (i != obj.begin())
			{
				stream << ", ";
			}

			stream << '"' << i->first << "\":" << i->second;
		}
		stream << '}';
		break;
	}

	case JSONType::TYPE::BOOLEAN: {
		stream << (*(bool*)json.m_data.get() ? "true" : "false");
		break;
	}

	case JSONType::TYPE::NULL: {
		stream << "null";
		break;
	}

	case JSONType::TYPE::NUMBER: {
		stream << *(JSONType::number_t*)json.m_data.get();
		break;
	}

	case JSONType::TYPE::STRING: {
		stream << '"' << *(JSONType::string_t*)json.m_data.get() << '"';
		break;
	}

	}

	return stream;
}

JSONParser::JSONParser()
{

}

JSONParser::~JSONParser()
{
}
