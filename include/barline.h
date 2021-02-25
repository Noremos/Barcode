#pragma once

#include "barstrucs.h"
#include "barImg.h"
namespace bc
{
	template<class T>
	struct EXPORT barline
	{

		bool dropChildes = true;
		//    cv::Mat binmat;
#ifdef USE_OPENCV


		cv::Mat getMat(const cv::Size& s)
		{
			cv::Mat m = cv::Mat::zeros(s.height, s.width, CV_8UC1);
			for (auto it = matr->begin(); it != matr->end(); ++it) {
				m.at<uchar>(it->first.y, it->first.x) = it->second;
			}
			return m;
		}
		void setFromMat(cv::Mat& mat)
		{
			matr->clear();
			mat.forEach<uchar>([m = matr](uchar& pixel, const int* pos) -> void {
				m->push_back(ppair(bc::point(pos[0], pos[1]), pixel)); });
		}
		cv::Rect getRect()
		{
			pmap& points = (*matr);
			int l, r, t, d;
			r = l = points[0].first.x;
			t = d = points[0].first.y;
			for (int j = 0; j < points.size(); ++j)
			{
				if (l > points[j].first.x)
					l = points[j].first.x;
				if (r < points[j].first.x)
					r = points[j].first.x;

				if (t > points[j].first.y)
					t = points[j].first.y;
				if (d < points[j].first.y)
					d = points[j].first.y;
			}
			return cv::Rect(l, t, r - l + 1, d - t + 1);
		}
#endif // USE_OPENCV

		bc::BarImg<T> getBarImg(const int wid, int hei)
		{
			BarImg<T> bc(wid, hei);
			for (auto it = matr->begin(); it != matr->end(); ++it) {
				bc.set(it->first.y, it->first.x, it->second);
			}
			return bc;
		}
		void setFromBarImg(const bc::BarImg<T>& mat)
		{
			matr->clear();
			size_t pos = 0;
			for (size_t i = 0; i < mat.getLiner(); i++)
				matr->push_back(ppair<T>(mat.getPointAt(i), mat.getLiner(i)));
		}

		BarRect getRect()
		{
			pmap<T>& points = (*matr);
			int l, r, t, d;
			r = l = points[0].first.x;
			t = d = points[0].first.y;
			for (int j = 0; j < points.size(); ++j)
			{
				if (l > points[j].first.x)
					l = points[j].first.x;
				if (r < points[j].first.x)
					r = points[j].first.x;

				if (t > points[j].first.y)
					t = points[j].first.y;
				if (d < points[j].first.y)
					d = points[j].first.y;
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
		barline(T _start, T _len, barcounter* _barc = nullptr, size_t coordsSize = 0) : start(_start), len(_len) {
			matr.reserve(coordsSize);
			bar3d = _barc;
		}
		~barline()
		{
			if (dropChildes)
			{
				for (int i = 0; i < childrens.size(); ++i)
				{
					delete childrens[i];
				}
			}
			if (parrent)
			{
				parrent->childrens[numInParet] = nullptr;
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

			if (matr != nullptr)
			{
				for (auto iter = matr->begin(); iter != matr->end(); ++iter)
					pydict[iter->first] = iter->second;
			}

			return pydict;
		}
#endif // _PYD

	};

	// comparable
	template<class T>
	using bline = barline<T>;

	template<class T>
	struct EXPORT BarRoot
	{
		std::vector<barline<T>*> children;

		void addChild(barline<T>* line)
		{
			children.push_back(line);
		}
	};
}
