#pragma once
#include "Edge.h"
#include <glm/glm.hpp>

struct Polygon
{
	float a_, b_, c_, d_;		// 三维空间平面的方程系数
	int id_;					// 多边形编号
	int dy_;					// 跨越多边形的扫描线数
	glm::vec3 color_;			// 多边形颜色

	// 所输入的多边形为三角形
	glm::vec3 p1_world_, p2_world_, p3_world_;		// 三个顶点的世界坐标
	glm::vec2 p1_screen_, p2_screen_, p3_screen_;	// 三个顶点的屏幕坐标
	
	int y_min_;					// 多边形在屏幕空间里的最小y坐标
	bool flag;					// 初始false

	Polygon* next_;
	Edge* e1_;
	Edge* e2_;
	Edge* e3_;

	Polygon(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 color=glm::vec3(255,0,0))
	{
		
		static int static_id = 0;
		id_ = static_id++;
		// 按屏幕y坐标由小到大排序(直接取世界坐标）
		// p1.y < p2.y < p3.y
		if (p1.y > p2.y) {
			glm::vec3 temp = p1;
			p1 = p2;
			p2 = temp;
		}
		if (p1.y > p3.y) {
			glm::vec3 temp = p1;
			p1 = p3;
			p3 = temp;
		}
		if (p2.y > p3.y) {
			glm::vec3 temp = p2;
			p2 = p3;
			p3 = temp;
		}

		printf("Polygon: %d\n", id_);
		printf("(%f, %f, %f) (%f, %f, %f) (%f,%f,%f)\n", p1.x, p1.y, p1.z,
			p2.x, p2.y, p2.z,
			p3.x, p3.y, p3.z);

		p1_world_ = p1;
		p2_world_ = p2;
		p3_world_ = p3;
		p1_screen_ = glm::vec2(p1.x, p1.y);
		p2_screen_ = glm::vec2(p2.x, p2.y);
		p3_screen_ = glm::vec2(p3.x, p3.y);

		// 求解平面方程
		glm::vec3 normal = glm::cross(p3 - p1, p2 - p1);
		a_ = normal.x;
		b_ = normal.y;
		c_ = normal.z;
		d_ = (glm::dot(normal, p1)) * (-1);

		color_ = color;
		dy_ = p3_screen_.y - p1_screen_.y + 1;  // ???
		y_min_ = p1_screen_.y;
		next_ = NULL;
		flag = false;
	}

	void AddEdge(Edge* e1, Edge* e2, Edge* e3)
	{
		e1_ = e1;
		e2_ = e2;
		e3_ = e3;
	}

	void IntersectWithScanLine(int scan_y, bool& flag1, bool& flag2, bool& flag3)
	{
		// 是否与e1相交
		if (scan_y >= e1_->y_up && scan_y <= e1_->y_down)
			flag1 = true;
		// 是否与e2相交
		if (scan_y >= e2_->y_up && scan_y <= e2_->y_down)
			flag2 = true;
		if (scan_y >= e3_->y_up && scan_y <= e3_->y_down)
			flag3 = true;

		if ((flag1 + flag2 + flag3) != 2 || (flag1+flag2+flag3)!=0)
		{
			printf("Scan Line can only intersect with a polygon at two or zero edge.\n");
			exit(3);
		}
	}
};
