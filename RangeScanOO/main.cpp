#include <stdio.h>
#include <map>
#include <vector>
#include <algorithm>

#ifdef _WIN32
    #include <glut.h>
#elif __linux
    #include <GL/glut.h>
#endif
// 扫描线由上到下扫，及y由小到大；

#include "ActiveEdge.h"
#include "RangeInter.h"

const int WIN_WIDTH = 400;
const int WIN_HEIGHT = 400;

// 分类多边形表
std::map<int, Edge*> ET;
// 分类边表
std::map<int, Polygon*> PT;
std::map<int, ActiveEdge*> AET;		// 描线的活化边表
std::map<int, ActivePolygon*> APT;	// 描线的活化多边形表

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
				else
					return NULL;
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
						// TODO 有边结束，且三条边都结束
						// 删除APT里last_scan_y的polygon，
						// 删除活化边表里last_scan_y poly_id的边。
						// 。。。
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
				//return true; 结束的边可能有多条
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
	glm::vec3 t1_p3 = glm::vec3(100, 300, 10);
	glm::vec3 t1_p2 = glm::vec3(300, 100, 10);
	// triangle 2
	glm::vec3 t2_p1 = glm::vec3(100, 100, 5);
	glm::vec3 t2_p2 = glm::vec3(300, 200, 5);
	glm::vec3 t2_p3 = glm::vec3(300, 300, 5);

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

bool PolygonIsInRange(ActivePolygon* polygon, int centerx, int cur_y)
{
	ActiveEdge* ae = GetActiveEdge(polygon->polygon_->id_, cur_y);  // 该多边形的active edge
	if (ae == NULL) {
		printf("can not find active edge according active polygon.\n");
		exit(1);
	}

	//if (centerx >= ae->xl_ && centerx <= ae->xr_)
	if((centerx - ae->xl_)*(centerx-ae->xr_) <= 0) // =?
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
		printf("GetRangeFromAET::cannot get range list.\n");
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
	printf("At scan line %d, ifEnd:%d.\n", cur_y, ifEnd);


	// 遍历PT，是否有多边形涉及当前扫描线
	// std::map<int, Polygon*>::iterator pt_it = PT.find(cur_y);
	std::map<int, Polygon*>::iterator pt_it = PT.begin();
	while (pt_it != PT.end())
	{
		Polygon* cur_polygon = pt_it->second;
		while (cur_polygon)
		{
			// 如果该多边形已经在活化多边形表里了
			//if (PolygonIsActive(cur_polygon->id_, cur_y - 1))
			if(PolygonIsNotEnd(cur_polygon->id_, cur_y - 1))
			{

				// TODO
				// 傻逼了，我为什么要把活化边对写在一起:)



			}
			// 该多边形不在活化多边形表里，是新的多边形
			else if (cur_y >= cur_polygon->p1_screen_.y && cur_y <= cur_polygon->p3_screen_.y)
			{
				printf("At scan line %d, Add new active polygon to APT.\n", cur_y);
				// pt_it多边形与当前扫描线相交
				// 把多边形放入活化多边形表
				ActivePolygon* active_polygon = new ActivePolygon(cur_polygon);
				InsertActivePolygonToAPT(active_polygon, cur_y);

				// 把该多边形与当前扫描线相交的边对，加入到活化边表里
				bool flag1, flag2, flag3;  // 是否与三条边相交
				flag1 = flag2 = flag3 = false;
				cur_polygon->IntersectWithScanLine(cur_y, flag1, flag2, flag3);
				if (!flag1) {
					// e2 e3
					ActiveEdge* active_edge = new ActiveEdge(cur_polygon->e2_, cur_polygon->e3_);
					InsertActiveEdgeToAET(active_edge, cur_y);
				}
				else if (!flag2) {
					// e1 e3
					ActiveEdge* active_edge = new ActiveEdge(cur_polygon->e1_, cur_polygon->e3_);
					InsertActiveEdgeToAET(active_edge, cur_y);
				}
				else if (!flag3) {
					// e1 e2
					ActiveEdge* active_edge = new ActiveEdge(cur_polygon->e1_, cur_polygon->e2_);
					InsertActiveEdgeToAET(active_edge, cur_y);
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
		for (int i = 0; i < range_list.size() - 1; i++)
		{
			RangeInter* left = range_list[i];
			RangeInter* right = range_list[i + 1];
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
					int centerx = (left->x_ + right->x_) / 2;
					if (PolygonIsInRange(polygon, centerx, cur_y))
					{
						if (max_z_polygon)
						{
							// 比较中点深度值
							ActiveEdge* ae = GetActiveEdge(polygon->polygon_->id_, cur_y);
							float cur_z = ae->zl_ + ae->dzx_*(centerx - ae->zl_);
							if (cur_z > max_z) {
								max_z = cur_z;
								max_z_polygon = polygon->polygon_;
							}

						}
						else if (!max_z_polygon) {
							ActiveEdge* ae = GetActiveEdge(polygon->polygon_->id_, cur_y);
							float cur_z = ae->zl_ + ae->dzx_*(centerx - ae->zl_);
							max_z = cur_z;
							max_z_polygon = polygon->polygon_;
						}
					}
					polygon = polygon->next_;
				}

				if (max_z_polygon) {
					// draw [left,right] with max_z_polygon's color
					printf("[y=%d] draw [%d, %d] with %d_polygon's color\n", cur_y, int(left->x_+0.5), int(right->x_+0.5), max_z_polygon->id_);
				}
				else {
					// 没有多边形在此区间
					// draw [left, right] with background color
					printf("[y=%d] draw [%d, %d] with bg's color\n", cur_y, int(left->x_+0.5), int(right->x_+0.5));
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
		printf("[y=%d] draw the whole line with bg color\n", cur_y);
	}

	// destroy last scaneline info in APT&AET
	if(cur_y > 0)
	{
		EraseScanlineInfoInAPT(cur_y - 1);
		EraseScanlineInfoInAET(cur_y - 1);
	}


}

void myDisplay() {
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 0.0, 0.0);
	glRectf(-0.5f, -0.5f, 0.5f, 0.5f);
	//glutSolidTeapot(1.0);
	glFlush();
}

int main(int argc, char* argv[]) {
	InitSceneData();
	for (int i = 0; i < WIN_HEIGHT; i++)
	{
		ScaneLine(i);
	}

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);

	glutInitWindowPosition(WIN_WIDTH, WIN_HEIGHT);
	glutInitWindowSize(200, 200);
	glutCreateWindow("range scane culling");

	glutDisplayFunc(myDisplay);
	glutMainLoop();
	return 0;

}
