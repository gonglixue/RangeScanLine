#include "ActiveEdge.h"

ActiveEdge::ActiveEdge(Edge* e1, Edge* e2)
{
	// 初始时与两条边交于一点，e1与e2应有共同的上端点 NOOOO!
	//if (abs(e1->x_ - e2->x_) < 0.0001)
	//{
	//	printf("ActiveEdge::ActiveEdge: two edges should have the same up end point.\n");
	//	exit(1);
	//}

	// 如果两边有相同的上端点，扫描线每向下处理一个单位，e1x的增量小于d2x，e1为左，增量小的为左
	if (fabs(e1->x_ - e2->x_) < 0.00001 && fabs(e1->y_up - e2->y_up))
	{
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
	}
	else {
		// 没有相同的上端点，则x_小的为左
		if (e1->x_ < e2->x_)
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
	}


	zl_ = edge_l_->z_up_;
	if (fabs(e1->in_poly_->c_) < 0.00001)
		dzx_ = 0;  // 沿x方向不改变z？
	else
		dzx_ = -1.0 * e1->in_poly_->a_ / e1->in_poly_->c_;

	if (fabs(e1->in_poly_->c_) < 0.00001)
		dzx_ = 0;
	else
		dzy_ = -1.0 * e1->in_poly_->b_ / e1->in_poly_->c_;

	poly_id_ = e1->in_poly_->id_;
	next_ = NULL;

}

ActiveEdge::ActiveEdge(Edge* e1, Edge* e2, int scan_y)
{
	// 初始时与两条边交于一点，e1与e2应有共同的上端点 NOOOO!
	//if (abs(e1->x_ - e2->x_) < 0.0001)
	//{
	//	printf("ActiveEdge::ActiveEdge: two edges should have the same up end point.\n");
	//	exit(1);
	//}

	// 如果两边有相同的上短线， 扫描线每向下处理一个单位，e1x的增量小于d2x，e1为左
	//if (fabs(e1->x_ - e2->x_) < 0.00001 && fabs(e1->y_up - e2->y_up))
	//{
	//	if (e1->dx_ < e2->dx_)
	//	{
	//		xl_ = e1->x_ + (scan_y - e1->y_up) * e1->dx_;
	//		dxl_ = e1->dx_;
	//		dyl_ = e1->dy_ - (scan_y - e1->y_up);

	//		xr_ = e2->x_ + (scan_y - e2->y_up)*e2->dx_;
	//		dxr_ = e2->dx_;
	//		dyr_ = e2->dy_ - (scan_y - e2->y_up);



	//		edge_l_ = e1;
	//		edge_r_ = e2;

	//	}
	//	else {
	//		xl_ = e2->x_ + (scan_y - e2->y_up) * e2->dx_;
	//		dxl_ = e2->dx_;
	//		dyl_ = e2->dy_ - (scan_y - e2->y_up);

	//		xr_ = e1->x_ + (scan_y - e1->y_up)*e1->dx_;
	//		dxr_ = e1->dx_;
	//		dyr_ = e1->dy_ - (scan_y - e1->y_up);

	//		edge_l_ = e2;
	//		edge_r_ = e1;
	//	}
	//}
	//else {
		// 没有相同的上端点，则【当前x_】小的为左
		float cur_x_e1, cur_x_e2;
		cur_x_e1 = e1->x_ + (scan_y - int(e1->y_up+0.5))*1.0 * e1->dx_;
		cur_x_e2 = e2->x_ + (scan_y - int(e2->y_up+0.5))*1.0 * e2->dx_;
		if (cur_x_e1 < cur_x_e2)  // e1为左
		{
			xl_ = cur_x_e1;
			dxl_ = e1->dx_;
			dyl_ = e1->dy_ - (scan_y - int(e1->y_up+0.5));

			xr_ = cur_x_e2;
			dxr_ = e2->dx_;
			dyr_ = e2->dy_ - (scan_y - e2->y_up);



			edge_l_ = e1;
			edge_r_ = e2;
		}
		else {
			xl_ = e2->x_ + (scan_y - e2->y_up) * e2->dx_;
			dxl_ = e2->dx_;
			dyl_ = e2->dy_ - (scan_y - e2->y_up);

			xr_ = e1->x_ + (scan_y - e1->y_up)*e1->dx_;
			dxr_ = e1->dx_;
			dyr_ = e1->dy_ - (scan_y - e1->y_up);

			edge_l_ = e2;
			edge_r_ = e1;
		}
	//}


	//zl_ = e1->z_up_;
	if (fabs(e1->in_poly_->c_) < 0.00001)
		dzx_ = 0;  // 沿x方向不改变z？
	else
		dzx_ = -1.0 * e1->in_poly_->a_ / e1->in_poly_->c_;

	if (fabs(e1->in_poly_->c_) < 0.00001)
		dzx_ = 0;
	else
		dzy_ = -1.0 * e1->in_poly_->b_ / e1->in_poly_->c_;
	//zl_ = e1->z_up_ + dzx_ * (xl_ - edge_l_->x_) + dzy_;
	zl_ = edge_l_->z_up_ + dzx_ * (xl_ - edge_l_->x_) + dzy_ * (scan_y - edge_l_->y_up);

	poly_id_ = e1->in_poly_->id_;
	next_ = NULL;

}

ActiveEdge::ActiveEdge(ActiveEdge* update_from_ae)
{
	dxl_ = update_from_ae->dxl_;
	xl_ = update_from_ae->xl_ + dxl_; 
	dyl_ = update_from_ae->dyl_ - 1;

	dxr_ = update_from_ae->dxr_;
	xr_ = update_from_ae->xr_ + dxr_;
	dyr_ = update_from_ae->dyr_ - 1;

	//zl_ = zl_ + dzx_ * dxl_ + dzy_;
	zl_ = update_from_ae->zl_ + update_from_ae->dzx_ * update_from_ae->dxl_ + update_from_ae->dzy_;
	dzx_ = update_from_ae->dzx_;
	dzy_ = update_from_ae->dzy_;
	poly_id_ = update_from_ae->poly_id_;
	
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