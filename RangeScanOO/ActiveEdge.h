#pragma once
#include "Edge.h"

class Edge;

class ActiveEdge
{
public:
	float xl_;			//�󽻵��x����
	float dxl_;
	int dyl_;			// �Ըñ߿�Խ��ɨ����Ϊ��ֵ���Ժ�ÿ���´���һ��ɨ���߼�1

	float xr_;
	float dxr_;
	int dyr_;

	float zl_;			// �󽻵���������������ֵ
	float dzx_;			// ɨ����������һ����λ�����������ƽ����������
	float dzy_;			// ��y���������ƹ�һ��ɨ����ʱ�����������ƽ������������

	int poly_id_;		// ��������ڵĶ���εı��

						// should add ActivePolygon* active_polygon

	Edge* edge_l_;
	Edge* edge_r_;
	ActiveEdge* next_;

	ActiveEdge(Edge* e1, Edge* e2);
	ActiveEdge(Edge* e1, Edge* e2, int scan_y);
	ActiveEdge(ActiveEdge* update_from_ae);
	void CopyFrom(ActiveEdge* next);
};