#include <stdio.h>
#include "Polygon.h"
#include "ActiveEdge.h"

Polygon::Polygon(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 color)
{
	static int static_id = 0;
	id_ = static_id++;
	alread_drawn = false;
	
	// ��ԭʼ��ʱ�붨����������������ƽ�淽��
	glm::vec3 normal = glm::cross(p2 - p1, p3 - p1);
	a_ = normal.x;
	b_ = normal.y;
	c_ = normal.z;
	d_ = (glm::dot(normal, p1)) * (-1);

	color_ = color;
	normal_ = glm::normalize(normal);

	// ����Ļy������С��������(ֱ��ȡ�������꣩
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

	//printf("Polygon: %d\n", id_);
	//printf("(%f, %f, %f) (%f, %f, %f) (%f,%f,%f)\n", p1.x, p1.y, p1.z,
		//p2.x, p2.y, p2.z,
		//p3.x, p3.y, p3.z);

	p1_world_ = p1;
	p2_world_ = p2;
	p3_world_ = p3;
	p1_screen_ = glm::vec2(p1.x, p1.y);
	p2_screen_ = glm::vec2(p2.x, p2.y);
	p3_screen_ = glm::vec2(p3.x, p3.y);


	//dy_ = p3_screen_.y - p1_screen_.y + 1;  // ???
	dy_ = p3_screen_.y - p1_screen_.y;
	y_min_ = int(p1_screen_.y+0.5);
	next_ = NULL;
	flag = false;
}

void Polygon::AddEdge(Edge* e1, Edge* e2, Edge* e3)
{
	e1_ = e1;
	e2_ = e2;
	e3_ = e3;
}

void Polygon::IntersectWithScanLine(int scan_y, bool& flag1, bool& flag2, bool& flag3)
{
	// �Ƿ���e1�ཻ
	if (scan_y >= e1_->y_up && scan_y < e1_->y_down)
		flag1 = true;
	// �Ƿ���e2�ཻ
	if (scan_y >= e2_->y_up && scan_y < e2_->y_down)
		flag2 = true;
	if (scan_y >= e3_->y_up && scan_y < e3_->y_down)
		flag3 = true;

	int r = int(flag1) + int(flag2) + int(flag3);
	if (r != 2 && r != 0)
	{
		printf("Polygon::Polygon Scan Line can only intersect with a polygon at two or zero edge.\n");
		exit(3);
	}
}

ActiveEdge* Polygon::GetLeftTwoEdgeWhenParallel()
{
	// �ҳ�e1 e2 e3 x_��С��������
	if (e1_->x_ > e2_->x_ && e1_->x_ > e3_->x_)
		return new ActiveEdge(e2_, e3_);
	else if (e2_->x_ > e1_->x_ && e2_->x_ > e3_->x_)
		return new ActiveEdge(e1_, e3_);
	else
		return new ActiveEdge(e1_, e2_);
}