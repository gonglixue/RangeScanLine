#ifndef __ACTIVE_POLYGON_H__
#define __ACTIVE_POLYGON_H__
#include <glm\glm.hpp>
#include "Polygon.h"
struct ActivePolygon
{
	Polygon* polygon_;

	int dy_;		// 多边形跨越的剩余扫描线数目
	ActivePolygon* next_;

	ActivePolygon(Polygon* polygon)
	{
		polygon_ = polygon;
		dy_ = polygon->dy_;

		next_ = NULL;
	}


};

#endif // !__ACTIVE_POLYGON_H__
