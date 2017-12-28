#pragma once
#include "Edge.h"

class Edge;

class ActiveEdge
{
public:
	float xl_;			//左交点的x坐标
	float dxl_;
	int dyl_;			// 以该边跨越的扫描线为初值，以后每向下处理一条扫描线减1

	float xr_;
	float dxr_;
	int dyr_;

	float zl_;			// 左交点在世界坐标的深度值
	float dzx_;			// 扫描线向右走一个单位，多边形所在平面的深度增量
	float dzy_;			// 沿y方向向下移过一根扫描线时，多边形所在平面的深度增量。

	int poly_id_;		// 交点对所在的多边形的编号

						// should add ActivePolygon* active_polygon

	Edge* edge_l_;
	Edge* edge_r_;
	ActiveEdge* next_;

	ActiveEdge(Edge* e1, Edge* e2);
	ActiveEdge(Edge* e1, Edge* e2, int scan_y);
	ActiveEdge(ActiveEdge* update_from_ae);
	void CopyFrom(ActiveEdge* next);
};