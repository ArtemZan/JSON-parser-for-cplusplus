#include "JSONParser.h"

std::ostream& undefined_ns::undefined::operator<<(std::ostream& stream)
{
	return stream << "undefined";
}


undefined_ns::undefined undefined;

JSONParser::JSONParser()
{

}

JSONParser::~JSONParser()
{
}
