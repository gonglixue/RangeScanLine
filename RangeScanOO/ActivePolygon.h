#pragma once
#include "Polygon.h"

class Polygon;

class ActivePolygon
{
public:
	Polygon* polygon_;

	int dy_;		// 多边形跨越的剩余扫描线数目
	ActivePolygon* next_;
	
	ActivePolygon(Polygon* polygon)
	{
		polygon_ = polygon;
		dy_ = polygon->dy_;

		next_ = NULL;
	}

	ActivePolygon(ActivePolygon* update_from)
	{
		polygon_ = update_from->polygon_;
		//dy_--;
		dy_ = update_from->dy_ - 1;

		next_ = NULL;
	}
};