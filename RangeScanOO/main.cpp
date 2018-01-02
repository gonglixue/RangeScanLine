#include <stdio.h>
#include <map>
#include <vector>
#include <algorithm>
#include <time.h>
#include <opencv2\opencv.hpp>
#include "glm/gtc/matrix_transform.hpp"


// 扫描线由上到下扫，及y由小到大；

#include "ActiveEdge.h"
#include "RangeInter.h"
#include "Mesh.h"

#define eps 0.6f

const int WIN_WIDTH = 400;
const int WIN_HEIGHT = 400;

// 分类多边形表
std::map<int, Edge*> ET;
// 分类边表
std::map<int, Polygon*> PT;
std::map<int, ActiveEdge*> AET;		// 描线的活化边表
std::map<int, ActivePolygon*> APT;	// 描线的活化多边形表
cv::Mat canvas = cv::Mat::zeros(WIN_HEIGHT, WIN_WIDTH, CV_8UC3);

// function declaration
void DrawInCVWindow(int y, int x_begin, int x_end, glm::vec3 color);
void DrawInCVWindow(int y, int x_begin, int x_end, Polygon* polygon);

glm::vec3 light_pos = glm::vec3(10, -10, -10);
glm::mat4 model = glm::mat4();
glm::vec3 light_color = glm::vec3(1, 1, 1);

std::vector<glm::vec3> color_table;

void InitColorTable()
{
	color_table.push_back(glm::vec3(255, 0, 0));
	color_table.push_back(glm::vec3(0, 255, 0));
	color_table.push_back(glm::vec3(0, 0, 255));
	color_table.push_back(glm::vec3(255, 255, 0));
	color_table.push_back(glm::vec3(0, 255, 255));
	color_table.push_back(glm::vec3(255, 0, 255));
	color_table.push_back(glm::vec3(0, 0, 0));
	color_table.push_back(glm::vec3(100, 0, 0));
	color_table.push_back(glm::vec3(0, 100, 0));
	color_table.push_back(glm::vec3(0, 0, 100));
	color_table.push_back(glm::vec3(100, 100, 0));
	color_table.push_back(glm::vec3(0, 100, 100));

}

void EraseScanlineInfoInAET(int y)
{
	std::map<int, ActiveEdge*>::iterator it;
	it = AET.find(y);
	if(it != AET.end())
	{
		ActiveEdge* ae = it->second;
		while(ae)
		{
			ActiveEdge* temp = ae->next_;
			free(ae);
			ae = temp;
		}

		AET.erase(it);
	}
}

void EraseScanlineInfoInAPT(int y)
{
	std::map<int, ActivePolygon*>::iterator it;
	it = APT.find(y);
	if(it != APT.end())
	{
		ActivePolygon* ap = it->second;
		while(ap)
		{
			ActivePolygon* temp = ap->next_;
			free(ap);

			ap = temp;
		}
		APT.erase(it);
	}
}

void InsertPolygonToPT(Polygon* polygon)
{
	//mymap.insert(std::pair<char, int>('a', 100));
	std::map<int, Polygon*>::iterator it;
	it = PT.find(polygon->y_min_);

	if (it != PT.end())
	{
		// PT中y_min_对应的链表不为空，把polygon插入到链表末尾
		Polygon* temp = it->second;
		while (temp->next_)
			temp = temp->next_;
		temp->next_ = polygon;

	}
	else {
		PT.insert(std::pair <int, Polygon*>(polygon->y_min_, polygon));
	}
}

void InsertEdgeToET(Edge* edge)
{
	std::map<int, Edge*>::iterator it;
	it = ET.find(edge->y_up);				// 以上端点y为键

	if (it != ET.end())
	{
		Edge* temp = it->second;
		while (temp->next_)
			temp = temp->next_;
		temp->next_ = edge;

	}
	else {
		ET.insert(std::pair<int, Edge*>(edge->y_up, edge));
	}
}

void InsertActivePolygonToAPT(ActivePolygon* active_polygon, int cur_scan_y)
{
	std::map<int, ActivePolygon*>::iterator it;
	it = APT.find(cur_scan_y);

	if (it != APT.end())
	{
		ActivePolygon* temp = it->second;
		while (temp->next_) {
			temp = temp->next_;
			if (temp->polygon_->id_ == active_polygon->polygon_->id_) {
				printf("该边多边形已存在于活化多边形表中\n");
				return;
			}
		}
		temp->next_ = active_polygon;
	}
	else
		APT.insert(std::pair<int, ActivePolygon*>(cur_scan_y, active_polygon));
}

void InsertActiveEdgeToAET(ActiveEdge* active_edge, int cur_san_y)
{
	std::map<int, ActiveEdge*>::iterator it;
	it = AET.find(cur_san_y);

	if (it != AET.end())
	{
		ActiveEdge* temp = it->second;
		while (temp->next_)
			temp = temp->next_;
		temp->next_ = active_edge;
	}
	else
		AET.insert(std::pair<int, ActiveEdge*>(cur_san_y, active_edge));
}

bool DeleteActiveEdgeInAET(int poly_id, int cur_scan_y)
{
	std::map<int, ActiveEdge*>::iterator it;
	it = AET.find(cur_scan_y);

	if (it != AET.end())
	{
		ActiveEdge* temp = it->second;
		if (temp->poly_id_ == poly_id) {
			// delete header
			it->second = temp->next_;
			free(temp);
		}
		else {
			ActiveEdge* temp_next = temp->next_;
			while (temp_next)
			{
				if (temp_next->poly_id_ == poly_id)
				{
					temp->next_ = temp_next->next_;
					free(temp_next);

					return true; // ?
				}
				temp = temp->next_;
				temp_next = temp->next_;
			}
		}
	}

	return false;
}

bool PolygonIsActive(int poly_id)
{
	// APT为空
	if (APT.empty()) {
		return false;
	}
	std::map<int, ActivePolygon*>::iterator pt_it = APT.begin();

	// 遍历APT
	while (pt_it != APT.end())
	{
		ActivePolygon* temp = pt_it->second;
		while (temp)
		{
			if (temp->polygon_->id_ == poly_id)
				return true;
			temp = temp->next_;
		}

		pt_it++;
	}

	return false;
}

ActivePolygon* PolygonIsActive(int poly_id, int last_scan_y)
{
	if (last_scan_y < 0)
		return NULL;
	if (APT.empty())
		return NULL;

	std::map<int, ActivePolygon*>::iterator pt_it;
	pt_it = APT.find(last_scan_y);

	if (pt_it != APT.end())
	{
		ActivePolygon* temp = pt_it->second;
		while (temp)
		{
			if (temp->polygon_->id_ == poly_id)
				return temp;
			temp = temp->next_;
		}
	}

	return NULL;
}

ActivePolygon* PolygonIsNotEnd(int poly_id, int scan_y)
{
	if (scan_y < 0)
		return NULL;
	if (APT.empty())
		return NULL;

	std::map<int, ActivePolygon*>::iterator apt_it;
	apt_it = APT.find(scan_y);

	if (apt_it != APT.end())
	{
		ActivePolygon* temp = apt_it->second;
		while (temp)
		{
			if (temp->polygon_->id_ == poly_id)
			{
				if (temp->dy_ > 0) //? >=0
					return temp;
				else {
					temp->polygon_->alread_drawn = true;
					return NULL;
				}
			}
			temp = temp->next_;
		}
	}

	return NULL;
}

// 检查上一个活化边表里，是否有边结束.有边结束了就返回true
bool CheckEndInAET(int last_scan_y)
{
	if (last_scan_y < 0 || APT.empty())
		return false;

	std::map<int, ActiveEdge*>::iterator aet_it;
	aet_it = AET.find(last_scan_y);

	if (aet_it != AET.end())
	{
		// check
		ActiveEdge* temp = aet_it->second;
		while (temp)
		{
			if (temp->dyl_ <= 0 || temp->dyr_ <= 0) {  // 有边要结束
													   // 判断该边所在的多边形是否在活化多边形表内
				//ActivePolygon* active_polygon = PolygonIsActive(temp->poly_id_, last_scan_y);
				ActivePolygon* active_polygon = PolygonIsNotEnd(temp->poly_id_, last_scan_y);
				// 如果所在多边形仍在活化，找新的相交对
				if (active_polygon)
				{
					bool flag1, flag2, flag3;
					flag1 = flag2 = flag3 = false;
					active_polygon->polygon_->IntersectWithScanLine(last_scan_y, flag1, flag2, flag3);
					// TODO 构建新的相交对,加到活化边表中。删除原来结束的边temp
					// 把temp的下一条边的data复制给temp，删除temp的下一条边:) 但temp边下个循环仍需处理
					//ActiveEdge* temp_next = temp->next_;
					//temp->CopyFrom(temp_next);
					//temp->next_ = temp_next->next_;
					//free(temp_next);

					if (!flag1 && !flag2 && !flag3) {
						// 有边结束，且三条边都结束

					}
					else if (!flag1) {
						ActiveEdge* active_edge = new ActiveEdge(active_polygon->polygon_->e2_, active_polygon->polygon_->e3_, last_scan_y+1);
						InsertActiveEdgeToAET(active_edge, last_scan_y + 1);
						//active_polygon->Update();
						ActivePolygon* change_edge_ap = new ActivePolygon(active_polygon);
						InsertActivePolygonToAPT(change_edge_ap, last_scan_y + 1);
					}
					else if (!flag2) {
						ActiveEdge* active_edge = new ActiveEdge(active_polygon->polygon_->e1_, active_polygon->polygon_->e3_, last_scan_y+1);
						InsertActiveEdgeToAET(active_edge, last_scan_y + 1);
						ActivePolygon* change_edge_ap = new ActivePolygon(active_polygon);
						InsertActivePolygonToAPT(change_edge_ap, last_scan_y + 1);
					}
					else if (!flag3) {
						ActiveEdge* active_edge = new ActiveEdge(active_polygon->polygon_->e1_, active_polygon->polygon_->e2_, last_scan_y+1);
						InsertActiveEdgeToAET(active_edge, last_scan_y + 1);
						ActivePolygon* change_edge_ap = new ActivePolygon(active_polygon);
						InsertActivePolygonToAPT(change_edge_ap, last_scan_y + 1);
					}
					else {
						printf("CheckEndInAET::scan line can only intersect with a polygon at two edges.\n");
						exit(3);
					}
					
				}

			}

			// 边不结束
			else {
				// 更新活化边信息与活化多边形信息，加入到当前扫描线的活化边表里
				ActiveEdge* new_ae = new ActiveEdge(temp); // 从temp更新
				InsertActiveEdgeToAET(new_ae, last_scan_y + 1);
				// 把对应的多边形，更新信息，加到活化多边形表里
				ActivePolygon* active_polygon = PolygonIsActive(temp->poly_id_, last_scan_y);
				if (active_polygon)
				{
					ActivePolygon* ap = new ActivePolygon(active_polygon);  // 从active_polygon更新
					InsertActivePolygonToAPT(ap, last_scan_y + 1);
				}
				else {
					printf("边未结束，多边形却不在活化表内.\n");
					exit(0);
				}
			}
			temp = temp->next_;
		}
		return false;
	}
	else {
		return false;
		//printf("CheckEndInAET::strange error.\n");
	}
}


void InitSceneData()
{
	// 定义两个三角形
	// triangle 1
	glm::vec3 t1_p1 = glm::vec3(50, 50, 10);
	glm::vec3 t1_p3 = glm::vec3(100, 200, 15);
	glm::vec3 t1_p2 = glm::vec3(300, 100, 10);
	// triangle 2
	glm::vec3 t2_p1 = glm::vec3(100, 100, 5);
	glm::vec3 t2_p2 = glm::vec3(300, 200, 5);
	glm::vec3 t2_p3 = glm::vec3(300, 300, 10);

	Polygon* p1 = new Polygon(t1_p1, t1_p2, t1_p3, glm::vec3(255, 0, 0));
	Polygon* p2 = new Polygon(t2_p1, t2_p2, t2_p3, glm::vec3(0,0,255));

	// 构造分类多边形表
	// 往ET里插入多边形
	InsertPolygonToPT(p1);
	InsertPolygonToPT(p2);

	// 构建边的数据结构
	Edge* e1 = new Edge(t1_p1, t1_p2, p1);
	Edge* e2 = new Edge(t1_p2, t1_p3, p1);
	Edge* e3 = new Edge(t1_p3, t1_p1, p1);

	Edge* e4 = new Edge(t2_p1, t2_p2, p2);
	Edge* e5 = new Edge(t2_p2, t2_p3, p2);
	Edge* e6 = new Edge(t2_p3, t2_p1, p2);

	p1->AddEdge(e1, e2, e3);
	p2->AddEdge(e4, e5, e6);

	InsertEdgeToET(e1);
	InsertEdgeToET(e2);
	InsertEdgeToET(e3);
	InsertEdgeToET(e4);
	InsertEdgeToET(e5);
	InsertEdgeToET(e6);


}

void InitSceneData(Mesh mesh)
{
	InitColorTable();
	// 遍历每个三角形
	for (int i = 0; i < mesh.indices_.size(); i++)
	{
		glm::vec3 p1, p2, p3;
		int indx, indy, indz;
		indx = mesh.indices_[i].x;
		indy = mesh.indices_[i].y;
		indz = mesh.indices_[i].z;

		p1 = mesh.vertices_[indx];
		p2 = mesh.vertices_[indy];
		p3 = mesh.vertices_[indz];

		Polygon* polygon = new Polygon(p1, p2, p3, color_table[i % 12]);
		InsertPolygonToPT(polygon);

		// 构建边数据结构
		Edge* e1 = new Edge(p1, p2, polygon);
		Edge* e2 = new Edge(p2, p3, polygon);
		Edge* e3 = new Edge(p3, p1, polygon);
		polygon->AddEdge(e1, e2, e3);

		InsertEdgeToET(e1);
		InsertEdgeToET(e2);
		InsertEdgeToET(e3);
	}

	
}

bool compare_range_inter(RangeInter* item1, RangeInter* item2)
{
	return item1->x_ < item2->x_;
}

ActiveEdge* GetActiveEdge(int poly_id, int cur_y)
{
	std::map<int, ActiveEdge*>::iterator it;
	it = AET.find(cur_y);
	if (it != AET.end())
	{
		ActiveEdge* temp = it->second;
		while (temp)
		{
			if (temp->poly_id_ == poly_id)
				return temp;
			temp = temp->next_;
		}
	}

	return NULL;
}

// 该多边形是否在给定区间里
bool PolygonIsInRange(ActivePolygon* polygon, RangeInter* left, RangeInter* right, int cur_y)
{
	int centerx = (left->x_ + right->x_) / 2;
	ActiveEdge* ae = GetActiveEdge(polygon->polygon_->id_, cur_y);  // 该多边形的active edge
	if (ae == NULL) {
		printf("can not find active edge according active polygon.\n");
		exit(1);
	}

	if (centerx > ae->xl_ && centerx < ae->xr_)
	{
		return true;
	}
	return false;

}

bool PolygonIsInRange(ActivePolygon* polygon, float centerx, int cur_y)
{
	ActiveEdge* ae = GetActiveEdge(polygon->polygon_->id_, cur_y);  // 该多边形的active edge
	if (ae == NULL) {
		printf("can not find active edge according active polygon.\n");
		exit(1);
	}

	//if (centerx >= ae->xl_ && centerx <= ae->xr_)
	//if((centerx - ae->xl_)*(centerx-ae->xr_) <= 0) // =?
	float ae_left, ae_right;
	if (ae->xl_ < ae->xr_) {
		ae_left = ae->xl_;
		ae_right = ae->xr_;
	}
	else {
		ae_right = ae->xl_;
		ae_left = ae->xr_;
	}
		
	//if((int(centerx+0.5)-int(ae_left+0.5)) * (int(centerx+0.5)-int(ae_right+0.5)) <= 0)
	if(centerx>=(ae_left-eps) && centerx<=(ae_right+eps))
	{
		return true;
	}
	return false;
}

std::vector<RangeInter*> GetRangeFromAET(int cur_y)
{
	std::vector<RangeInter*> range_list;
	range_list.push_back(new RangeInter(NULL, 0));  // 屏幕左交点

	std::map<int, ActiveEdge*>::iterator aet_it;
	aet_it = AET.find(cur_y);

	if (aet_it != AET.end())
	{
		ActiveEdge* temp = aet_it->second;
		while (temp)
		{
			ActivePolygon* active_polygon = PolygonIsActive(temp->poly_id_, cur_y);
			if (!active_polygon)
			{
				printf("GetRangeFromAET::unknown error.\n");
				exit(1);
			}
			else {
				RangeInter* item_l = new RangeInter(active_polygon, temp->xl_);
				RangeInter* item_r = new RangeInter(active_polygon, temp->xr_);

				range_list.push_back(item_l);
				range_list.push_back(item_r);
			}

			temp = temp->next_;
		}
	}
	else {
		//printf("GetRangeFromAET::cannot get range list.\n");
	}

	range_list.push_back(new RangeInter(NULL, WIN_WIDTH));
	// 区间排序
	std::sort(range_list.begin(), range_list.end(), compare_range_inter);
	return range_list;

}

void ScaneLine(int cur_y)
{
	// 如果有些边在这条扫描线结束了（上条扫描线活化边表里的边dy=0）
	bool ifEnd = CheckEndInAET(cur_y - 1);		// 根据相交是否结束构建当前扫描线的活化边、活化多边形
	//printf("At scan line %d, ifEnd:%d.\n", cur_y, ifEnd);


	// 遍历PT，是否有多边形涉及当前扫描线
	// std::map<int, Polygon*>::iterator pt_it = PT.find(cur_y);
	std::map<int, Polygon*>::iterator pt_it = PT.begin();
	while (pt_it != PT.end())
	{
		Polygon* cur_polygon = pt_it->second;
		while (cur_polygon)
		{
			if (cur_polygon->alread_drawn) {
				//printf("skip polygon already drawn.\n");
				cur_polygon = cur_polygon->next_;
				continue;
			}
			// 如果该多边形已经在活化多边形表里了
			//if (PolygonIsActive(cur_polygon->id_, cur_y - 1))
			if(PolygonIsNotEnd(cur_polygon->id_, cur_y - 1))
			{

				// TODO

			}
			// 该多边形不在活化多边形表里，是新的多边形
			else if (cur_y >= cur_polygon->p1_screen_.y && cur_y <= cur_polygon->p3_screen_.y)
			{
				//printf("At scan line %d, Add new active polygon to APT.\n", cur_y);


				// 把该多边形与当前扫描线相交的边对，加入到活化边表里
				bool flag1, flag2, flag3;  // 是否与三条边相交
				flag1 = flag2 = flag3 = false;
				cur_polygon->IntersectWithScanLine(cur_y, flag1, flag2, flag3);
				if (!flag2 && !flag2 && !flag3)
				{
					//// 和三条边都不相交，说明该面重合于扫描线，取以最左边的顶点为端点的两条边
					//ActiveEdge* active_edge = cur_polygon->GetLeftTwoEdgeWhenParallel();
					//InsertActiveEdgeToAET(active_edge, cur_y);
					// 与扫描线重合的面不活化
				}
				else if (!flag1) {
					// e2 e3
					ActiveEdge* active_edge = new ActiveEdge(cur_polygon->e2_, cur_polygon->e3_);
					InsertActiveEdgeToAET(active_edge, cur_y);
					// pt_it多边形与当前扫描线相交
					// 把多边形放入活化多边形表
					ActivePolygon* active_polygon = new ActivePolygon(cur_polygon);
					InsertActivePolygonToAPT(active_polygon, cur_y);
				}
				else if (!flag2) {
					// e1 e3
					ActiveEdge* active_edge = new ActiveEdge(cur_polygon->e1_, cur_polygon->e3_);
					InsertActiveEdgeToAET(active_edge, cur_y);
					// pt_it多边形与当前扫描线相交
					// 把多边形放入活化多边形表
					ActivePolygon* active_polygon = new ActivePolygon(cur_polygon);
					InsertActivePolygonToAPT(active_polygon, cur_y);
				}
				else if (!flag3) {
					// e1 e2
					ActiveEdge* active_edge = new ActiveEdge(cur_polygon->e1_, cur_polygon->e2_);
					InsertActiveEdgeToAET(active_edge, cur_y);
					// pt_it多边形与当前扫描线相交
					// 把多边形放入活化多边形表
					ActivePolygon* active_polygon = new ActivePolygon(cur_polygon);
					InsertActivePolygonToAPT(active_polygon, cur_y);
				}
				else {
					printf("Main::scan line can only intersect with a polygon at two edges.\n");
					exit(3);
				}

			}
		
			cur_polygon = cur_polygon->next_;
		}

		pt_it++;
	}

	// 获得区间
	std::vector<RangeInter*> range_list = GetRangeFromAET(cur_y);
	// 判断在每个区间内,当前活化的多边形in/out情况
	// 遍历每个区间
	if (range_list.size() > 2)
	{
		Polygon* last_polygon = NULL;
		for (int i = 0; i < range_list.size() - 1; i++)
		{
			RangeInter* left = range_list[i];
			RangeInter* right = range_list[i + 1];
			float centerx = (int(left->x_+0.5) + int(right->x_+0.5)) / 2.0;

			//if (fabs(left->x_ - right->x_) < 0.00001)  // remove duplicate
			if(int(left->x_+0.5) == int(right->x_+0.5))
				continue;
			//if ((int(left->x_ + 0.5) + 1) == int(right->x_ + 0.5)) {
			//	// 1像素宽的区间特殊chuli
			//	if (last_polygon) {
			//		printf("[y=%d] draw [%d, %d] with %d_polygon's color\n", cur_y, int(left->x_ + 0.5), int(right->x_ + 0.5), last_polygon->id_);
			//		DrawInCVWindow(cur_y, left->x_ + 0.5, right->x_ + 0.5, last_polygon);
			//		continue;
			//	}
			//}

			// 判断每个多边形，比较在此区间的z
			std::map<int, ActivePolygon*>::iterator it;
			it = APT.find(cur_y);
			if (it != APT.end())
			{
				ActivePolygon* temp_in_polygon_list;
				ActivePolygon* polygon = it->second;
				Polygon* max_z_polygon = NULL;
				float max_z;
				while (polygon)
				{




					
					if (PolygonIsInRange(polygon, centerx, cur_y))
					{
						if (max_z_polygon)
						{
							// 比较中点深度值
							ActiveEdge* ae = GetActiveEdge(polygon->polygon_->id_, cur_y);
							float cur_z = ae->zl_ + ae->dzx_*(centerx - ae->xl_);
							if (cur_z > max_z) {
								max_z = cur_z;
								max_z_polygon = polygon->polygon_;
							}

						}
						else if (!max_z_polygon) {
							ActiveEdge* ae = GetActiveEdge(polygon->polygon_->id_, cur_y);
							float cur_z = ae->zl_ + ae->dzx_*(centerx - ae->xl_);
							max_z = cur_z;
							max_z_polygon = polygon->polygon_;
						}
					}
					polygon = polygon->next_;
				}

				if (max_z_polygon) {
					// draw [left,right] with max_z_polygon's color
					// DrawInCVWindow(cur_y, left->x_, right->x_, max_z_polygon->color_);
					DrawInCVWindow(cur_y, left->x_+0.5, right->x_+0.5, max_z_polygon);
					last_polygon = max_z_polygon;
					//printf("[y=%d] draw [%d, %d] with %d_polygon's color\n", cur_y, int(left->x_+0.5), int(right->x_+0.5), max_z_polygon->id_);
				}
				else {
					// 没有多边形在此区间
					// draw [left, right] with background color
					//DrawInCVWindow(cur_y, left->x_+0.5, right->x_+0.5, glm::vec3(0, 0, 0));
					//printf("[y=%d] draw [%d, %d] with bg's color\n", cur_y, int(left->x_+0.5), int(right->x_+0.5));
				}

			}
			else {
				printf("There are many ranges but can not find active polygon in this line.\n");
				exit(1);
			}
		}
	}
	else {
		// draw background for the whole line
		//DrawInCVWindow(cur_y, 0, WIN_WIDTH - 1, glm::vec3(0,0,0));
		//printf("[y=%d] draw the whole line with bg color\n", cur_y);
	}

	// destroy last scaneline info in APT&AET
	if(cur_y > 0)
	{
		EraseScanlineInfoInAPT(cur_y - 1);
		EraseScanlineInfoInAET(cur_y - 1);
	}


}


int main(int argc, char* argv[]) {
	//InitSceneData();
	std::string filename;
	if (argc != 2)
		filename = "./bunny.obj";
	else
		filename = argv[1];

	clock_t start = clock();
	Mesh mesh(filename);
	model = glm::rotate(model, 0.5f, glm::vec3(0, 1, 0));
	model = glm::rotate(model, 3.1410f, glm::vec3(1, 0, 0));
	model = glm::rotate(model, 0.5f, glm::vec3(0, 0, 1));
	mesh.transform(model);
	InitSceneData(mesh);
	clock_t finish = clock();
	printf("face number: %d\n", mesh.indices_.size());
	printf("Init time: %lf ms\n", 1000.0*(finish - start) / CLOCKS_PER_SEC);
	
	start = clock();
	for (int i = 0; i < WIN_HEIGHT; i++)
	{
		ScaneLine(i);
	}
	finish = clock();
	printf("scan time: %lf ms\n", 1000.0*(finish - start) / CLOCKS_PER_SEC);

	cv::imshow("canvas", canvas);
	cv::imwrite("zbuffer.jpg", canvas);
	cv::waitKey(0);
	return 0;

}


void DrawInCVWindow(int y, int x_begin, int x_end, glm::vec3 color)
{
	for (int i = x_begin; i <= x_end; i++)
	{
		if (i >= WIN_WIDTH)
			break;
		if (i < 0)
			continue;
		// 边用蓝色色
		//if (i == x_begin || i == x_end)
		//{
		//	canvas.at<cv::Vec3b>(y, i) = cv::Vec3b(255, 255, 0);
		//}
		//else
			canvas.at<cv::Vec3b>(y, i) = cv::Vec3b(color.x, color.y, color.z);
	}

	//cv::imshow("canvas", canvas);
	//char fn[100];
	//sprintf(fn, "./mid_imgs2/%d.jpg", y);
	//cv::imwrite(fn, canvas);
}

void DrawInCVWindow(int y, int x_begin, int x_end, Polygon* polygon)
{
	glm::vec3 light_dir = glm::normalize(light_pos);
	float diffuse = glm::dot(polygon->normal_, light_dir);
	diffuse = (diffuse + 1) / 2.0;

	//diffuse = (diffuse > 0) ? diffuse : 0.01;
	if (diffuse < 0)
		printf("diffuse < 0");

	glm::vec3 diffuse_color = 255.0f * diffuse * light_color;

	//diffuse_color = polygon->color_;
	DrawInCVWindow(y, x_begin, x_end, diffuse_color);
	//DrawInCVWindow(y, x_begin, x_end, polygon->color_);

}