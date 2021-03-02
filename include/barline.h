#pragma once

#include "barstrucs.h"
#include "barImg.h"

#include "include_py.h"
#include "include_cv.h"


namespace bc
{
	template<class T>
	struct EXPORT barline
	{

		bool isCopy = false;
		//    cv::Mat binmat;
#ifdef USE_OPENCV

		cv::Mat getCvMat(const cv::Size& s)
		{
			cv::Mat m = cv::Mat::zeros(s.height, s.width, CV_8UC1);
			for (auto it = matr.begin(); it != matr.end(); ++it) {
				m.at<uchar>(it->point.y, it->point.x) = it->value;
			}
			return m;
		}
		void setFromCvMat(cv::Mat& mat)
		{
			matr->clear();
			mat.forEach<uchar>([m = matr](uchar& pixel, const int* pos) -> void {
				m->push_back(ppair(bc::point(pos[0], pos[1]), pixel)); });
		}
		cv::Rect getCvRect()
		{
			auto r = getBarRect();
			return cv::Rect(r.x, r.y, r.width, r.height);
		}
#endif // USE_OPENCV

		bc::BarImg<T> getBarImg(const int wid, int hei)
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
				matr->push_back(ppair<T>(mat.getPointAt(i), mat.getLiner(i)));
		}

		BarRect getBarRect()
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
			matr.push_back(ppair<T>(first, bright));
		}
		pmap<T> matr;
		T start;
		T len;
		barcounter* bar3d = nullptr;
		//    barline(uchar _start, uchar _len) :binmat(0,0,CV_8UC1), start(_start), len(_len) {}
		//    barline(uchar _start, uchar _len, cv::Mat _mat) :  start(_start), len(_len)
		//    {
		//        binmat = _mat;
		//    }
		barline()
		{

		}
		barline(T _start, T _len, barcounter* _barc = nullptr, size_t coordsSize = 0) : start(_start), len(_len) {
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
				for (int i = 0; i < childrens.size(); ++i)
				{
					childrens[i]->parrent = nullptr;
					delete childrens[i];
				}
			}
			if (bar3d != nullptr)
			{
				delete bar3d;
			}

		}
		barline* clone()
		{
			auto temp = new barline(start, len, nullptr);
			if (matr.size() != 0)
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

		//bc::Component *comp;
		barline<T>* parrent = nullptr;
		std::vector<barline<T>*> childrens;
		size_t numInParet = 0;

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

#ifdef _PYD
		bp::dict getPoints()
		{
			bp::dict pydict;

			for (auto iter = matr.begin(); iter != matr.end(); ++iter)
				pydict[iter->point] = iter->value;

			return pydict;
		}

		bp::list getRect()
		{
			BarRect rect= getBarRect();
			bp::list ls;
			ls.append(rect.x);
			ls.append(rect.y);
			ls.append(rect.width);
			ls.append(rect.height);
			return ls;
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
