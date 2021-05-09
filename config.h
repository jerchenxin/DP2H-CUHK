//
// Created by ChenXin on 2021/5/7.
//

#ifndef DP2H_CONFIG_H
#define DP2H_CONFIG_H

#include "boost/dynamic_bitset.hpp"

#define MY_INFINITY 99999999

#define VIRTUAL_NUM 12

#define USE_INV_LABEL

//#define DELETE_ADD_INFO

//#define USE_BIT_VECTOR
#define USE_INT

// bit vector is not efficient!!
#ifdef USE_BIT_VECTOR
typedef boost::dynamic_bitset<> LABEL_TYPE;
#endif

#ifdef USE_INT
typedef unsigned long long LABEL_TYPE;
#endif


#endif //DP2H_CONFIG_H
