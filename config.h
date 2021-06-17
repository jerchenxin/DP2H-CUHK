//
// Created by ChenXin on 2021/5/7.
//

#ifndef DP2H_CONFIG_H
#define DP2H_CONFIG_H

#include "boost/dynamic_bitset.hpp"

#define MY_INFINITY 99999999

#define VIRTUAL_NUM 12

// may cause isolated vertex
#define THRESHOLD  0.3

#define EPSILON 0.2

#define CONST_C 1

#define ITERATION_NUM 1024

#define ALPHA 0.8

#define RANDOM_STEP 16

#define PATH_NUM 32

//#define SHOW_SUB_QUERY_TIME

#define USE_KMEANS

//#define USE_UNORDERED_MAP

#define NUM_OF_SECOND 8

#define USE_INV_LABEL

#define DELETE_OPT false

#define IS_USED_OPTION

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
