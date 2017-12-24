#include "Edge.h"

Edge::Edge(glm::vec3 p1, glm::vec3 p2, Polygon* in_poly)
{
	static int id = 0;
	edge_id_ = id++;

	poly_id_ = in_poly->id_;
	in_poly_ = in_poly;

	dy_ = abs(p2.y - p1.y) + 1;
	if (p1.y < p2.y) {	// ���϶˵㣨y��С�ģ�
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