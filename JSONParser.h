#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <stack>
#include <cstdarg>

#include <variant>
#include <any>
#include <unordered_map>
#include <map>

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
	void* m_data;

public:

	typedef std::map<size_t, JSONType> array_t;
	typedef int number_t;
	typedef std::unordered_map<std::string, JSONType> object_t;
	typedef std::string string_t;

	JSONType()
		:m_type(TYPE::NULL), m_data(nullptr)
	{

	}

	JSONType(const string_t& string)
		:m_type(TYPE::STRING)
	{
		m_data = new string_t(string);
	}

	JSONType(number_t number)
		:m_type(TYPE::NUMBER)
	{
		m_data = new number_t(number);
	}

	JSONType(const object_t& object)
		:m_type(TYPE::OBJECT)
	{
		m_data = new object_t(object);
	}

	JSONType(const array_t& array)
		:m_type(TYPE::ARRAY)
	{
		m_data = new array_t(array);
	}

	JSONType(bool boolean)
		:m_type(TYPE::BOOLEAN)
	{
		m_data = new bool(boolean);
	}

	void operator=(const JSONType& other)
	{
		m_type = other.m_type;
		delete m_data;
		switch (m_type)
		{
		case TYPE::ARRAY:	m_data = new array_t(*reinterpret_cast<array_t*>(other.m_data)); break;
		case TYPE::BOOLEAN: m_data = new bool(*reinterpret_cast<bool*>(other.m_data));			break;
		case TYPE::NULL:	m_data = nullptr;													break;
		case TYPE::NUMBER:	m_data = new number_t(*reinterpret_cast<number_t*>(other.m_data));	break;
		case TYPE::OBJECT:	m_data = new object_t(*reinterpret_cast<object_t*>(other.m_data));	break;
		case TYPE::STRING:	m_data = new string_t(*reinterpret_cast<string_t*>(other.m_data));	break;
		}
	}

	JSONType& operator[](size_t key)
	{
		if (m_type == TYPE::ARRAY)
		{
			array_t& ar = *static_cast<array_t*>(m_data);
			return ar[key];
		}

		if (m_type == TYPE::OBJECT)
		{
			object_t& obj = *static_cast<object_t*>(m_data);
			return obj[std::to_string(key)];
		}
	}


	JSONType& operator[](std::string key)
	{
		if (m_type == TYPE::OBJECT)
		{
			object_t& obj = *static_cast<object_t*>(m_data);
			return obj[key];
		}
	}

	~JSONType()
	{
		delete m_data;
	}
};

class JSONParser
{
public:
	JSONParser();
	~JSONParser();



	JSONType ParseFile(const std::string& file_path)
	{


	}

	void ParseFile(JSONType& result_buffer, const std::string& json)
	{

	}

	JSONType Parse(const std::string& json)
	{

	}

	void Parse(JSONType& result_buffer, const std::string& json)
	{
		bool initialized = false;
		bool reading_string = false;
		std::string string;
		std::string key;
		std::string value;
		bool valid = true;
		int line = 0;

		std::stack<JSONType*> obj_and_arr;
		
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
			COMMA
		};

		int expect = ARRAY | OBJECT;

		for (int i = 0; json[i] && valid; i++)
		{
			int got = -1;
			switch (json[i])
			{
			case ' ': continue;
			case '\t': continue;

			case '{': {
				if (!initialized && (OBJECT & expect))
				{
					result_buffer = JSONType(JSONType::object_t());
					obj_and_arr.push(&result_buffer);
					expect = STRING | KEY;

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
						value = string;
						(*obj_and_arr.top())[key] = value;
					}
				}

				reading_string = !reading_string;

				break;
			}

			case ',':
			{
				if (value.size())
				{
					expect = KEY;
					value = "";
					key = "";
					string = "";
				}

				break;
			}

			default: {
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