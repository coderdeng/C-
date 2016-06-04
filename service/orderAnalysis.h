#pragma once
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/noncopyable.hpp>

enum order {
	test
};

class orderAnalysis : public boost::noncopyable
{
public:
	static order analysis(const std::string &order);
private:
	orderAnalysis();
	~orderAnalysis();
};
