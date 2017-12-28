#pragma once
#include <glm/glm.hpp>
#include "ActivePolygon.h"

// 区间端点
struct RangeInter {
	ActivePolygon* belong_to_poly_;		// 所属多边形
	float x_;		// x坐标

	RangeInter(ActivePolygon* polygon, float x)
	{
		x_ = x;
		belong_to_poly_ = polygon;
	}
};