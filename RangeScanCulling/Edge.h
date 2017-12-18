#ifndef __EDGE_H__
#define __EDGE_H__
#include <glm\glm.hpp>

// 投影多边形边界
struct Edge	
{
	int x_;					// 边的上端点x 的屏幕空间坐标（左上角为原点）
	int y_up;					// 上端点的y
	int y_down;					// 下端点的y
	float z_up_;				// 上端点的z
	int poly_id_;				// 所属多边形id
	int edge_id_;
	float dx_;				// (1/k) 扫描线每向下扫一个单位，x的变化量
	int dy_;				// 跨越扫描线数

	Edge* next_;
	Polygon* in_poly_;

	Edge(glm::vec3 p1, glm::vec3 p2, Polygon* in_poly)
	{
		static int id = 0;
		edge_id_ = id++;

		poly_id_ = in_poly->id_;
		in_poly_ = in_poly;

		dy_ = abs(p2.y - p1.y) + 1;
		if (p1.y < p2.y) {	// 找上端点（y更小的）
			x_ = p1.x;
			y_up = p1.y;
			y_down = p2.y;
			z_up_ = p1.z;
		}
		else {
			x_ = p2.x;
			y_up = p2.y;
			y_down = p1.y;
			z_up_ = p2.z;
		}

		if ((p2.x - p1.x) != 0) {
			float k = (p2.y - p1.y) / (p2.x - p1.x);
			dx_ = 1.0 / k;
		}
		else {
			dx_ = 0;
		}

	}
};
#endif // ! __EDGE_H__

