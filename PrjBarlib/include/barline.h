#pragma once

#include "barstrucs.h"
#include "barImg.h"

#include "include_py.h"
#include "include_cv.h"

#include <unordered_map>

namespace bc
{
	template<class T>
	struct EXPORT barline
	{
		bc::barvector<T> matr;
		barcounter<T>* bar3d = nullptr;
		std::vector<barline<T>*> childrens;
		barline<T>* parrent = nullptr;
		size_t numInParet = 0;
		T start;
		T len;
		bool isCopy = false;

		//bc::Component *comp;
		//    cv::Mat binmat;
#ifdef USE_OPENCV

		cv::Mat getCvMat(const cv::Size& s)
		{
			cv::Mat m = cv::Mat::zeros(s.height, s.width, CV_8UC1);
			for (auto it = matr.begin(); it != matr.end(); ++it) {
				m.at<T>(it->point.y, it->point.x) = it->value;
			}
			return m;
		}
		void setFromCvMat(cv::Mat& mat)
		{
			matr->clear();
			mat.forEach<T>([m = matr](uchar& pixel, const int* pos) -> void {
				m->push_back(barvalue(bc::point(pos[0], pos[1]), pixel)); });
		}
		cv::Rect getCvRect()
		{
			auto r = getBarRect();
			return cv::Rect(r.x, r.y, r.width, r.height);
		}
#endif // USE_OPENCV

		bc::BarImg<T> getBarImg(const int wid, int hei) const
		{
			BarImg<T> bc(wid, hei);
			for (auto* it = matr.begin(); it != matr.end(); ++it) {
				bc.set(it->point.y, it->point.x, it->value);
			}
			return bc;
		}
		void setFromBarImg(const bc::BarImg<T>& mat)
		{
			matr.clear();

			for (size_t i = 0; i < mat.getLiner(); i++)
				matr->push_back(barvalue<T>(mat.getPointAt(i), mat.getLiner(i)));
		}

		BarRect getBarRect()  const
		{
			int l, r, t, d;
			r = l = matr[0].point.x;
			t = d = matr[0].point.y;
			for (int j = 0; j < matr.size(); ++j)
			{
				if (l > matr[j].point.x)
					l = matr[j].point.x;
				if (r < matr[j].point.x)
					r = matr[j].point.x;

				if (t > matr[j].point.y)
					t = matr[j].point.y;
				if (d < matr[j].point.y)
					d = matr[j].point.y;
			}
			return BarRect(l, t, r - l + 1, d - t + 1);
		}
		void addCoord(const point& first, T bright)
		{
			matr.push_back(barvalue<T>(first, bright));
		}

		//    barline(uchar _start, uchar _len) :binmat(0,0,CV_8UC1), start(_start), len(_len) {}
		//    barline(uchar _start, uchar _len, cv::Mat _mat) :  start(_start), len(_len)
		//    {
		//        binmat = _mat;
		//    }
		barline()
		{

		}
		barline(T _start, T _len, barcounter<T>* _barc = nullptr, size_t coordsSize = 0) : start(_start), len(_len) {
			matr.reserve(coordsSize);
			bar3d = _barc;
		}
		~barline()
		{
			// canBeDleted - у копии, копи¤ не может удал¤ть детей. ј оригинал может
			if (!isCopy)
			{
				if (parrent && parrent->childrens.size() > numInParet && parrent->childrens[numInParet] == this)
				{
					parrent->childrens[numInParet] = nullptr;
				}
				for (size_t i = 0; i < childrens.size(); ++i)
				{
					childrens[i]->parrent = nullptr;
					//delete childrens[i];
				}
			}
			if (bar3d != nullptr)
			{
				delete bar3d;
			}

		}
		barline* clone() const
		{
			auto temp = new barline(start, len, nullptr);
			if (matr.size() != 0)
			{
				temp->matr.insert(temp->matr.begin(), matr.begin(), matr.end());
			}
			temp->isCopy = true;

			if (bar3d != nullptr)
			{
				temp->bar3d = new barcounter<T>();
				temp->bar3d->insert(temp->bar3d->begin(), bar3d->begin(), bar3d->end());
			}
			return temp;
		}

		void setParrent(barline<T>* node)
		{
			assert(parrent == nullptr || parrent == node);
			if (this->parrent == node)
				return;

			this->parrent = node;
			numInParet = parrent->childrens.size();
			parrent->childrens.push_back(this);
		}

		T end() const
		{
			return start + len;
		}

		size_t getPointsSize() const
		{
			return matr.size();
		}

		barvalue<T> getPoint(size_t index) const
		{
			if (index >= matr.size())
				index = index % matr.size();

			return matr[index];
		}


		void getChildsMatr(std::unordered_map<bc::point, bool, bc::pointHash>& childs)
		{
			for (barline<T>* chil : this->childrens)
			{
				for (barvalue<T>& val : chil->matr)
				{
					childs.insert(std::pair< bc::point, bool>(val.point, true));
				}
			}
		}

#ifdef _PYD



		//bp::list getPoints()
		//{
		//	return getPoints(false);
		//}
		bp::list getPoints(bool skipChildPoints = false)
		{
			std::unordered_map<bc::point, bool, bc::pointHash> childs;
			bp::list l;

			if (skipChildPoints)
			{
				getChildsMatr(childs);

				for (size_t i = 0; i < matr.size(); i++)
				{
					if (childs.find(matr[i].point) == childs.end())
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

		size_t getBarcode3dSize()
		{
			return bar3d->size();
		}

		bar3dvalue<T> getBarcode3dValue(size_t index)
		{
			if (bar3d == nullptr)
				return bar3dvalue<T>();

			if (index >= bar3d->size())
				index = index % bar3d->size();

			return bar3d->at(index);
		}

		float compire3dbars(bc::barline<T>* inc, bc::CompireStrategy cmp)
		{
			T s0[255];
			float t = 0, x2 = 0, y2 = 0;
			int n = MIN(bar3d->size(), inc->bar3d->size());

			if (n == 0)
				return 1;
			memset(&s0, 0, 255 * sizeof(T));
			for (size_t i = 0; i < bar3d->size(); ++i)
			{
				bc::bar3dvalue<T>& b = bar3d->at(i);
				s0[(uchar)b.value] = b.count;
				x2 += b.count * b.count;
			}

			T s1[255];
			memset(&s1, 0, 255 * sizeof(T));
			for (size_t i = 0; i < inc->bar3d->size(); ++i)
			{
				bc::bar3dvalue<T>& b = inc->bar3d->at(i);
				s1[(uchar)b.value] = b.count;
				y2 += b.count * b.count;
			}
			if (cmp == bc::CompireStrategy::compire3dHist)
			{
				for (size_t i = 0; i < 255; i++)
					t += s0[i] * s1[i];
			}
			else if (cmp == bc::CompireStrategy::compire3dBrightless)
			{
				for (size_t i = 0; i < n; i++)
					t += inc->bar3d->at(i).count * bar3d->at(i).count;
			}
			else
				return 0;

			x2 = sqrtf(x2);
			y2 = sqrtf(y2);
			t = acosf(t / (x2 * y2));

			const double PI = acos(-1.0) / 2;
			if (isnan(t))
				return 1.f;
			return  abs(roundf(1000 * (PI - t) / PI) / 1000.f);
		}

		//bp::dict getPointsInDict()
		//{
		//	return getPointsInDict(false);
		//}
		bp::dict getPointsInDict(bool skipChildPoints = false)
		{
			std::unordered_map<bc::point, bool, bc::pointHash> childs;
			bp::dict pydict;

			if (skipChildPoints)
			{
				getChildsMatr(childs);

				for (auto iter = matr.begin(); iter != matr.end(); ++iter)
				{
					if (childs.find(iter->point) == childs.end())
						pydict[iter->point] = iter->value;
				}
			}
			else
			{
				for (auto iter = matr.begin(); iter != matr.end(); ++iter)
				{
					if (childs.find(iter->point) == childs.end())
						pydict[iter->point] = iter->value;
				}
			}
			return pydict;
		}

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
			for (barline<T>* child : childrens)
				list.append(child);

			return list;
		}

		bc::barline<T>* getParent()
		{
			return this->parrent;
		}
#endif // _PYD

	};

	// comparable
	template<class T>
	using bline = barline<T>;

	template<class T>
	using BarRoot = barline<T>;
	//	template<class T>
	//	struct EXPORT BarRoot
	//	{
	//		std::vector<barline<T>*> children;

	//		void addChild(barline<T>* line)
	//		{
	//			children.push_back(line);
	//		}
	//	};
}
