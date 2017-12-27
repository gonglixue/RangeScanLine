#pragma once
#include "Polygon.h"
#include <glm/glm.hpp>
class Polygon;
class Edge
{
public:
	int x_;					// �ߵ��϶˵�x ����Ļ�ռ����꣨���Ͻ�Ϊԭ�㣩
	int y_up;					// �϶˵��y
	int y_down;					// �¶˵��y
	float z_up_;				// �϶˵��z
	int poly_id_;				// ���������id
	int edge_id_;
	float dx_;				// (1/k) ɨ����ÿ����ɨһ����λ��x�ı仯��
	int dy_;				// ��Խɨ������

	Edge* next_;
	Polygon* in_poly_;

	Edge(glm::vec3 p1, glm::vec3 p2, Polygon* in_poly);
};