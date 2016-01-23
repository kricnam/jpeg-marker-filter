/*
 * jfif_filter.h
 *
 *  Created on: Jan 23, 2016
 */

#ifndef JFIF_FILTER_H_
#define JFIF_FILTER_H_
#include <string>
// filter away no standard markers of JFIF in the input string,
// which is the content of a jfif file in binary mode
// the return string is the binary data of the picture
// without no-standard marks.
std::string filter_stream(const std::string &strIn);
#endif /* JFIF_FILTER_H_ */
