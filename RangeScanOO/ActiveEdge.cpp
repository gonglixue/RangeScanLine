#include "ActiveEdge.h"

ActiveEdge::ActiveEdge(Edge* e1, Edge* e2)
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

ActiveEdge::ActiveEdge(ActiveEdge* update_from_ae)
{
	dxl_ = update_from_ae->dxl_;
	xl_ = update_from_ae->xl_ + dxl_;  //四舍五入
	dyl_ = update_from_ae->dyl_ - 1;

	dxr_ = update_from_ae->dxr_;
	xr_ = update_from_ae->xr_ + dxr_;
	dyr_ = update_from_ae->dyr_ - 1;

	zl_ = zl_ + dzx_ * dxl_ + dzy_;
	next_ = NULL;
}

void ActiveEdge::CopyFrom(ActiveEdge* next)
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