#pragma once
#include <stdio.h>
#include <glm/glm.hpp>

//存放多边形与扫描线相交的边对。边对！是两条边！
struct Edge;
typedef struct Edge* ptrEdge;
struct ActiveEdge
{
	int xl_;			//左交点的x坐标
	float dxl_;
	int dyl_;			// 以该边跨越的扫描线为初值，以后每向下处理一条扫描线减1

	int xr_;
	float dxr_;
	int dyr_;

	float zl_;			// 左交点在世界坐标的深度值
	float dzx_;			// 扫描线向右走一个单位，多边形所在平面的深度增量
	float dzy_;			// 沿y方向向下移过一根扫描线时，多边形所在平面的深度增量。

	int poly_id_;		// 交点对所在的多边形的编号

						// should add ActivePolygon* active_polygon

	ptrEdge edge_l_;
	ptrEdge edge_r_;
	ActiveEdge* next_;

	ActiveEdge(ptrEdge e1, ptrEdge e2)
	{
		// 初始时与两条边交于一点，e1与e2应有共同的上端点 NOOOO!
		//if (abs(e1->x_ - e2->x_) < 0.0001)
		//{
		//	printf("ActiveEdge::ActiveEdge: two edges should have the same up end point.\n");
		//	exit(1);
		//}

		// 扫描线每向下处理一个单位，e1x的增量小于d2x，e1为左
		if (e1->dx_ < e2->dx_)
		{
			xl_ = e1->x_;
			dxl_ = e1->dx_;
			dyl_ = e1->dy_;

			xr_ = e2->x_;
			dxr_ = e2->dx_;
			dyr_ = e2->dy_;



			edge_l_ = e1;
			edge_r_ = e2;

		}
		else {
			xl_ = e2->x_;
			dxl_ = e2->dx_;
			dyl_ = e2->dy_;

			xr_ = e1->x_;
			dxr_ = e1->dx_;
			dyr_ = e1->dy_;

			edge_l_ = e2;
			edge_r_ = e1;
		}

		zl_ = e1->z_up_;
		dzx_ = -1.0 * e1->in_poly_->a_ / e1->in_poly_->c_;
		dzy_ = -1.0 * e1->in_poly_->b_ / e1->in_poly_->c_;

		poly_id_ = e1->in_poly_->id_;
		next_ = NULL;

	}

	ActiveEdge(ActiveEdge* update_from_ae)
	{
		dxl_ = update_from_ae->dxl_;
		xl_ = update_from_ae->xl_ + dxl_;
		dyl_--;

		dxr_ = update_from_ae->dxr_;
		xr_ = update_from_ae->xr_ + dxr_;
		dyr_--;

		zl_ = zl_ + dzx_ * dxl_ + dzy_;
		next_ = NULL;
	}


	void CopyFrom(ActiveEdge* next)
	{
		xl_ = next->xl_;
		dxl_ = next->dxl_;
		dyl_ = next->dyl_;

		xr_ = next->xr_;
		dxr_ = next->dxr_;
		dyr_ = next->dyr_;

		zl_ = next->zl_;
		dzx_ = next->dzx_;
		dzy_ = next->dzy_;

		poly_id_ = next->poly_id_;
		edge_l_ = next->edge_l_;  //?
		edge_r_ = next->edge_r_;

	}

};

struct Polygon;
typedef struct Polygon* ptrPolygon;
struct ActivePolygon
{
	ptrPolygon polygon_;

	int dy_;		// 多边形跨越的剩余扫描线数目
	ActivePolygon* next_;

	ActivePolygon(ptrPolygon polygon)
	{
		polygon_ = polygon;
		dy_ = polygon->dy_;

		next_ = NULL;
	}

	ActivePolygon(ActivePolygon* update_from)
	{
		polygon_ = update_from->polygon_;
		dy_--;

		next_ = NULL;
	}


};

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
	ptrPolygon in_poly_;

	Edge(glm::vec3 p1, glm::vec3 p2, ptrPolygon in_poly)
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

	Polygon(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 color = glm::vec3(255, 0, 0))
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

		if ((flag1 + flag2 + flag3) != 2 || (flag1 + flag2 + flag3) != 0)
		{
			printf("Scan Line can only intersect with a polygon at two or zero edge.\n");
			exit(3);
		}
	}
};