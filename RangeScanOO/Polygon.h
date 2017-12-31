#pragma once
#include "Edge.h"
#include <glm/glm.hpp>
class Edge;
class ActiveEdge;

class Polygon
{
public:
	float a_, b_, c_, d_;		// 三维空间平面的方程系数
	int id_;					// 多边形编号
	int dy_;					// 跨越多边形的扫描线数
	glm::vec3 color_;			// 多边形颜色
	glm::vec3 normal_;

								// 所输入的多边形为三角形
	glm::vec3 p1_world_, p2_world_, p3_world_;		// 三个顶点的世界坐标
	glm::vec2 p1_screen_, p2_screen_, p3_screen_;	// 三个顶点的屏幕坐标

	int y_min_;					// 多边形在屏幕空间里的最小y坐标
	bool flag;					// 初始false

	Polygon* next_;
	Edge* e1_;
	Edge* e2_;
	Edge* e3_;

	Polygon(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 color = glm::vec3(255, 0, 0));
	void AddEdge(Edge* e1, Edge* e2, Edge* e3);
	void IntersectWithScanLine(int scan_y, bool& flag1, bool& flag2, bool& flag3);
	ActiveEdge* GetLeftTwoEdgeWhenParallel();

};