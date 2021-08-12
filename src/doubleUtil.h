#ifndef DOUBLE_UTIL_H
#define DOUBLE_UTIL_H

union _double{
	double d;
	long long l;
};

//equality comparison of double will be compiled wrongly by -O3
//change to binary comparison to avoid this bug
#define DOUBLE2LONG(x) (((union _double)x).l)

#endif
