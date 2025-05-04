#include "BarcodeCreatorCloud.h"
#include <memory>
// Radius
using namespace bc;

std::unique_ptr<bc::Baritem> CloudPointsBarcode::createBarcode(const CloudPoints* points)
{
	std::unique_ptr<Barcontainer> cont(new Barcontainer());
	if (points->size() > 0)
		processTypeF(points, cont.get());

	std::unique_ptr<bc::Baritem> it(cont->exractItem(0));
	return it;
}

void CloudPointsBarcode::processTypeF(const CloudPoints* points, Barcontainer* item)
{
	cloud = points;
	sortPixels();
	process(item);
}

//#include "../side/delaunator.hpp"
using Point3D = CloudPointsBarcode::CloudPoint;

Point3D operator+(const Point3D& p, const Point3D& v) {
	return { p.x + v.x, p.y + v.y, p.z + v.z };
}

Point3D operator-(const Point3D& p, const Point3D& v) {
	return { p.x - v.x, p.y - v.y, p.z - v.z };
}
bool operator==(const Point3D& p, const Point3D& v) {
	return p.x == v.x && p.y == v.y;
}

struct LineSegment {
	Point3D p1, p2;
};

// Calculates the cross product of two vectors.
double crossProduct(Point3D a, Point3D b)
{
	return a.x * b.y - a.y * b.x;
}

// Calculates the direction of a line segment relative to another line segment.
int direction(Point3D a, Point3D b, Point3D c)
{
	Point3D ab(b.x - a.x, b.y - a.y, 0);
	Point3D ac(c.x - a.x, c.y - a.y, 0);
	double cross = crossProduct(ab, ac);

	if (cross > 0) return 1;
	if (cross < 0) return -1;
	return 0;
}

bool onSegment(Point3D a, Point3D b, Point3D c)
{
	if (c.x <= std::max(a.x, b.x) && c.x >= std::min(a.x, b.x) &&
		c.y <= std::max(a.y, b.y) && c.y >= std::min(a.y, b.y)) {
		return true;
	}
	return false;
}


// Checks if two line segments intersect.
bool intersectLineLine(LineSegment a, LineSegment b)
{
	if (a.p1 == b.p1 || a.p1 == b.p2 || a.p2 == b.p1 || a.p2 == b.p2)
	{
		return false;
	}

	int d1 = direction(b.p1, b.p2, a.p1);
	int d2 = direction(b.p1, b.p2, a.p2);
	int d3 = direction(a.p1, a.p2, b.p1);
	int d4 = direction(a.p1, a.p2, b.p2);

	if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) &&
		((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0)))
	{
		return true;
	}

	if (d1 == 0 && onSegment(b.p1, b.p2, a.p1)) return true;
	if (d2 == 0 && onSegment(b.p1, b.p2, a.p2)) return true;
	if (d3 == 0 && onSegment(a.p1, a.p2, b.p1)) return true;
	if (d4 == 0 && onSegment(a.p1, a.p2, b.p2)) return true;

	return false;
}


void bc::CloudPointsBarcode::sortPixels()
{
	const size_t wid = (*cloud).size();

	// n(n+1)/2
	totalSize = wid * (wid + 1) / 2 - wid;

	//	if (totalSize > 500000)
	//		throw std::exception();

	float dist;
	PointIndexCov* data = new PointIndexCov[totalSize];
	sortedArr.reset(data);

	// Сичтаем расстояние между всеми соседними пикселями для каждого пикселя.
	// Чтобы не считать повтороно, от текущего проверяем только уникальные - в форме отражённой по вертикали буквы "L"

	int k = 0;

	for (size_t i = 0; i < wid - 1; ++i)
	{
		for (size_t j = i + 1; j < wid; ++j)
		{
			//			int offset = wid * i + j;

			dist = (*cloud)[i].distanse((*cloud)[j]);
			data[k++] = PointIndexCov(i, j, dist);
		}
	}
	assert(k == totalSize);

	// Тип не имеет занчение потому что соединяем не по яркости
	std::sort(data, data + k, [](const PointIndexCov& a, const PointIndexCov& b) {
		return a.dist < b.dist;
		});

	// ------------
	// Holes
	if (!useHolde)
		return;

	std::vector<PointIndexCov> newVals;
	newVals.push_back(data[0]);

	for (size_t i = 1; i < totalSize; i++)
	{
		const PointIndexCov& a = data[i];
		Point3D a1 = (*cloud)[a.points[0]];
		Point3D a2 = (*cloud)[a.points[1]];
		bool f = true;
		if (newVals.size() == 50)
			f = true;
		for (size_t j = 0; j < newVals.size(); j++)
		{
			const PointIndexCov& b = newVals[j];
			Point3D b1 = (*cloud)[b.points[0]];
			Point3D b2 = (*cloud)[b.points[1]];
			if (intersectLineLine({ a1, a2 }, { b1, b2 }))
			{
				f = false;
				break;
			}
		}

		if (f)
			newVals.push_back(a);
	}

	totalSize = newVals.size();
	sortedArr.reset(new PointIndexCov[totalSize]);
	std::copy(newVals.begin(), newVals.end(), sortedArr.get());

}

void bc::CloudPointsBarcode::sortTriangulate()
{
}


void bc::CloudPointsBarcode::process(Barcontainer* item)
{
	root = new Baritem(0, BarType::FLOAT32_1);
	item->addItem(root);
	if (useHolde)
	{
		processHold();
	}
	else
	{
		for (curIndexInSortedArr = 0; curIndexInSortedArr < totalSize; ++curIndexInSortedArr)
		{
			const PointIndexCov& val = sortedArr.get()[curIndexInSortedArr];
			processComp(val);
		}
	}

	root->barlines = std::move(components);
	clearIncluded();
}


void CloudPointsBarcode::processComp(const  CloudPointsBarcode::PointIndexCov& val)
{
	const int curIndex = val.points[0];
	CloudPoint curCloudPoint = (*cloud)[curIndex];
	bc::point curPoint(curCloudPoint.x, curCloudPoint.y);
	poidex curPoindex = curPoint.getLiner(BAR_MAX_WID);

	int nextIndex = val.points[1];
	CloudPoint nextCloudPoint = (*cloud)[nextIndex];
	bc::point nextPoint(nextCloudPoint.x, nextCloudPoint.y);
	poidex nextPoindex = nextPoint.getLiner(BAR_MAX_WID);

	barline* first = getComp(curPoindex);
	barline* connected = getComp(nextPoindex);

	if (first != nullptr)
	{
		//assert(first->m_end == 0);
		//если в найденном уже есть этот элемент
		//существует ли ребро вокруг
		if (connected != nullptr && first != connected)
		{
			//assert(connected->m_end == 0);

			barline* main = first;
			barline* sub = connected;
			// Attach
			if (first->start > connected->start)
			{
				main = connected;
				sub = first;
			}

			// for (size_t i = 0; i < sub->matr.size(); i++)
			// {
			// 	auto& pas = sub->matr[i];
			// 	bc::point p = pas.getPoint();
			// 	main->addCoord(p, pas.value);
			// 	setInclude(p.getLiner(BAR_MAX_WID), main);
			// }

			main->addChild(sub);
			sub->m_end = val.dist;
		}
		else if (connected == nullptr)
		{
			first->addCoord(nextPoint, nextCloudPoint.getScalar());
			setInclude(nextPoindex, first);
		}
	}
	else
	{
		// Ребро не создано или не получилось присоединить
		if (connected == nullptr)
		{
			connected = new barline(Barscalar(val.dist, BarType::FLOAT32_1), Barscalar(val.dist, BarType::FLOAT32_1));
			connected->initRoot(root);
			components.push_back(connected);
		}
		// assert(connected->m_end == 0);

		connected->addCoord(curPoint, curCloudPoint.getScalar());
		setInclude(curPoindex, connected);

		connected->addCoord(nextPoint, nextCloudPoint.getScalar());
		setInclude(nextPoindex, connected);
	}
}


struct HoleInfo
{
	barline* line = nullptr;
	int connesLeft = 0;

	HoleInfo(Barscalar start,Baritem* root )
	{
		line = new barline(start, start);
		line->initRoot(root);
	}

	bool holeStarted()
	{
		return connesLeft > 0;
	}

	bool holeEnded()
	{
		return line->m_end != 0;
	}

	void updateLeft()
	{
		int n = line->matr.size();
		connesLeft = n * (n - 1) / 2 - n;
		if (connesLeft == 0)
		{
			line->m_end = line->start;
		}
	}
	void addPoint(point p, Barscalar dist)
	{
		--connesLeft;
		line->addCoord(p, dist);
	}
};

struct GraphPoint;
struct GpathConnect
{
	GraphPoint* point = nullptr;
	float dist = 0;
};

struct GraphPoint
{
	point p;
	std::vector<GpathConnect> others;
	std::shared_ptr<HoleInfo> hole = nullptr;

	bool holeStarted() const
	{
		return hole.use_count() > 0;
	}
};


// std::function<void(const point&, const point&, bool)> CloudPointsBarcode::drawLine;
// std::function<void(bc::PloyPoints&, bool)> CloudPointsBarcode::drawPlygon;

struct Graph
{
	barmap<poidex, GraphPoint> graphPoints;
	bc::CloudPointsBarcode::ComponentsVector instakilled;

	bc::CloudPointsBarcode::ComponentsVector& vect;
	bc::Baritem* root;

	Graph(bc::Baritem* root, bc::CloudPointsBarcode::ComponentsVector& components)
		: vect(components),
		root(root)
	{ }

	~Graph()
	{
		for (size_t i = 0; i < instakilled.size(); i++)
		{
			delete instakilled[i];
		}
	}


	GraphPoint* getGrath(poidex p)
	{
		auto i = graphPoints.find(p);
		if (i != graphPoints.end())
		{
			return &(*i).second;
		}

		return nullptr;
	}

	//HoleInfo* getHole(poidex p)
	//{
	//	auto i = getGrath(p);
	//	if (i)
	//	{
	//		return i->hole.get();
	//	}

	//	return nullptr;
	//}

	GraphPoint& create(poidex p, Barscalar start)
	{
		GraphPoint& gp = graphPoints[p];
		gp.p = barvalue::getStatPoint(p);
		//gp.hole = std::make_shared<HoleInfo>(start);
		//gp.hole->addPoint(gp.p, start);

		return gp;
	}


	void connect(poidex p1, poidex p2, float dist)
	{
		GraphPoint* h1 = getGrath(p1);
		GraphPoint* h2 = getGrath(p2);
		//float d1 = h1->p.x - h2->p.x;
		//float d2 = h1->p.y - h2->p.y;
		//float dist = sqrtf(d1 * d1 + d2 * d2);
		assert(h1 != h2);
		assert(h1->p != h2->p);
		h1->others.push_back({ h2, dist });
		h2->others.push_back({ h1, dist });
	}

	void addConnect(poidex src, poidex newPoint, Barscalar start)
	{
		GraphPoint* srcHole = getGrath(src);
		addConnect(srcHole, newPoint, start);
	}

	void addConnect(GraphPoint* srcHole, poidex newPoint, Barscalar start)
	{
		//assert(!srcHole->holeStarted());

		GraphPoint& newPoi = graphPoints[newPoint];
		newPoi.p = barvalue::getStatPoint(newPoint);
		assert(srcHole->p != newPoi.p);

		srcHole->others.push_back({ &newPoi, start.getAvgFloat() });
		newPoi.others.push_back({ srcHole, start.getAvgFloat() });
	}

	static int dot(const point& a, const point& b)
	{
		return a.x * b.x + a.y * b.y;
	}
	static int det(const point& a, const point& b)
	{
		return a.x * b.y - a.y * b.x;
	}

	static float magnitude(const point& v) {
		return std::sqrt(v.x * v.x + v.y * v.y);
	}

	static double angleBetweenLines(const point& a, const point& common, const point& b, const bool rev = false)
	{
		if (a == b)
			return 0;

		point directionA(a - common);
		point directionB(b - common);
		//float dotf = dot(directionA, directionB);
		//float magnitudeA = magnitude(directionA);
		//float magnitudeB = magnitude(directionB);

		////double angle = std::acos(dot(directionA, directionB));
		//float cosine = dotf / (magnitudeA * magnitudeB);
		//return std::acos(cosine);

		float _dot = dot(directionA, directionB);
		float _det = det(directionA, directionB);
		const double pi = 3.14159265358979323846;
		float ang = std::atan2(_det, _dot);// atan2(y, x) or atan2(sin, cos)
		if (ang < 0)
		{
			ang += 2 * pi;
		}
		if (rev)
		{
			ang = 2 * pi - ang;
		}
		return ang;
		//return angle;
	}

	struct HoleRecur
	{
		HoleRecur(const point& dest) : dest(dest)
		{ }

		const point dest;
		std::vector<int> path;
		std::unordered_set<poidex> passedPoints;
		bool rev = false;

		double run(GpathConnect* startPoint)
		{
			passedPoints.clear();
			passedPoints.insert(startPoint->point->p.getLiner(BAR_MAX_WID));
			return findHoleRecurs(dest, startPoint); // p2 >-
		}

		double findHoleRecurs(const point& prev, const GpathConnect* const gcon)
		{
			const GraphPoint* con = gcon->point;
			point cp = con->p;
			bool lrev = rev;

			std::vector<int> indexes(con->others.size());
			for (int i = 0; i < con->others.size(); i++)
				indexes[i] = i;

			std::sort(indexes.begin(), indexes.end(), [prev, cp, &con, lrev](int ai, int bi)
				{
					const GpathConnect& a = con->others[ai];
			const GpathConnect& b = con->others[bi];
			double angle1 = angleBetweenLines(prev, con->p, a.point->p, lrev);
			double angle2 = angleBetweenLines(prev, con->p, b.point->p, lrev);
			return angle1 < angle2;
				});

			for (auto& i : indexes)
			{
				auto& p = con->others[i];
				point curVec(p.point->p);
				//assert(curVec != con->p);
				//assert(con == gcon->point);

				if (curVec == prev)
					continue;

				const poidex pind = curVec.getLiner(BAR_MAX_WID);
				if (passedPoints.count(pind) > 0)
					return 0;

				if (curVec == dest)
					return gcon->dist;

				if (p.point->others.size() > 1)
				{
					passedPoints.insert(pind);
					path.push_back(i);
					auto r = findHoleRecurs(con->p, &p);
					if (r != 0)
						return r + gcon->dist;
					else
						path.pop_back();
				}
			}

			return 0;
		}
	};


	bool findHole(poidex p1, poidex p2, float dist)
	{
		GraphPoint* h1 = getGrath(p1);
		GraphPoint* h2 = getGrath(p2);

		GpathConnect cong1{ h1, 0 }; // dist p1 - p2
		//GpathConnect cong2{ h2, 0 }; // dist p1 - p2
		HoleRecur hr(h2->p);

		double dist1 = hr.run(&cong1); // p2 >----< p1. Starts with p1 others

		std::vector<int> paths = std::move(hr.path);
		hr.rev = true;
		double dist2 = hr.run(&cong1); // p2 >----< p1. Starts with p1 others
		if (dist2 < dist1)
		{
			dist1 = dist2;
			paths = std::move(hr.path);
		}

		if (dist1 == 0)
			return false;

		GpathConnect& con = cong1;

		PloyPoints pew;
		pew.push_back(h2->p);
		pew.push_back(h1->p);

		std::shared_ptr<HoleInfo> main = std::make_shared<HoleInfo>(dist, root);

		for (auto& p : paths)
		{
			con = con.point->others[p];
			con.point->hole = main;
			main->addPoint(con.point->p, con.dist);
			pew.push_back(con.point->p);
		}

		h1->hole = main;
		main->addPoint(h1->p, dist);

		h2->hole = main;
		main->addPoint(h2->p, dist);

		main->updateLeft();
		if (main->holeEnded())
		{
			instakilled.push_back(main->line);
		}
		else
			vect.push_back(main->line);

		//if (CloudPointsBarcode::drawPlygon)
		//	CloudPointsBarcode::drawPlygon(pew, true);

		return true;
		/*double dist2 = attachRecurs(h1->p, &cong2);

		return dist1 < dist2 ? h1->hole : h2->hole;*/
	}


	//void attach(HoleInfo* first, HoleInfo* second)
	//{
	//	barline* main = first->line;
	//	barline* sub = second->line;
	//	// Attach
	//	if (first->line->start > second->line->start)
	//	{
	//		main = second->line;
	//		sub = first->line;
	//		first->line = main;
	//	}
	//	else
	//	{
	//		second->line = main;
	//	}

	//	for (size_t i = 0; i < sub->matr.size(); i++)
	//	{
	//		auto& pas = sub->matr[i];
	//		bc::point p = pas.getPoint();
	//		main->addCoord(p, pas.value);
	//		getGrath(pas.getIndex())->hole->line = main;
	//	}
	//	delete sub;
	//}
};

void CloudPointsBarcode::processHold()
{
	Graph grath(root, components);

	for (curIndexInSortedArr = 0; curIndexInSortedArr < totalSize; ++curIndexInSortedArr)
	{
		const PointIndexCov& val = sortedArr.get()[curIndexInSortedArr];

		//const buint curIndex = val.points[0];
		CloudPoint curCloudPoint = (*cloud)[val.points[0]];
		bc::point curPoint(curCloudPoint.x, curCloudPoint.y);
		poidex curPoindex = curPoint.getLiner(BAR_MAX_WID);

		//int unextIndex = val.points[1];
		CloudPoint nextCloudPoint = (*cloud)[val.points[1]];
		bc::point nextPoint(nextCloudPoint.x, nextCloudPoint.y);
		poidex nextPoindex = nextPoint.getLiner(BAR_MAX_WID);

		GraphPoint* first = grath.getGrath(curPoindex);
		GraphPoint* connected = grath.getGrath(nextPoindex);

		//if (drawLine)
		{
			//drawLine(nextPoint, curPoint, false);
			//using namespace std::chrono_literals;
			//std::this_thread::sleep_for(200ms);
		}

		if (first != nullptr)
		{
			//assert(first->line->m_end == 0);
			//если в найденном уже есть этот элемент
			//существует ли ребро вокруг
			if (connected != nullptr)
			{
				if (first == connected)
				{
					if (first->hole->line->m_end != 0)
					{
						// Дыра живая
						first->hole->line->m_end = val.dist;
					}
					else
					{
						// Дыры нет, созадём
						assert(connected->hole == nullptr);
						//if (drawLine)
						//	drawLine(curPoint, nextPoint, true);
						using namespace std::chrono_literals;
						//std::this_thread::sleep_for(1000ms);
						bool h = grath.findHole(curPoindex, nextPoindex, val.dist);
						//endPoints();
						assert(h);
					}
				}
				else
				{
					//assert(connected->hole == nullptr);
					//if (drawLine)
					//	drawLine(curPoint, nextPoint, true);
					using namespace std::chrono_literals;
					//std::this_thread::sleep_for(1000ms);

					barline* fLines[2];
					fLines[0] = first->hole ? first->hole->line : nullptr;
					fLines[1] = connected->hole ? connected->hole->line : nullptr;
					bool h = grath.findHole(curPoindex, nextPoindex, val.dist);
					if (h)
					{
						barline* newHole = connected->hole->line;
						assert(newHole);
						assert(newHole != fLines[1]);

						if (fLines[0] == fLines[1])
							fLines[1] = nullptr;

						for (auto* p : fLines)
						{
							if (p == nullptr)
								continue;

							auto* parent = p->getParent();
							while (parent)
							{
								p = parent;
							}

							if (p != newHole)
								newHole->addChild(p);
						}
					}
				}

				grath.connect(curPoindex, nextPoindex, val.dist);
			}
			else
			{
				grath.addConnect(curPoindex, nextPoindex, nextCloudPoint.getScalar());
			}
		}
		else
		{
			// Ребро не создано или не получилось присоединить
			if (connected == nullptr)
			{
				grath.create(nextPoindex, nextCloudPoint.getScalar());
			}
			//else
			//	assert(connected->line->m_end == 0);

			grath.addConnect(nextPoindex, curPoindex, curCloudPoint.getScalar());
		}
	} // For!
}

void CloudPointsBarcode::clearIncluded()
{
	components.clear();
	included.clear();
	sortedArr.reset(nullptr);
}
