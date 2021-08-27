#include "JSONParser.h"




int main()
{
	JSONParser parser;

	JSONType res;

	parser.ParseFile(res, "C:/Users/Professional/Documents/VisualStudio/JSONParser/assets/object.json");

	std::cout << res << std::endl;

	res["f3"] += -5.0;

	res["f5"] = !res["f5"];

	res["f6"] = std::string("Some text");

	std::cout << res;
}