#ifndef SKIP_M_INC
#pragma once

#include "barstrucs.h"

#include "../extra/include_py.h"
#include "../extra/include_cv.h"

#include <math.h>
#include <string>
#include <vector>
#include <unordered_set>
#include <stack>
#include <limits.h>
#endif
MEXP namespace bc
{
	struct barline;
	using barlinevector = std::vector<barline*>;

	class BarclinesHolder
	{
	public:
		barlinevector barlines;
	};


	struct ShortPoint
	{
		short x, y;
		ShortPoint(short x, short y) : x(x), y(y)
		{ }

		short diff(int a)
		{
			if (a > 0)
				return 1;
			else if (a < 0)
				return -1;
			else
				return 0;
		}
	};

	struct EXPORT barline
	{
		BarclinesHolder* root = nullptr;
		// Graph
		std::vector<uint> childrenId;
		uint id = UINT_MAX;
		uint parentId = UINT_MAX;

		// Binary mask
		bc::barvector matr;

		// 3d barcode
		barcounter* bar3d = nullptr;

		// Main params
		Barscalar start;
		//Barscalar len;
		Barscalar m_end;
		//		int matWid;
	private:
		// System member
		bool isCopy = false;

	public:
		//		int dep = 0;

		barline() : root(nullptr)
		{
		}

		barline(Barscalar _start, Barscalar _end, int = 0, barcounter* _barc = nullptr, size_t coordsSize = 0) :
			start(_start), m_end(_end)
			//			  ,matWid(wid)
		{
			matr.reserve(coordsSize);
			bar3d = _barc;
			//			assert(matWid != 0);
		}

		void initRoot(BarclinesHolder* root)
		{
			assert(this->root == nullptr);

			this->root = root;
			id = root->barlines.size();
			root->barlines.push_back(this);
		}

		////copy const
		//barline(barline const& obj)
		//{
		//	this->start = obj.start;
		//	this->m_end = obj.m_end;
		//	//			this->matWid = obj.matWid;
		//	this->parentId = obj.parentId;
		//	this->childrenId = obj.childrenId;
		//	this->matr = obj.matr;
		//	this->isCopy = true;

		//	if (obj.bar3d != nullptr)
		//	{
		//		this->bar3d = new barcounter();
		//		this->bar3d->insert(this->bar3d->begin(), obj.bar3d->begin(), obj.bar3d->end());
		//	}
		//}

		//// copy
		//void operator=(barline const& obj)
		//{
		//	this->start = obj.start;
		//	this->m_end = obj.m_end;
		//	//			this->matWid = obj.matWid;

		//	this->parentId = obj.parentId;
		//	this->childrenId = obj.childrenId;
		//	this->matr = obj.matr;

		//	this->isCopy = true;

		//	if (obj.bar3d != nullptr)
		//	{
		//		this->bar3d = new barcounter();
		//		this->bar3d->insert(this->bar3d->begin(), obj.bar3d->begin(), obj.bar3d->end());
		//	}
		//}

		bool operator==(barline const& obj)
		{
			//			assert(this->start == obj.start);
			//			assert(this->m_end ==  obj.m_end);
			//			assert(matr.size() == obj.matr.size());

			return this->start == obj.start && this->m_end == obj.m_end && matr.size() == obj.matr.size();
			//			return this->numberInParent = obj.numberInParent;
		}

		// move costr
		barline(barline&& obj)
		{
			this->start = obj.start;
			this->m_end = obj.m_end;
			//			this->matWid = obj.matWid;

			this->id = std::exchange(obj.id, UINT_MAX);
			this->parentId = std::exchange(obj.parentId, UINT_MAX);
			this->childrenId = std::move(obj.childrenId);
			this->isCopy = false;
			matr = std::move(obj.matr);

			this->bar3d = obj.bar3d;
			obj.bar3d = nullptr;
		}

		// move assign
		void operator=(barline&& obj)
		{
			this->start = obj.start;
			this->m_end = obj.m_end;
			//			this->matWid = obj.matWid;
			this->isCopy = false;
			this->id = std::exchange(obj.id, UINT_MAX);
			this->parentId = std::exchange(obj.parentId, UINT_MAX);
			this->childrenId = std::move(obj.childrenId);
			this->isCopy = false;
			matr = std::move(obj.matr);

			this->bar3d = obj.bar3d;
		}

		~barline()
		{
			// canBeDeleted - у копии, копия не может удалять детей. ј оригинал может
			delete bar3d;
		}

		//		int getWid()
		//		{
		//			return matWid;
		//		}

		const barvector& getMatrix() const
		{
			return matr;
		}
		barvector& getMatrix()
		{
			return matr;
		}

		size_t getMatrixCount() const
		{
			return matr.size();
		}

		//bc::Component *comp;
		//    cv::Mat binmat;
#ifdef USE_OPENCV

		cv::Mat getCvMat(const cv::Size& s)
		{
			cv::Mat m = cv::Mat::zeros(s.height, s.width, CV_8UC1);
			for (auto it = matr.begin(); it != matr.end(); ++it) {
				m.at<uchar>(it->getY(), it->getX()) = it->value.data.b1;
			}
			return m;
		}
		//void setFromCvMat(cv::Mat& mat)
		//{
		//	matr.clear();
		//	mat.forEach([m = matr](uchar& pixel, const int* pos) -> void {
		//		m->push_back(barvalue(pos[0], pos[1], pixel, mat.cols)); });
		//}
		cv::Rect getCvRect()
		{
			auto r = getBarRect();
			return cv::Rect(r.x, r.y, r.width, r.height);
		}
#endif // USE_OPENCV

		//bc::BarImg getBarImg(const int wid, int hei) const
		//{
		//	BarImg bc(wid, hei);
		//	for (barvalue& it = matr.begin(); it != matr.end(); ++it) {
		//		bc.set(it->point.y, it->point.x, it->value);
		//	}
		//	return bc;
		//}

		//void setFromBarImg(const bc::BarImg& mat)
		//{
		//	matr.clear();

		//	for (size_t i = 0; i < mat.getLiner(); i++)
		//		matr->push_back(barvalue(mat.getPointAt(i), mat.getLiner(i), mat._wid));
		//}

		BarRect getBarRect() const
		{
			int l = 0, r = 0, t = 0, d = 0;
			if (matr.size() > 0)
			{
				r = l = matr[0].getX();
				t = d = matr[0].getY();
				for (size_t j = 0; j < matr.size(); ++j)
				{
					if (l > matr[j].getX())
						l = matr[j].getX();
					if (r < matr[j].getX())
						r = matr[j].getX();

					if (t > matr[j].getY())
						t = matr[j].getY();
					if (d < matr[j].getY())
						d = matr[j].getY();
				}
			}
			return BarRect(l, t, r - l + 1, d - t + 1);
		}

		void addCoord(const point& p, const Barscalar& bright)
		{
			matr.push_back(barvalue(p, bright));
			//			if (bright < start)
			//				start = bright;
			//			if (bright > m_end)
			//				m_end = bright;
		}

		void addCoord(barvalue val)
		{
			matr.push_back(val);
		}

		//    barline(uchar _start, uchar _len) :binmat(0,0,CV_8UC1), start(_start), len(_len) {}


		barline* clone(bool cloneMatrix = true) const
		{
			barline* temp = new barline(start, m_end, /*matWid*/0, nullptr);

			temp->root = root;
			temp->parentId = this->parentId;
			temp->childrenId = this->childrenId;

			if (matr.size() != 0 && cloneMatrix)
			{
				temp->matr.insert(temp->matr.begin(), matr.begin(), matr.end());
			}
			temp->isCopy = true;

			if (bar3d != nullptr)
			{
				temp->bar3d = new barcounter();
				temp->bar3d->insert(temp->bar3d->begin(), bar3d->begin(), bar3d->end());
			}
			return temp;
		}

		void addChild(barline* nchild)
		{
			assert(root);
			assert(nchild->parentId == UINT_MAX);
			assert(nchild != nullptr);
			assert(this != nchild);
			assert(id != UINT_MAX);

			nchild->parentId = id;
			childrenId.push_back(nchild->id);
		}

		Barscalar len() const
		{
			return m_end > start ? m_end - start : start - m_end;
		}

		Barscalar getLength() const
		{
			return m_end > start ? m_end - start : start - m_end;
		}

		Barscalar getStart() const
		{
			return start;
		}

		Barscalar getEnd() const
		{
			return m_end;
		}

#ifdef INCLUDE_PY
		bp::tuple pystart()
		{
			return start.pyvalue();
		}
		bp::tuple pyend()
		{
			return m_end.pyvalue();
		}
		bp::tuple pylen()
		{
			return len().pyvalue();
		}

#endif // INCLUDE_PY


		float lenFloat() const
		{
			return m_end > start ?
				m_end.getAvgFloat() - start.getAvgFloat() :
				start.getAvgFloat() - m_end.getAvgFloat();
		}

		int getDeath()
		{
			int r = 0;
			barline* temp = getParent();
			while (temp)
			{
				++r;
				temp = temp->getParent();
			}
			//			dep = sr;
			return r;
		}

		const Barscalar& end() const
		{
			return m_end;
		}

		size_t getPointsSize() const
		{
			return matr.size();
		}

		barvalue getPoint(size_t index) const
		{
			if (index >= matr.size())
				index = index % matr.size();

			return matr[index];
		}

		bc::point getPointValue(size_t index) const
		{
			if (index >= matr.size())
				index = index % matr.size();

			return matr[index].getPoint();
		}

		void getChildsMatr(barmapHash<bc::point, bool, bc::pointHash>& childs)
		{
			for (uint chilId : this->childrenId)
			{
				barline* child = getChild(chilId);
				for (barvalue& val : child->matr)
				{
					childs.insert(std::pair< bc::point, bool>(val.getPoint(), true));
				}
			}
		}


		void getChildsMatr(barvector& vect, bool recursive = false) const
		{
			for (uint chilId : this->childrenId)
			{
				barline* child = getChild(chilId);
				if (recursive)
					getChildsMatr(vect);
				for (barvalue& val : child->matr)
				{
					vect.push_back(val);
				}
			}
		}


		size_t getMatrSize() const
		{
			size_t msize = matr.size();

			for (uint chilId : this->childrenId)
			{
				const barline* child = getChild(chilId);
				msize += child->getMatrSize();
			}

			return msize;
		}

		size_t getBarcode3dSize()
		{
			return bar3d->size();
		}

		bar3dvalue getBarcode3dValue(size_t index)
		{
			if (bar3d == nullptr)
				return bar3dvalue();

			if (index >= bar3d->size())
				index = index % bar3d->size();

			return bar3d->at(index);
		}

		//  bc::CompareStrategy::compare3dHist ONLY FOR !!! UCHAR !!!
		float compare3dbars(bc::barline* inc, bc::CompareStrategy cmp)
		{
			float t = 0, x2 = 0, y2 = 0;
			size_t n = MIN(bar3d->size(), inc->bar3d->size());

			if (cmp == bc::CompareStrategy::compare3dHist)
			{
				Barscalar s0[255];

				if (n == 0)
					return 1;
				memset(&s0, 0, 255 * sizeof(Barscalar));
				for (size_t i = 0; i < bar3d->size(); ++i)
				{
					bc::bar3dvalue& b = bar3d->at(i);
					s0[(int)b.value] = (uchar)b.count;
					x2 += b.count * b.count;
				}

				Barscalar s1[255];
				memset(&s1, 0, 255 * sizeof(Barscalar));
				for (size_t i = 0; i < inc->bar3d->size(); ++i)
				{
					bc::bar3dvalue& b = inc->bar3d->at(i);
					s1[(int)b.value] = (uchar)b.count;
					y2 += b.count * b.count;
				}

				for (size_t i = 0; i < 255; i++)
					t += static_cast<float>(s0[i] * s1[i]);
			}
			else if (cmp == bc::CompareStrategy::compare3dBrightless)
			{
				for (size_t i = 0; i < bar3d->size(); ++i)
				{
					bc::bar3dvalue& b = bar3d->at(i);
					x2 += b.count * b.count;
				}

				for (size_t i = 0; i < inc->bar3d->size(); ++i)
				{
					bc::bar3dvalue& b = inc->bar3d->at(i);
					y2 += b.count * b.count;
				}

				for (size_t i = 0; i < n; i++)
					t += inc->bar3d->at(i).count * bar3d->at(i).count;
			}
			else
				return 0;

			x2 = sqrtf(x2);
			y2 = sqrtf(y2);
			t = acosf(t / (x2 * y2));

			const float PI = acosf(-1.0f) / 2;
			if (isnan(t))
				return 1.f;
			return  abs(roundf(1000.f * (PI - t) / PI) / 1000.f);
		}

		bc::barvector getEnclusivePoints()
		{
			bc::barvector out;
			getChildsMatr(out);

			return out;
		}

		bc::barvector getExclusivePoints()
		{
			bc::barvector out;
			barmapHash<bc::point, bool, bc::pointHash> childs;
			getChildsMatr(childs);

			for (size_t i = 0; i < matr.size(); i++)
			{
				if (childs.find(matr[i].getPoint()) == childs.end())
					out.push_back(matr[i]);
			}

			return out;
		}

		bc::barline* getParent() const
		{
			if (parentId == UINT_MAX)
				return nullptr;

			return root->barlines[parentId];
		}

		bc::barline* getChild(uint id) const
		{
			if (id < 0 || id >= childrenId.size())
				return nullptr;

			return root->barlines[childrenId[id]];
		}

		size_t getChildrenCount() const
		{
			return childrenId.size();
		}

#ifdef _PYD
		bp::list getPoints(bool skipChildPoints = false) const
		{
			barmapHash<bc::point, bool, bc::pointHash> childs;
			bp::list l;

			if (skipChildPoints)
			{
				getChildsMatr(childs);

				for (size_t i = 0; i < matr.size(); i++)
				{
					if (childs.find(matr[i].getPoint()) == childs.end())
						l.append(matr[i]);
				}
			}
			else
			{
				for (size_t i = 0; i < matr.size(); i++)
					l.append(matr[i]);
			}

			return l;
		}

		bp::list getBarcode3d() const
		{
			bp::list l;
			if (bar3d != nullptr)
			{
				for (size_t i = 0; i < bar3d->size(); i++)
					l.append(bar3d->at(i));
			}

			return l;
		}

		//bp::dict getPointsInDict(bool skipChildPoints = false)
		//{
		//	barmapHash<bc::point, bool, bc::pointHash> childs;
		//	bp::dict pydict;

		//	if (skipChildPoints)
		//	{
		//		getChildsMatr(childs);

		//		for (auto iter = matr.begin(); iter != matr.end(); ++iter)
		//		{
		//			if (childs.find(iter->getPoint()) == childs.end())
		//				pydict[iter->getPoint()] = iter->value;
		//		}
		//	}
		//	else
		//	{
		//		for (auto iter = matr.begin(); iter != matr.end(); ++iter)
		//		{
		//			if (childs.find(iter->getPoint()) == childs.end())
		//				pydict[iter->getPoint()] = iter->value;
		//		}
		//	}
		//	return pydict;
		//}

		bp::list getRect() const
		{
			BarRect rect = getBarRect();
			bp::list ls;
			ls.append(rect.x);
			ls.append(rect.y);
			ls.append(rect.width);
			ls.append(rect.height);
			return ls;
		}

		bp::list getChildren() const
		{
			bp::list list;
			for (size_t i = 0; i < getChildrenCount(); i++)
				list.append(getChild(i));

			return list;
		}

		bp::list PY_getBettyNumbers() const
		{
			auto hist = getBettyNumbers();
			bp::list pyhist;
			for (size_t i = 0; i < 256; i++)
				pyhist.append(hist[i]);

			return pyhist;
		}

#endif // _PYD

		enum class ExportGraphType
		{
			noExport,
			exportIndeies,
			exportLines
		};

		template<class TSTR, typename TO_STRING>
		void getJsonObject(TSTR& outObj,
			ExportGraphType exportGraph = ExportGraphType::noExport,
			bool export3dbar = false,
			bool expotrBinaryMask = false) const
		{
			//			std::string nl = "\r\n";
			outObj += "{";

			outObj += "\"i\":";
			outObj += TO_STRING::toStr(reinterpret_cast<size_t>(this));
			outObj += ",\"s\":";
			outObj += start.text<TSTR, TO_STRING>(true);
			outObj += ",\"e\":";
			outObj += m_end.text<TSTR, TO_STRING>(true);


			if (exportGraph != ExportGraphType::noExport)
			{
				outObj += ",\"c\":[";
				int total = childrenId.size();
				if (total > 0)
				{
					assert(total < 16410954);

					if (exportGraph == ExportGraphType::exportIndeies)
					{
						for (int i = 0; i < total; ++i)
						{
							if (childrenId[i] == UINT_MAX)
								continue;

							outObj += TO_STRING::toStr(childrenId[i]);
							outObj += ",";
						}
					}
					else
					{
						for (int i = 0; i < total; ++i)
						{
							if (childrenId[i] == UINT_MAX)
								continue;

							getChild(i)->getJsonObject<TSTR, TO_STRING>(outObj, exportGraph, export3dbar, expotrBinaryMask);
							outObj += ",";
						}
					}
				}

				auto lastChar = outObj.at(outObj.length() - 1);
				if (lastChar == ',')
					outObj[outObj.length() - 1] = ']';
				else
					outObj += "]";
			}
			if (export3dbar && bar3d)
			{
				// TODO

				//				barcounter* bar3d = nullptr;
			}
			if (expotrBinaryMask)
			{
				outObj += ",\"m\":[";

				for (int i = 0, total = matr.size(); i < total; ++i)
				{
					outObj += "{\"x\":";
					outObj += matr[i].getX();
					outObj += ",\"y\":";
					outObj += matr[i].getY();
					outObj += ",\"v\":";
					outObj += matr[i].value.text<TSTR, TO_STRING>(true);
					outObj += "},";
				}
				auto lastChar = outObj.at(outObj.length() - 1);
				if (lastChar == ',')
					outObj[outObj.length() - 1] = ']';
				else
					outObj += "]";
			}
			outObj += "}";
		}

		void getChilredAsList(barlinevector& lines, bool includeItself, bool clone, bool cloneMatrxi = true)
		{
			if (includeItself)
			{
				lines.push_back(clone ? this->clone(cloneMatrxi) : this);
			}

			for (size_t i = 0, total = childrenId.size(); i < total; ++i)
			{
				getChild(i)->getChilredAsList(lines, true, clone, cloneMatrxi);
			}
		}

		void extractChilred(barlinevector& lines, bool includeItself, bool keepMatrix = false)
		{
			if (includeItself)
			{
				lines.push_back(this);
				if (!keepMatrix)
					this->matr.clear();
			}

			for (size_t i = 0, total = childrenId.size(); i < total; ++i)
			{
				auto child = getChild(i);
				child->getChilredAsList(lines, true, keepMatrix);
				childrenId[i] = UINT_MAX;
			}
		}

		void getAsListSafe(barlinevector& lines, bool clone, bool cloneMatrxi = true)
		{
			::std::unordered_set<barline*> setd;
			::std::stack<barline*> stack;
			::std::stack<int> stackIndex;

			lines.push_back(clone ? this->clone(cloneMatrxi) : this);
			stack.push(this);
			stackIndex.push(0);
			while (stack.size() > 0)
			{
				barline* cur = stack.top(); stack.pop();
				size_t i = stackIndex.top(); stackIndex.pop();
				size_t total = cur->childrenId.size();
				for (; i < total;)
				{
					barline* nchld = cur->getChild(i);
					if (nchld == nullptr || setd.count(nchld) > 0)
					{
						++i;
						continue;
					}

					setd.insert(nchld);
					lines.push_back(clone ? nchld->clone(cloneMatrxi) : nchld);

					if (nchld->childrenId.size() > 0)
					{
						stack.push(nchld);
						stackIndex.push(i + 1);
						cur = nchld;

						i = 0;
						total = cur->childrenId.size();
					}
					else
					{
						++i;
					}
				}
			}
		}

		bool is3dmoreing() const
		{
			int coos[256];
			for (size_t i = 0; i < bar3d->size(); ++i)
			{
				Barscalar val = bar3d->at(i).value;
				int tval = 256 * static_cast<int>(static_cast<float>(val) / (float)len());

				++coos[tval];
			}

			int prev = coos[0];
			for (size_t i = 1; i < 256; ++i)
			{
				int cur = coos[i];
				if (cur == 0)
					continue;

				if (prev < cur)
					return false;

				prev = cur;
			}

			return true;
		}

		std::array<int, 256> getBettyNumbers() const
		{
			std::array<int, 256> bs;
			std::fill(bs.begin(), bs.end(), 0);

			short st = start.getAvgUchar();
			short ed = end().getAvgUchar();
			if (st > ed)
				std::swap(st, ed);

			addBettyNumbers(bs, st);
		}

		void addBettyNumbers(std::array<int, 256>& bs, short offset) const
		{
			auto st = start.getAvgUchar();
			auto ed = end().getAvgUchar();
			if (st > ed)
				std::swap(st, ed);

			for (short i = st; i <= ed; ++i)
			{
				++bs[i];
			}

			for (const uint l : childrenId)
			{
				getChild(l)->addBettyNumbers(bs, offset);
			}
		}

		std::vector<ShortPoint> createSperal()
		{
			std::vector<ShortPoint> speral;

			barvalue prev = matr[0];
			speral.push_back(ShortPoint(matr[1].x - prev.x,0));
			for (auto &p : matr)
			{
				if (p.value != prev.value)
				{
					speral.push_back(ShortPoint(p.x - prev.x, p.y - prev.y));
					prev = p;
				}
			}

			return speral;
		}
	};

	// comparable

	using bline = barline;
	using BarRoot = barline;
}
