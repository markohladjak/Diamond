/*
 * config.hpp
 *
 *  Created on: Mar 11, 2021
 *      Author: rem
 */

#ifndef INCLUDE_CONFIG_HPP_
#define INCLUDE_CONFIG_HPP_

#include <WString.h>

namespace diamon
{

class Config {
public:
	static String PWD;
	static String HTML_PREFIX;
};

String Config::PWD = "/nodescontroller";
String Config::HTML_PREFIX = "/html";

}

#endif /* INCLUDE_CONFIG_HPP_ */
