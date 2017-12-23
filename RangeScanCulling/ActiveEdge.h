#ifndef __ACTIVE_EDGE_H__
#define __ACTIVE_EDGE_H__
#include "Edge.h"

//存放多边形与扫描线相交的边对。边对！是两条边！
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

	Edge* edge_l_;
	Edge* edge_r_;
	ActiveEdge* next_;

	ActiveEdge(Edge* e1, Edge* e2)
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

#endif // ! __ACTIVE_EDGE_H__
