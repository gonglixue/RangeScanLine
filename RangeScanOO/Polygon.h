#pragma once
#include "Edge.h"
#include <glm/glm.hpp>
class Edge;
class Polygon
{
public:
	float a_, b_, c_, d_;		// ��ά�ռ�ƽ��ķ���ϵ��
	int id_;					// ����α��
	int dy_;					// ��Խ����ε�ɨ������
	glm::vec3 color_;			// �������ɫ

								// ������Ķ����Ϊ������
	glm::vec3 p1_world_, p2_world_, p3_world_;		// �����������������
	glm::vec2 p1_screen_, p2_screen_, p3_screen_;	// �����������Ļ����

	int y_min_;					// ���������Ļ�ռ������Сy����
	bool flag;					// ��ʼfalse

	Polygon* next_;
	Edge* e1_;
	Edge* e2_;
	Edge* e3_;

	Polygon(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 color = glm::vec3(255, 0, 0));
	void AddEdge(Edge* e1, Edge* e2, Edge* e3);
	void IntersectWithScanLine(int scan_y, bool& flag1, bool& flag2, bool& flag3);
};