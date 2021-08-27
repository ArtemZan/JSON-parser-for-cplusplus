#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include <vector>
#include <set>
#include <stack>
#include <unordered_map>
#include <map>
#include <variant>
#include <any>

#include <cassert>

namespace undefined_ns
{
	struct undefined {
		std::ostream& operator<<(std::ostream& stream);
	};
}

extern undefined_ns::undefined undefined;

#define NULL NULL

#define stringify(varname) "#varname"

class JSONType
{
public:
	enum class TYPE : int
	{
		ARRAY,
		BOOLEAN,
		NULL,
		NUMBER,
		OBJECT,
		STRING
	};

	//private:
	TYPE m_type;
	std::shared_ptr<void> m_data;

public:

	typedef std::map<size_t, JSONType> array_t;
	typedef double number_t;
	typedef std::unordered_map<std::string, JSONType> object_t;
	typedef std::string string_t;

	JSONType()
		:m_type(TYPE::NULL), m_data(nullptr)
	{

	}

	JSONType(const string_t& string)
		:m_type(TYPE::STRING)
	{
		m_data = std::make_shared<string_t>(string);
	}

	JSONType(number_t number)
		:m_type(TYPE::NUMBER)
	{
		m_data = std::make_shared<number_t>(number);
	}

	JSONType(const object_t& object)
		:m_type(TYPE::OBJECT)
	{
		m_data = std::shared_ptr<void>(new object_t(object));
	}

	JSONType(const array_t& array)
		:m_type(TYPE::ARRAY)
	{
		m_data = std::make_shared<array_t>(array);
	}

	JSONType(bool boolean)
		:m_type(TYPE::BOOLEAN)
	{
		m_data = std::make_shared<bool>(boolean);
	}

	void operator=(const JSONType& other)
	{
		m_type = other.m_type;
		switch (m_type)
		{
		case TYPE::ARRAY:	m_data = std::make_shared<array_t>(*reinterpret_cast<array_t*>(other.m_data.get())); break;
		case TYPE::BOOLEAN: m_data = std::make_shared<bool>(*reinterpret_cast<bool*>(other.m_data.get()));			break;
		case TYPE::NULL:	m_data = nullptr;													break;
		case TYPE::NUMBER:	m_data = std::make_shared<number_t>(*reinterpret_cast<number_t*>(other.m_data.get()));	break;
		case TYPE::OBJECT:	m_data = std::make_shared<object_t>(*reinterpret_cast<object_t*>(other.m_data.get()));	break;
		case TYPE::STRING:	m_data = std::make_shared<string_t>(*reinterpret_cast<string_t*>(other.m_data.get()));	break;
		}
	}

	JSONType& operator[](size_t key)
	{
		if (m_type == TYPE::ARRAY)
		{
			array_t& ar = *static_cast<array_t*>(m_data.get());
			return ar[key];
		}

		if (m_type == TYPE::OBJECT)
		{
			object_t& obj = *static_cast<object_t*>(m_data.get());
			return obj[std::to_string(key)];
		}
	}


	JSONType& operator[](std::string key)
	{
		if (m_type == TYPE::OBJECT)
		{
			object_t& obj = *static_cast<object_t*>(m_data.get());
			return obj[key];
		}
	}

	JSONType& operator+=(const JSONType& other)
	{
		if (m_type != TYPE::NUMBER || other.m_type != TYPE::NUMBER)
		{
			std::cout << "Operands of += must be numbers\n";
		}
		else
		{
			*(number_t*)(m_data.get()) += *(number_t*)(other.m_data.get());
		}

		return *this;
	}

	JSONType operator+(const JSONType& other)
	{
		JSONType res(*this);

		if (m_type != TYPE::NUMBER || other.m_type != TYPE::NUMBER)
		{
			std::cout << "Operands of + must be numbers\n";
		}
		else
		{
			res += other;
		}

		return res;
	}

	JSONType operator!()
	{
		JSONType res(*this);

		if (m_type == TYPE::BOOLEAN)
		{
			*(bool*)res.m_data.get() = !*(bool*)res.m_data.get();
		}
		else if (m_type == TYPE::NUMBER)
		{
			*(int64_t*)res.m_data.get() = !*(int64_t*)res.m_data.get();
		}
		else
		{
			std::cout << "Operand of + must be boolean or number\n";
		}

		return res;
	}

	std::ostream& operator<<(std::ostream& stream)
	{
		stream << (*this);
	}

	~JSONType()
	{
	}

	friend std::ostream& operator<<(std::ostream& stream, JSONType& json);
};



std::ostream& operator<<(std::ostream& stream, JSONType& json);

class JSONParser
{
public:
	JSONParser();
	~JSONParser();



	JSONType ParseFile(const std::string& file_path)
	{
		JSONType res;
		ParseFile(res, file_path);
	}

	void ParseFile(JSONType& result_buffer, const std::string& file_path)
	{
		std::ifstream file;
		file.open(file_path, std::ios::in);
		if (!file.is_open())
		{
			std::cout << "Failed to open file\n";
		}

		std::stringstream ss;
		std::string line;

		while (std::getline(file, line))
		{
			ss << line << '\n';
		}

		file.close();

		Parse(result_buffer, ss.str());
	}

	JSONType Parse(const std::string& json)
	{
		JSONType res;
		Parse(res, json);
		return res;
	}



	void Parse(JSONType& result_buffer, const std::string& json)
	{
		bool initialized = false;
		bool reading_string = false;
		bool reading_value = false;
		std::string string;
		std::string key;
		std::string value;
		bool valid = true;
		int line = 0;

		enum
		{
			ARRAY = 1,
			BOOLEAN = 2,
			NULL = 4,
			NUMBER = 8,
			OBJECT = 16,
			STRING = 32,

			KEY = 64,
			VALUE = ARRAY | BOOLEAN | NULL | NUMBER | OBJECT | STRING,
			COMMA = 128
		};

		int expect = ARRAY | OBJECT;
		int value_type = NULL;

		int open_curly_br = 0;

		for (int i = 0; json[i] && valid; i++)
		{
			int got = -1;

			if (value_type == OBJECT)
			{
				// Read object to 'string'

				switch (json[i])
				{
				case '{':
				{
					open_curly_br++;
					break;
				}
				case '}':
				{
					open_curly_br--;
					if (value_type == OBJECT && open_curly_br == 0)
					{
						result_buffer[key] = Parse(string + '}');
						value_type = NULL;
						string = "";
						value = "";
						expect = COMMA;
					}
					break;
				}
				default:
				{
					string += json[i];
				}
				}
				
				continue;
			}

			switch (json[i])
			{
			case ' ': continue;
			case '\t': continue;

			case '{': {
				if (initialized)
				{
					//Start reading object to 'string'

					open_curly_br = 1;
					string = '{';
					value_type = OBJECT;
					break;
				}

				if (OBJECT & expect)
				{
					*(&result_buffer) = JSONType(JSONType::object_t());
					expect = KEY;

					initialized = true;
				}

				break;
			}

			case ':': {

				if (key.size())
				{
					expect = VALUE;
				}
				break;
			}

			case '\n': {
				line++;
				break;
			}

			case '"': {
				if (reading_string)
				{
					if (expect & KEY)
					{
						key = string;
						string = "";
					}
					else if (expect & VALUE)
					{
						value_type = STRING;
					}
				}
				else
				{
					value_type = NULL;
				}

				reading_string = !reading_string;

				break;
			}

			case '}':
			{
				if ((expect & KEY) && string.size() == 0)
				{
					return;
				}
				// NO BREAK !!!
			}
			case ',':
			{
				if (string.size())
				{
					value = string;
				}

				if (value.size())
				{
					if (value_type == STRING)
					{
						result_buffer[key] = value;
					}
					else if (value == "null")
					{
						result_buffer[key] = JSONType();
					}
					else if (value == "true")
					{
						result_buffer[key] = true;
					}
					else if (value == "false")
					{
						result_buffer[key] = false;
					}
					else
					{
						result_buffer[key] = JSONType(std::stod(value));
					}

					expect = KEY;
					value = "";
					key = "";
					string = "";
				}

				break;
			}

			default: {
				if (expect & VALUE)
				{
					reading_value = true;
				}
				string += json[i];
			}
			}

			if ((got & expect) == 0)
			{
				std::cout << "Error: at position " << i << ", line " << line << ": Expected " << expect << ", but got " << got << '\n';
			}
		}
	}
};