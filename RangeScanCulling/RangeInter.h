#ifndef __RANGE_INTER_H__
#define __RANGE_INTER_H__
#include <glm\glm.hpp>
#include "ActivePolygon.h"

// ����˵�
struct RangeInter {
	ActivePolygon* belong_to_poly_;		// ���������
	int x_;		// x����

	RangeInter(ActivePolygon* polygon, int x)
	{
		x_ = x;
		belong_to_poly_ = polygon;
	}
};

#endif