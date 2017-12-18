#ifndef __ACTIVE_POLYGON_H__
#define __ACTIVE_POLYGON_H__
#include <glm\glm.hpp>
#include "Polygon.h"
struct ActivePolygon
{
	Polygon* polygon_;

	int dy_;		// ����ο�Խ��ʣ��ɨ������Ŀ
	ActivePolygon* next_;

	ActivePolygon(Polygon* polygon)
	{
		polygon_ = polygon;
		dy_ = polygon->dy_;

		next_ = NULL;
	}


};

#endif // !__ACTIVE_POLYGON_H__
