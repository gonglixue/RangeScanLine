#pragma once
#include "Polygon.h"
#include <glm/glm.hpp>
class Polygon;
class Edge
{
public:
	float x_;					// 边的上端点x 的屏幕空间坐标（左上角为原点）
	float x_down_;				// 边的下端点
	float y_up;					// 上端点的y
	float y_down;					// 下端点的y
	float z_up_;				// 上端点的z
	int poly_id_;				// 所属多边形id
	int edge_id_;
	float dx_;				// (1/k) 扫描线每向下扫一个单位，x的变化量
	int dy_;				// 跨越扫描线数

	Edge* next_;
	Polygon* in_poly_;

	Edge(glm::vec3 p1, glm::vec3 p2, Polygon* in_poly);
};