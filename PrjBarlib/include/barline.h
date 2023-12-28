#ifndef SKIP_M_INC
#pragma once

#include "barstrucs.h"

#include "include_py.h"
#include "include_cv.h"

#include <math.h>
#include <string>
#include <vector>
#include <unordered_set>
#include <stack>
#endif
namespace bc
{
	struct barline;
	using barlinevector = std::vector<barline*>;

	struct ShortPoint
	{
		short x, y;

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
		// Graph
		barlinevector children;
		barline* parent = nullptr;
		size_t numberInParent = 0;

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

		barline(int = 0)
		{
			//			matWid = wid;
		}
		barline(Barscalar _start, Barscalar _end, int = 0, barcounter* _barc = nullptr, size_t coordsSize = 0) : start(_start), m_end(_end)
			//			  ,matWid(wid)
		{
			matr.reserve(coordsSize);
			bar3d = _barc;
			//			assert(matWid != 0);
		}

		//copy const
		barline(barline const& obj)
		{
			this->start = obj.start;
			this->m_end = obj.m_end;
			//			this->matWid = obj.matWid;
			this->numberInParent = obj.numberInParent;
			this->parent = obj.parent;
			this->children = obj.children;

			if (obj.matr.size() != 0)
			{
				this->matr.insert(this->matr.begin(), obj.matr.begin(), obj.matr.end());
			}
			this->isCopy = true;

			if (obj.bar3d != nullptr)
			{
				this->bar3d = new barcounter();
				this->bar3d->insert(this->bar3d->begin(), obj.bar3d->begin(), obj.bar3d->end());
			}
		}

		// copy
		void operator=(barline const& obj)
		{
			this->start = obj.start;
			this->m_end = obj.m_end;
			//			this->matWid = obj.matWid;

			this->numberInParent = obj.numberInParent;
			this->parent = obj.parent;
			this->children = obj.children;

			if (obj.matr.size() != 0)
				this->matr.insert(this->matr.begin(), obj.matr.begin(), obj.matr.end());

			this->isCopy = true;

			if (obj.bar3d != nullptr)
			{
				this->bar3d = new barcounter();
				this->bar3d->insert(this->bar3d->begin(), obj.bar3d->begin(), obj.bar3d->end());
			}
		}

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

			this->numberInParent = obj.numberInParent;
			this->parent = std::exchange(obj.parent, nullptr);
			this->children = obj.children;
			obj.children.clear();
			this->isCopy = false;

			if (obj.matr.size() != 0)
			{
				this->matr = obj.matr;
				obj.matr.clear();
			}

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

			this->numberInParent = obj.numberInParent;
			this->parent = std::exchange(obj.parent, nullptr);
			this->children = obj.children;
			obj.children.clear();

			if (obj.matr.size() != 0)
			{
				this->matr = obj.matr;
				obj.matr.clear();
			}

			this->bar3d = obj.bar3d;
		}

		~barline()
		{
			// canBeDeleted - у копии, копия не может удалять детей. ј оригинал может
			if (!isCopy)
			{
				if (parent && parent->children.size() > numberInParent && parent->children[numberInParent] == this)
				{
					parent->children[numberInParent] = nullptr;
				}
				for (size_t i = 0; i < children.size(); ++i)
				{
					// not null, not parent on any child
					if (children[i])
						children[i]->parent = nullptr;
					//delete children[i];
				}
			}
			if (bar3d != nullptr)
			{
				delete bar3d;
			}
			matr.clear();
		}

		//		int getWid()
		//		{
		//			return matWid;
		//		}

		const barvector& getMatrix() const
		{
			return matr;
		}
		barvector& getMatrix() {
			return matr;
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

		BarRect getBarRect()  const
		{
			int l, r, t, d;
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
			return BarRect(l, t, r - l + 1, d - t + 1);
		}

		void addCoord(const point& p, Barscalar bright)
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

			temp->numberInParent = this->numberInParent;
			temp->parent = this->parent;
			temp->children = this->children;

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

		void setparent(barline* node)
		{
			//assert(parent == nullptr || parent == node);
			assert(this != node);
			if (this->parent == node)
				return;

			this->parent = node;
			numberInParent = parent->children.size();
			parent->children.push_back(this);
		}

		Barscalar len() const
		{
			return m_end > start ? m_end - start : start - m_end;
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
			barline* temp = parent;
			while (temp)
			{
				++r;
				temp = temp->parent;
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
			for (barline* chil : this->children)
			{
				for (barvalue& val : chil->matr)
				{
					childs.insert(std::pair< bc::point, bool>(val.getPoint(), true));
				}
			}
		}


		void getChildsMatr(barvector& vect, bool recursive = false) const
		{
			for (barline* chil : this->children)
			{
				if (recursive)
					getChildsMatr(vect);
				for (barvalue& val : chil->matr)
				{
					vect.push_back(val);
				}
			}
		}


		size_t getMatrSize() const
		{
			size_t msize = matr.size();

			for (barline* chil : this->children)
			{
				msize += chil->getMatrSize();
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

		//  bc::CompireStrategy::compire3dHist ONLY FOR !!! UCHAR !!!
		float compire3dbars(bc::barline* inc, bc::CompireStrategy cmp)
		{
			float t = 0, x2 = 0, y2 = 0;
			size_t n = MIN(bar3d->size(), inc->bar3d->size());

			if (cmp == bc::CompireStrategy::compire3dHist)
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
			else if (cmp == bc::CompireStrategy::compire3dBrightless)
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

		bc::barline* getChild(int id)
		{
			if (id < 0 || id >= children.size())
				return children[id];
			else
				return nullptr;
		}

		size_t getChildrenCount()
		{
			return children.size();
		}

#ifdef _PYD

		bp::list getPoints(bool skipChildPoints = false)
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

		bp::list getBarcode3d()
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

		bp::list getRect()
		{
			BarRect rect = getBarRect();
			bp::list ls;
			ls.append(rect.x);
			ls.append(rect.y);
			ls.append(rect.width);
			ls.append(rect.height);
			return ls;
		}

		bp::list getChildren()
		{
			bp::list list;
			for (barline* child : children)
				list.append(child);

			return list;
		}

		bc::barline* getParent()
		{
			return this->parent;
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
				int total = children.size();
				if (total > 0)
				{
					assert(total < 16410954);

					if (exportGraph == ExportGraphType::exportIndeies)
					{
						for (int i = 0; i < total; ++i)
						{
							if (children[i] == NULL)
								continue;

							outObj += TO_STRING::toStr(reinterpret_cast<size_t>(children[i]));
							outObj += ",";
						}
					}
					else
					{
						for (int i = 0; i < total; ++i)
						{
							if (children[i] == NULL)
								continue;

							children[i]->getJsonObject<TSTR, TO_STRING>(outObj, exportGraph, export3dbar, expotrBinaryMask);
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

			for (size_t i = 0, total = children.size(); i < total; ++i)
			{
				children[i]->getChilredAsList(lines, true, clone, cloneMatrxi);
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

			for (size_t i = 0, total = children.size(); i < total; ++i)
			{
				children[i]->getChilredAsList(lines, true, keepMatrix);
				children[i] = nullptr;
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
				size_t total = cur->children.size();
				for (; i < total;)
				{
					barline* nchld = cur->children[i];
					if (nchld == nullptr || setd.count(nchld) > 0)
					{
						++i;
						continue;
					}

					setd.insert(nchld);
					lines.push_back(clone ? nchld->clone(cloneMatrxi) : nchld);

					if (nchld->children.size() > 0)
					{
						stack.push(nchld);
						stackIndex.push(i + 1);
						cur = nchld;

						i = 0;
						total = cur->children.size();
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
			float d = (float)this->len() / 256;
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

		void getBettyNumbers(int* bs) const
		{
			memset(bs, 0, 256 * sizeof(int));

			short st = start.getAvgUchar();
			short ed = end().getAvgUchar();
			if (st > ed)
				std::swap(st, ed);

			addBettyNumbers(bs, st);
		}

		void addBettyNumbers(int* bs, short offset) const
		{
			auto st = start.getAvgUchar();
			auto ed = end().getAvgUchar();
			if (st > ed)
				std::swap(st, ed);

			for (short i = st; i <= ed; ++i)
			{
				++bs[i];
			}

			for (const barline* l : children)
			{
				l->addBettyNumbers(bs, offset);
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
