#include "orderAnalysis.h"
#include <iostream>
#include <sstream>

order orderAnalysis::analysis(const std::string &order)
{
	std::string jsonStr(
		"{ \"from_user\" : \"deng\" , \"to_user\" : \"admin\" , \"msg\" : \"fuck you\" }");
	using namespace boost::property_tree;
	ptree pt;
	std::stringstream ss(jsonStr);
	try
	{
		read_json(ss, pt);
	}
	catch (const ptree_error &err)
	{
		std::cout << err.what() << std::endl;
		return test;
	}
	try {
		std::cout << pt.get<std::string>("from") << std::endl;
	}
	catch (const ptree_error &err)
	{
		std::cout << err.what() << std::endl;
	}
	return test;
}

orderAnalysis::orderAnalysis()
{

}
orderAnalysis::~orderAnalysis()
{

}