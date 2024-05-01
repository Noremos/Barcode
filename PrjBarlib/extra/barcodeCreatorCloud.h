#ifndef SKIP_M_INC
#pragma once

#include "presets.h"

#include "barstrucs.h"
#include "barclasses.h"

#include <functional>
#include <memory>


#include "include_py.h"
#include "include_cv.h"
#endif

namespace bc
{
	using PloyPoints = std::vector<bc::point>;

	class CloudPointsBarcode
	{
		struct PointIndexCov
		{
			uint points[2];
			float dist = 0;
			PointIndexCov(uint ind0 = 0, uint ind1 = 0, float _dist = 0) : dist(_dist)
			{
				points[0] = ind0;
				points[1] = ind1;
			}
		};

	public:
		struct CloudPoint
		{
			CloudPoint(int x, int y, float z) : x(x), y(y), z(z)
			{}
			int x, y;
			float z;
			float distanse(const CloudPoint& R) const
			{
				float res = sp(x - R.x) + sp(y - R.y) + sp(z - R.z);
				return sqrtf(res);
			}

			Barscalar getScalar()
			{
				Barscalar a;
				//a.data.i3 = new int[3] { x, y, z };
				//a.type = BarType::INT32_3;
				a.data.f = z;
				a.type = BarType::FLOAT32_1;
				return a;
			}

		private:
			float sp(float v) const
			{
				return v * v;
			}
			float sp(int v) const
			{
				return (float)(v * v);
			}
		};

		struct CloudPoints
		{
			std::vector<CloudPoint> points;
		};

	public:
		CloudPointsBarcode()
		{}

		bc::Barcontainer* createBarcode(const CloudPoints* points);
		//bc::Barcontainer* searchHoles(float* img, int wid, int hei, float nullVal = -9999);
		bool useHolde = false;
		// static std::function<void(const point&, const point&, bool)> drawLine;
		// static std::function<void(PloyPoints&, bool)> drawPlygon;
		using ComponentsVector = std::vector<barline*>;

	private:
		bool isContain(poidex ind) const
		{
			return included.find(ind) != included.end();
		}

		inline void setInclude(const poidex ind, barline* comp)
		{
			included[ind] = comp;
		}

		inline barline* getComp(poidex ind) const
		{
			auto itr = included.find(ind);
			return itr != included.end() ? itr->second : nullptr;
		}

		void sortPixels();
		void sortTriangulate();

		void processTypeF(const CloudPoints* points, Barcontainer* item = nullptr);
		void processFULL(const CloudPoints* points, bc::Barcontainer* item);

		void process(Barcontainer *item);
		void processComp(const CloudPointsBarcode::PointIndexCov& val);
		void processHold();

		void addItemToCont(Barcontainer* item);
		void clearIncluded();


	private:
		friend class Baritem;

		const CloudPoints* cloud = nullptr;
		uint curIndexInSortedArr = 0;
		size_t totalSize = 0;
		barmap<poidex, barline*> included;
		ComponentsVector components;
		std::unique_ptr<PointIndexCov> sortedArr;

	};

}
