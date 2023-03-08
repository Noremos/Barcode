module;

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

#define BAREXPORT

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif


//EXCLUDE_FILES
//include/presets.h
//include/barline.h
//include/barImg.h
//include/barscalar.h
//include/barstrucs.h
//include/include_cv.h
//include/include_py.h
//END_EXLCUDE

//import std.core;

#include "barline.h"
#include "barImg.h"

#include "StateBinFile.h"

export module BarcodeModule;


namespace bc
{
	template<class T>
	class un_ptr
	{
	public:
		T* item = nullptr;
		void reset(T* val)
		{
			if (item)
				delete[] item;
			item = val;
		}

		T* get()
		{
			return item;
		}

		~un_ptr()
		{
			delete[] item;
		}
	};
	class BAREXPORT Barbase
	{
	public:
		virtual void removePorog(Barscalar const porog) = 0;
		virtual void preprocessBar(Barscalar const& porog, bool normalize) = 0;
		virtual float compireFull(const Barbase* Y, bc::CompireStrategy strat) const = 0;
		virtual Barbase* clone() const = 0;
		virtual Barscalar sum() const = 0;
		virtual void relen() = 0;
		//    virtual void fullCompite(barbase const *bc, CompireFunction fn, float poroc = 0.5f) = 0;
		virtual ~Barbase();
	};

	class BAREXPORT Baritem : public Barbase
	{
	public:
		barlinevector barlines;

	private:
		bc::BarRoot* rootNode = nullptr;
		int wid;
		BarType type;
	public:
		Baritem(int wid = 0, BarType type = BarType::NONE);

		//copy constr
		Baritem(Baritem const& obj, bool cloneMatrix = true)
		{
			this->rootNode = obj.rootNode;
			this->wid = obj.wid;
			this->type = obj.type;

			for (auto* barval : obj.barlines)
			{
				this->barlines.push_back(barval->clone(cloneMatrix));
			}
		}

		// copy
		void operator=(Baritem const& obj)
		{
			this->rootNode = obj.rootNode;
			this->wid = obj.wid;
			this->type = obj.type;

			for (auto* barval : obj.barlines)
			{
				this->barlines.push_back(barval->clone());
			}
		}

		// move constr
		Baritem(Baritem&& obj) noexcept
		{
			this->rootNode = std::exchange(obj.rootNode, nullptr);
			this->wid = obj.wid;

			this->barlines = obj.barlines;
			obj.barlines.clear();
		}

		// move assign
		void operator=(Baritem&& obj) noexcept
		{
			this->rootNode = std::exchange(obj.rootNode, nullptr);
			this->wid = obj.wid;
			this->type = obj.type;

			this->barlines = obj.barlines;
			obj.barlines.clear();
		}

		//    cv::Mat binmap;
		void add(Barscalar st, Barscalar len);
		void add(barline* line);

		Barscalar sum() const;
		void relen();
		Barscalar maxLen() const;
		Baritem* clone() const;
		inline BarType getType()
		{
			return type;
		}

		inline void setType(BarType bt)
		{
			type = bt;
		}
		inline void setType()
		{
			if (barlines.size() > 0 && barlines[0]->matr.size() > 0)
				type = barlines[0]->matr[0].value.type
				;
			else
				type = BarType::BYTE8_1;
		}

		void getBettyNumbers(int* bs);

		// remove lines than less then passed value
		void removePorog(Barscalar const porog);
		void preprocessBar(Barscalar const& porog, bool normalize);
		float compireFull(const Barbase* bc, bc::CompireStrategy strat) const;
		float compireBestRes(Baritem const* bc, bc::CompireStrategy strat) const;
		float compareOccurrence(Baritem const* bc, bc::CompireStrategy strat) const;
		//    void fullCompite(const barbase *bc, CompireFunction fn, float poroc = 0.5f);
		void normalize();

		template<class TSTR, typename TO_STR>
		void getJsonObject(TSTR &out, bool exportGraph = false,
						   bool export3dbar = false,
						   bool expotrBinaryMask = false) const
		{
			TSTR nlt = "\r\n";

			out = "{";
			out += nlt;
			out += "lines: ";

			getJsonLinesArray<TSTR, TO_STR>(out, exportGraph, export3dbar, expotrBinaryMask);
			out += nlt;
			out += '}';
		}

		template<class TSTR, typename TO_STR>
		void getJsonLinesArray(TSTR &out, bool exportGraph = false,
							   bool export3dbar = false,
							   bool expotrBinaryMask = false) const
		{
			out = "[ ";

			for (bc::barline *line : barlines)
			{
				line->getJsonObject<TSTR, TO_STR>(out, exportGraph, export3dbar, expotrBinaryMask);
				out += ",";
			}

			out[out.length() - 1] = ']';
		}


		~Baritem();

		bc::BarRoot* getRootNode()
		{
			return rootNode;
		}
		void setRootNode(bc::BarRoot* root)
		{
			rootNode = root;
		}

		Barscalar getMax()
		{
			Barscalar _max{ 0 };
			for (auto* b : this->barlines)
			{
				if (b->start + b->len() > _max)
					_max = b->start + b->len();
			}
			return _max;
		}


		class BAREXPORT BarscalHash
		{
		public:
			size_t operator()(const Barscalar& p) const
			{
				return ((size_t)0 << 32) + ((size_t)p.data.b3[2] << 16) +
						((size_t)p.data.b3[1] << 8) + ((size_t)p.data.b3[0]);
			}
		};


		typedef barmapHash<Barscalar, int, BarscalHash> maphist;


		class Barlfd
		{
		public:
			barline* line = NULL;
			float acc = 0;
			maphist hist;

		/*	void caclHist()
			{
				hist.clear();
				for (size_t i = 0; i < line->matr.size(); ++i)
				{
					Barscalar& scl = line->matr.at(i).value;
					auto asd = hist.find(scl);
					if (asd != hist.end())
					{
						++asd->second;
					}
					else
						hist.insert<pair< Barscalar, int>>(pair<Barscalar, int>(scl, 1));
				}
			}

			void appendHist(maphist& outHist)
			{
				for (auto& val : hist)
				{
					auto asd = outHist.find(val.second);
					if (asd != outHist.end())
					{
						asd->second += val.second;
					}
					else
						outHist.insert<pair< Barscalar, int>>(val);
				}
			}*/

			void calculateEntropy(maphist& histm)
			{
				float s = 0;
				size_t total = line->matr.size();
				for (size_t i = 0; i < total; i++)
				{
					Barscalar& scl = line->matr.at(i).value;
					auto asd = histm.find(scl);
					float lacc = asd->second / static_cast<float>(total);
					s += lacc * log(lacc);
				}

				acc = s;
			}

			void calculateMask(DatagridProvider& mask)
			{
				float s = 0;
				for (size_t w = 0; w < line->matr.size(); ++w)
				{
					if (mask.get(line->matr[w].getPoint()) > 128)
					{
						s += 1;
					}
				}

				acc =  s / mask.length();
			}
		};

		typedef std::vector<Barlfd> barsplitvec;

		float calcEntrpyByValues(const barsplitvec::iterator begin, const barsplitvec::iterator end)
		{
			maphist hist;

			//for(auto it = begin; it != end; it++)
			//{
			//	//it->appendHist(hist);
			//}

			float s = 0;
			for (auto it = begin; it != end; it++)
			{
				it->calculateEntropy(hist);
				s += it->acc * log(it->acc);
			}

			return -s;
		}

		float calcEntrpyByMask(const barsplitvec::iterator begin, const barsplitvec::iterator end, DatagridProvider& mask)
		{
			float s = 0;
			for (auto it = begin; it != end; it++)
			{
				it->calculateMask(mask);
				s += it->acc * log(it->acc);
			}

			return -s;
		}

		float calcEntropySimple(const barsplitvec::const_iterator begin, const barsplitvec::const_iterator end)
		{
			float s = 0;
			for (auto it = begin; it != end; it++)
			{
				s += it->acc * log(it->acc);
			}

			return -s;
		}
		// left - low entorpy; ritht - big
		void splitRes(barsplitvec& input, barsplitvec& left, barsplitvec& right)
		{
			//int bestI = 0; //  a = { <= I}; b = {>I}
			const barsplitvec::iterator begin = input.begin();
			const barsplitvec::iterator end = input.end();
			barsplitvec::iterator spkit = begin + 1;

			for (auto it = begin; it != end; ++it)
			{
				//it->caclHist();
			}

			if (input.size() == 1)
			{
				left.insert(left.end(), begin, begin + 1);
				return;
			}
			if (input.size() == 2)
			{
				float s0 = calcEntrpyByValues(begin, begin + 1);
				float s1 = calcEntrpyByValues(begin + 1, end);
				if (s0 < s1)
				{
					left.push_back(input[0]);
					right.push_back(input[1]);
				}
				else
				{
					left.push_back(input[1]);
					right.push_back(input[0]);
				}
				return;
			}

			float s = calcEntrpyByValues(begin, end);
			if (s == 0)
			{
				left.insert(left.end(), begin, end);
				return;
			}

			//std::cout << "Start with len: " << input.size() << std::endl;
			float minS = s;
			bool leftBigger = true;
			for (auto it = begin + 1; it != end; ++it)
			{
				float s0 = calcEntrpyByValues(begin, it);
				float s1 = calcEntrpyByValues(it, end);
				float sAvg = (s0 + s1) / 2;
				if (sAvg < minS)
				{
					leftBigger = s0 > s1;
					minS = sAvg;
					spkit = it;
					//break;
				}
			}
			if (spkit == end)
			{
				left.insert(left.end(), begin, end);
				return;
			}
			barsplitvec leftInput;
			barsplitvec rightInput;
			if (leftBigger)
			{
				// ���� ����� ����� ������, ������ �������
				leftInput.insert(leftInput.end(), spkit, end);
				rightInput.insert(rightInput.end(), begin, spkit);
			}
			else
			{
				leftInput.insert(leftInput.end(), begin, spkit);
				rightInput.insert(rightInput.end(), spkit, end);
			}

			//std::cout << "Left len: " << leftInput.size() << std::endl;
			//std::cout << "Right len: " << rightInput.size() << std::endl;

			splitRes(leftInput, left, right);
			splitRes(rightInput, left, right);
		}

		void splitByValue(barsplitvec& low, barsplitvec& high)
		{
			barsplitvec input;
			for (size_t i = 0; i < barlines.size(); i++)
			{
				input.push_back({ barlines[i], 0 });
			}
			splitRes(input, low, high);
		}

		//void splitByMask(barlinevector& left, barlinevector& right, DatagridProvider& mask)
		//{

		//	barsplitvec barvec;
		//	for (size_t i = 0; i < barlines.size(); i++)
		//	{
		//		Barlfd as;
		//		as.line = barlines.at(i);
		//		auto& matr = as.line->matr;
		//		for (size_t w = 0; w < matr.size(); ++w)
		//		{
		//			if (mask.get(matr[w].getPoint()) > 128)
		//			{
		//				as.acc += 1;
		//			}
		//		}

		//		as.acc /= mask.length();
		//	}
		//}

#ifdef _PYD
		// only for uchar
		bp::list calcHistByBarlen(/*Barscalar maxLen*/)
		{
			int maxLen = 256;
			int* hist = new int[maxLen];
			memset(hist, 0, maxLen * sizeof(int));

			for (size_t i = 0; i < barlines.size(); i++)
				++hist[static_cast<int>(barlines[i]->len())];

			bp::list pyhist;
			for (size_t i = 0; i < maxLen; i++)
				pyhist.append(hist[i]);

			delete[] hist;

			return pyhist;
		}

		bp::list PY_getBettyNumbers()
		{
			int hist[256];
			getBettyNumbers(hist);
			bp::list pyhist;
			for (size_t i = 0; i < 256; i++)
				pyhist.append(hist[i]);

			return pyhist;
		}

		// only for uchar
		bp::list calcHistByPointsSize(/*Barscalar maxLen*/)
		{
			int rm = 0;
			for (size_t i = 0; i < barlines.size(); i++)
			{
				int rf = barlines[i]->getPointsSize();
				if (rf > rm)
					rm = rf;
			}
			int* hist = new int[rm];
			memset(hist, 0, rm * sizeof(int));

			for (size_t i = 0; i < barlines.size(); i++)
				++hist[barlines[i]->getPointsSize()];

			bp::list pyhist;
			for (size_t i = 0; i < rm; i++)
				pyhist.append(hist[i]);

			delete[] hist;

			return pyhist;
		}


		bp::list getBarcode()
		{
			bp::list lines;
			for (auto* line : barlines)
			{
				// on deliting list will call ~destr for every line
				lines.append(line->clone());
			}
			return lines;
		}

		float cmp(const Baritem* bitem, bc::CompireStrategy strat) const
		{
			return compireFull((const Baritem*)bitem, strat);
		}


#endif // _PYD

		void sortByLen();
		void sortBySize();
		void sortByStart();
	};

	//template<size_t N>

	class BAREXPORT Barcontainer : public Barbase
	{
		std::vector<Baritem*> items;
	public:
		Barcontainer();

		Barscalar sum() const;
		void relen();
		Barbase* clone() const;
		Barscalar maxLen() const;
		size_t count();
		//    Baritem *operator [](int i);
		Baritem *getItem(size_t i);

		void setItem(size_t index, Baritem *newOne)
		{
			if (index < items.size())
			{
				auto *item = items[index];
				delete item;
				items[index] = newOne;
			}
		}

		Baritem *exractItem(size_t index)
		{
			if (index < items.size())
			{
				auto *item = items[index];
				items[index] = nullptr;
				return item;
			}
			return nullptr;
		}

		void remoeLast()
		{
			size_t s = items.size();
			if (s > 0)
			{
				delete items[s - 1];
				items[s - 1] = nullptr;
				items.pop_back();
			}
		}

		void exractItems(std::vector<Baritem *> extr)
		{
			for (size_t i = 0; i < items.size(); ++i)
			{
				if (items[i]!=nullptr)
					extr.push_back(items[i]);
			}
			items.clear();
		}
		Baritem* lastItem();
		void addItem(Baritem* item);
		// remove lines than less then passed value
		void removePorog(Barscalar const porog);
		void preprocessBar(Barscalar const& porog, bool normalize);

		float compireFull(const Barbase* bc, bc::CompireStrategy strat) const;
		float compireBest(const Baritem* bc, bc::CompireStrategy strat) const;

		void clear()
		{
			for (size_t i = 0; i < items.size(); ++i)
			{
				if (items[i] != nullptr)
					delete items[i];
			}
			items.clear();
		}
		//    void fullCompite(const barbase *bc, CompireFunction fn, float poroc = 0.5f);
		~Barcontainer();

		// Barbase interface
	};
}


//#define POINTS_ARE_AVAILABLE
namespace bc
{
	class BarcodeCreator;

	class Component
	{
	public:
#ifdef POINTS_ARE_AVAILABLE
		size_t getTotalSize()
		{
			return resline->matr.size();
		}
#else
		size_t startIndex = 0;
		size_t getTotalSize()
		{
			return totalCount;
		}
	private:
		size_t totalCount = 0/*, ownSize = 0*/;
#endif // !POINTS_ARE_AVAILABLE
	protected:
		BarcodeCreator* factory;
		Component* cachedMaxParent = nullptr;

	public:
		Component* parent = nullptr;
		barline* resline = nullptr;

	protected:
		int cashedSize = 0;
		Barscalar lastVal = 0;
		bool same = true;
		//Barscalar avgSr = 0;
		//point startPoint;
		bool lived = true;

		//float sums = 0;

	private:
		//0 - nan

		void init(BarcodeCreator* factory, const Barscalar& val);
	public:


		Component(poidex pix, const Barscalar& val, bc::BarcodeCreator* factory);
		Component(BarcodeCreator* factory, bool create = false);

		int getLastRowSize()
		{
			return cashedSize;
		}

		Barscalar getStart();

		Barscalar getLast()
		{
			return lastVal;
		}

		bool isAlive()
		{
			return lived;
		}

		bool justCreated();

		//Barscalar len()
		//{
		//	//return round(100000 * (end - start)) / 100000;
		//	return  abs(resline->len());
		//	//return end - start;
		//}
		//    cv::Mat binmap;
		Component* getMaxparent()
		{
			if (parent == nullptr)
				return this;

			if (cachedMaxParent == nullptr)
			{
				cachedMaxParent = parent;
			}
			while (cachedMaxParent->parent)
			{
				cachedMaxParent = cachedMaxParent->parent;
				//totalCount += cachedMaxParent->coords->size();
			}
			return cachedMaxParent;
		}


		bool isContain(poidex index);
		virtual bool add(const poidex index, const point p, const Barscalar& col, bool forsed = false);
		void kill();
		virtual void kill(const Barscalar& endScalar);
		virtual void setParent(Component* parnt);


		bool canBeConnected(const bc::point& p, bool incrSum = false);

		virtual ~Component();

	};

	typedef barmapHash<point, Component*, pointHash> cmap;
}
namespace bc
{


class Hole : public Component
{
    int index = 0;
public:
    Barscalar start, end;
    bool isValid = false;
    pmap crossHoles;

    Hole(point p1, point p2, point p3, BarcodeCreator* factory);
    Hole(point p1, BarcodeCreator* factory);
    ~Hole();

//        void setOutside(bool b) {
//            isOutside = b;

//        }
//        inline bool getOutside() { return isOutside;}
//    void addAround();
    bool tryAdd(const point &p);
    //add withot checks. NOT SAFE. not add if valid
    //************************************************************************//
    bool checkValid(const bc::point& p);
    //добовляет точку в дыру, если она там не содержится. Возвращает

    //явяется ли точка точкой соединения двух дыр
    bool findCross(point p, Hole* hole);
    bool isContain(int x, int y);
    bool isContain(const bc::point& p);
};

}




#define COMPP Component*
#define HOLEP Hole*


namespace bc {

	// # - �������; * - ������� �����
	// 00#
	// 0*#
	// 0##
	enum nextPoz : char
	{
		//topRight = 0,
		middleRight = 1,
		bottomRight = 2,

		//topCenter,
		//middleCenter,
		bottomCenter,

		//topLeft,
		//middleLeft,
		bottomLeft
	};
	struct indexCov
	{
		poidex offset = 0;
		float dist = 0;
		nextPoz poz;
		indexCov(uint _offset = 0, float _dist = 0, nextPoz _vert = middleRight) : offset(_offset), dist(_dist), poz(_vert)
		{}

		bc::point getNextPoint(const bc::point& p) const
		{
			switch (poz)
			{
			case middleRight:
				return bc::point(p.x + 1, p.y);
			case bottomRight:
				return bc::point(p.x + 1, p.y + 1);
			case bottomCenter:
				return bc::point(p.x, p.y + 1);
			case bottomLeft:
				return bc::point(p.x - 1, p.y + 1);
			}
		}
	};


	using Include = Component*;


	class BAREXPORT BarcodeCreator
	{
		typedef bc::DatagridProvider bcBarImg;

		bool originalImg = true;
		std::vector<COMPP> components;
	private:
#ifdef USE_OPENCV
		std::vector<cv::Vec3b> colors;
#endif

		BarConstructor settings;
		bool skipAddPointsToParent = false;

		Include* included = nullptr;
		const DatagridProvider* workingImg = nullptr;
		void setWorkingImg(const bcBarImg* newWI)
		{
			if (workingImg != nullptr && needDelImg)
				delete workingImg;

			workingImg = newWI;

			if (!settings.stepPorog.isCached || !settings.maxLen.isCached)
			{
				Barscalar maxVal, minVal;
				workingImg->maxAndMin(minVal, maxVal);

				if (!settings.stepPorog.isCached)
					settings.stepPorog.set(maxVal - minVal);

				if (!settings.maxLen.isCached)
					settings.maxLen.set(maxVal - minVal);
			}
		}

		BarType type;

		bool needDelImg = false;
		Barscalar curbright;
		poidex curpoindex;
		uint curIndexInSortedArr;
		//uint compIndex = 0;
		point curpix;
		int wid;
		int hei;
		//Barscalar sourceMax;
		//Barscalar sourceMin;
		// int lastB;
		friend class Component;
		friend class Hole;
		//		friend struct BarRoot;
		friend class Baritem;

		size_t processCount = 0;
		size_t totalSize = 0;
		poidex* sortedArr = nullptr;
		//bc::BarImg drawimg;

		//***************************************************
		constexpr bool IS_OUT_OF_REG(int x, int y)
		{
			return x < 0 || y < 0 || x >= wid || y >= hei;
		}
		poidex GETPOFF(const point& p) const
		{
			return wid * p.y + p.x;
		}

		poidex GETOFF(uint x, uint y) const {
			return wid * y + x;
		}

		bool GETDIFF(const Barscalar& a, const Barscalar& b) const
		{
			return !this->settings.stepPorog.isCached || (a > b ? a - b : b - a) <= this->settings.getMaxStepPorog();
		}

		point getPoint(poidex i) const
		{
			return point(i % wid, i / wid);
		}

		//#define GETPOFF(P) (this->wid*P.y+P.x)
		//#define GETOFF(X, Y) (this->wid*y+x)

		bool isContain(poidex ind) const
		{
			return included[ind] != nullptr;
		}

		inline void setInclude(const poidex ind, COMPP comp)
		{
			included[ind] = comp;
		}

		inline COMPP getComp(poidex ind)
		{
			auto itr = included[ind];
			return itr ? itr->getMaxparent() : nullptr;
		}


		COMPP getPorogComp(const point& p, poidex index);

		COMPP getInclude(size_t pos);

		// ONLY FOR HOLE
		bool isContain(const point& p) const
		{
			if (p.x < 0 || p.y < 0 || p.x >= wid || p.y >= hei)
				return false;

			return included[wid * p.y + p.x] != nullptr;
		}

		HOLEP getHole(uint x, uint y);
		HOLEP getHole(const point& p);

		COMPP attach(COMPP first, COMPP second);
		HOLEP tryAttach(HOLEP h1, HOLEP h2, point p);

		bool checkCloserB0();
		bool checkCloserB1();


		int sortOrtoPixels(bc::ProcType type, int rtoe = 0, int off = 0, int offDop = 0);
		void sortPixels(bc::ProcType type);

		void clearIncluded();

		void draw(std::string name = "test");
		void VISUAL_DEBUG();
		void VISUAL_DEBUG_COMP();


		void init(const bc::DatagridProvider* src, ProcType& type, ComponentType& comp);

		void processComp(Barcontainer* item = nullptr);
		void processHole(Barcontainer* item = nullptr);
		//void processHole255to0(bcBarImg& img, int* retBty, Barcontainer* item = nullptr);

		void processTypeF(barstruct& str, const bc::DatagridProvider* img, Barcontainer* item = nullptr);

		void processFULL(barstruct& str, const bc::DatagridProvider* img, bc::Barcontainer* item);
		void addItemToCont(Barcontainer* item);

		void computeNdBarcode(Baritem* lines, int n);

	public:

		bc::Barcontainer* createBarcode(const bc::DatagridProvider* img, const BarConstructor& structure);
		//bc::Barcontainer* searchHoles(float* img, int wid, int hei, float nullVal = -9999);


		virtual ~BarcodeCreator()
		{
			//			clearIncluded();
#ifdef USE_OPENCV
			colors.clear();
#endif // USE_OPENCV
		}

#ifdef _PYD

		bc::Barcontainer* createBarcode(bn::ndarray& img, bc::BarConstructor& structure);
#endif // _PYD

		///////////GEOMETRY
	private:
		void processCompByRadius(Barcontainer* item);
		void processCompByStepRadius(Barcontainer* item);
		void processByValueRadius(Barcontainer* item);

		void processRadar(const indexCov& val, bool allowAttach);


		un_ptr<indexCov> geometrySortedArr;
	};

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
		static std::function<void(const point&, const point&, bool)> drawLine;
		static std::function<void(PloyPoints&, bool)> drawPlygon;
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
		un_ptr<PointIndexCov> sortedArr;
	};
}
#ifndef SKIP_M_INC


#endif

bc::Barbase::~Barbase() {}


bc::Baritem::Baritem(int wid, BarType type)
{
	this->wid = wid;
	this->type = type;
}

void bc::Baritem::add(Barscalar st, Barscalar len)
{
	barlines.push_back(new barline(st, len, wid));
}

void bc::Baritem::add(bc::barline* line)
{
	barlines.push_back(line);
}

Barscalar bc::Baritem::sum() const
{
	Barscalar ssum = 0;
	for (const barline* l : barlines)
		ssum += l->len();

	return ssum;
}

void bc::Baritem::getBettyNumbers(int* bs)
{
	memset(bs, 0, 256 * sizeof(int));

	for (const barline* l : barlines)
	{
		for (size_t i = l->start.data.b1; i < l->end().data.b1; ++i)
		{
			++bs[i];
		}
	}
}

//
//void cloneGraph(bc::barline* old, bc::barline* newone)
//{
//	for (size_t i = 0; i < old->children.size(); i++)
//	{
//		cloneGraph(old->children[i], newone->children[i]);
//		old->children[i] = newone->children[i]->clone();
//	}
//}

bc::Baritem* bc::Baritem::clone() const
{
	barmap<barline*, barline*> oldNew;
	Baritem* nb = new Baritem(wid);
	nb->barlines.insert(nb->barlines.begin(), barlines.begin(), barlines.end());
	bool createGraph = false;
	if ((barlines.size() > 0 && barlines[0]->parent != nullptr) || barlines[0]->children.size() > 0)
		createGraph = true;

	for (size_t i = 0, total = nb->barlines.size(); i < total; ++i)
	{
		auto* nnew = nb->barlines[i]->clone();
		if (createGraph)
			oldNew.insert(std::pair<barline*, barline*>(nb->barlines[i], nnew));

		nb->barlines[i] = nnew;
	}
	if (createGraph)
	{
		for (size_t i = 0, total = nb->barlines.size(); i < total; ++i)
		{
			auto* nline = nb->barlines[i];
			nline->parent = oldNew[nline->parent];

			for (size_t i = 0; i < nline->children.size(); i++)
				nline->children[i] = oldNew[nline->children[i]];
		}
	}
	return nb;
}

Barscalar bc::Baritem::maxLen() const
{
	Barscalar max = 0;
	for (const barline* l : barlines)
		if (l->len() > max)
			max = l->len();

	return max;
}

void bc::Baritem::relen()
{
	if (barlines.size() == 0)
		return;

	Barscalar mini = barlines[0]->start;
	for (size_t i = 1; i < barlines.size(); ++i)
		if (barlines[i]->start < mini)
			mini = barlines[i]->start;

	for (size_t i = 0; i < barlines.size(); ++i)
		barlines[i]->start -= mini;

	//mini = std::min_element(arr.begin(), arr.end(), [](barline &b1, barline &b2) { return b1.start < b2.start; })->start;
	//std::for_each(arr.begin(), arr.end(), [mini](barline &n) {return n.start - uchar(mini); });
}

void bc::Baritem::removePorog(const Barscalar porog)
{
	if (porog == 0)
		return;
	std::vector<barline*> res;
	for (size_t i = 0; i < barlines.size(); i++)
	{
		barline* line = barlines[i];
		if (line->len() >= porog)
			res.push_back(line);
		else// if (line->isCopy)
			delete line;
	}
	barlines.clear();
	barlines.insert(barlines.begin(), res.begin(), res.end());
}

void bc::Baritem::preprocessBar(const Barscalar& porog, bool normalize)
{
	this->removePorog(porog);

	if (normalize)
		this->relen();
}

float findCoof(bc::barline* X, bc::barline* Y, bc::CompireStrategy& strat)
{
	const Barscalar &Xst = X->start < X->end() ? X->start : X->end();
	const Barscalar &Xed = X->start < X->end() ? X->end() : X->start;

	const Barscalar &Yst = Y->start < Y->end() ? Y->start : Y->end();
	const Barscalar &Yed = Y->start < Y->end() ? Y->end() : Y->start;


	float maxlen, minlen;
	if (strat == bc::CompireStrategy::CommonToSum)
	{
		if (Xst == Yst && Xed == Yed)
			return 1.f;

		float st = MAX(Xst, Yst).getAvgFloat();
		float ed = MIN(Xed, Yed).getAvgFloat();
		minlen = ed - st;

		st = MIN(Xst, Yst).getAvgFloat();
		ed = MAX(Xed, Yed).getAvgFloat();
		maxlen = ed - st;
	}
	else if (strat == bc::CompireStrategy::CommonToLen)
	{
		if (Xst == Yst && Xed == Yed)
			return 1.f;

		// Start всегда меньше end
		float st = MAX(Xst, Yst).getAvgFloat();
		float ed = MIN(Xed, Yed).getAvgFloat();
		minlen = ed - st; // Может быть меньше 0, если воообще не перекасаются
		maxlen = MAX(X->lenFloat(), Y->lenFloat());
	}
	else
	{
		return X->compire3dbars(Y, strat);
	}

	if (minlen <= 0 || maxlen <= 0)
		return -1;

	assert(minlen <= maxlen);
	return minlen / maxlen;
}



void soirBarlens(bc::barlinevector& barl)
{
	std::sort(barl.begin(), barl.end(), [](const bc::barline* a, const bc::barline* b)
		{
			return a->len() > b->len();
		});
}

float bc::Baritem::compireBestRes(const bc::Baritem* bc, bc::CompireStrategy strat) const
{
	barlinevector Xbarlines = barlines;
	barlinevector Ybarlines = dynamic_cast<const Baritem*>(bc)->barlines;

	if (Xbarlines.size() == 0 || Ybarlines.size() == 0)
		return 0;

	float totalsum = 0.f;
	int n = static_cast<int>(MIN(barlines.size(), Ybarlines.size()));

	float tsum = 0.f;
	for (int re = 0; re < n; ++re)
	{
		float maxCoof = 0;
		float maxsum = 0;
		size_t ik = 0;
		size_t jk = 0;
		for (size_t i = 0, totalI = Xbarlines.size(); i < totalI; ++i)
		{
			for (size_t j = 0, totalY = Ybarlines.size(); j < totalY; ++j)
			{
                float coof = findCoof(Xbarlines[i], Ybarlines[j], strat);
				if (coof < 0)
					continue;

				if (coof > maxCoof)
				{
					maxCoof = coof;
					maxsum = Xbarlines[i]->len().getAvgFloat() + Ybarlines[j]->len().getAvgFloat();
					ik = i;
					jk = j;
				}
			}
		}
		Xbarlines.erase(Xbarlines.begin() + ik);
		Ybarlines.erase(Ybarlines.begin() + jk);
		tsum += maxsum * maxCoof;
		totalsum += maxsum;
	}
	return tsum / totalsum;
}

float bc::Baritem::compireFull(const bc::Barbase* bc, bc::CompireStrategy strat) const
{
	barlinevector Xbarlines = barlines;
	barlinevector Ybarlines = dynamic_cast<const Baritem*>(bc)->barlines;

	if (Xbarlines.size() == 0 || Ybarlines.size() == 0)
		return 0;

	float totalsum = 0;
    size_t n = MIN(Xbarlines.size(), Ybarlines.size());
	soirBarlens(Xbarlines);
	soirBarlens(Ybarlines);

	float tcoof = 0.f;
	for (size_t i = 0; i < n; ++i)
	{
        float coof = findCoof(Xbarlines[i], Ybarlines[i], strat);
		if (coof < 0)
			continue;

		float xysum = static_cast<float>(Xbarlines[i]->len()) + static_cast<float>(Ybarlines[i]->len());
		totalsum += xysum;
		tcoof += xysum * coof;
	}
	return totalsum !=0 ? tcoof / totalsum : 0;
}

float bc::Baritem::compareOccurrence(const bc::Baritem* bc, bc::CompireStrategy strat) const
{
	barlinevector Xbarlines = barlines;
	barlinevector Ybarlines = dynamic_cast<const Baritem*>(bc)->barlines;

	if (Xbarlines.size() == 0 || Ybarlines.size() == 0)
		return 0;

	size_t n = static_cast<int>(MIN(Xbarlines.size(), Ybarlines.size()));
	soirBarlens(Xbarlines);
	soirBarlens(Ybarlines);

	float coofsum = 0.f, totalsum = 0.f;
	for (size_t re = 0; re < n; ++re)
	{
		float maxCoof = 0;
		float maxsum = 0;
		size_t jk = 0;
		for (size_t j = 0, total2 = Ybarlines.size(); j < total2; ++j)
		{
            float coof = findCoof(Xbarlines[re], Ybarlines[j], strat);
			if (coof < 0)
				continue;

			if (coof > maxCoof)
			{
				maxCoof = coof;
				maxsum = (float)(Xbarlines[re]->len() + Ybarlines[j]->len());
				jk = j;
			}
		}
		Ybarlines.erase(Ybarlines.begin() + jk);
		totalsum += maxsum;
		coofsum += maxsum * maxCoof;
	}
	return coofsum / totalsum;
}

void bc::Baritem::normalize()
{
	if (barlines.size() == 0)
		return;

	Barscalar mini = barlines[0]->start;
	Barscalar maxi = barlines[0]->end();
	for (size_t i = 1; i < barlines.size(); ++i)
	{
		if (barlines[i]->start < mini)
			mini = barlines[i]->start;
		if (barlines[i]->end() > maxi)
			maxi = barlines[i]->end();
	}

	for (size_t i = 0; i < barlines.size(); ++i)
	{
		barlines[i]->start = (barlines[i]->start - mini) / (maxi - mini);
		barlines[i]->m_end = barlines[i]->start + (barlines[i]->len() - mini) / (maxi - mini);
	}
}

void bc::Baritem::sortByLen()
{
	soirBarlens(barlines);
}

void bc::Baritem::sortBySize()
{
	std::sort(barlines.begin(), barlines.end(), [](const bc::barline* a, const bc::barline* b)
		{
			return a->matr.size() > b->matr.size();
		});
}

void bc::Baritem::sortByStart()
{
	std::sort(barlines.begin(), barlines.end(), [](const bc::barline* a, const bc::barline* b)
		{
			return a->start > b->start;
		});
}

bc::Baritem::~Baritem()
{
	for (auto* bline : barlines)
	{
		if (bline != nullptr)
			delete bline;
	}
	barlines.clear();

	if (rootNode != nullptr)
		delete rootNode;
}

//=======================barcontainer=====================


bc::Barcontainer::Barcontainer()
{
}

Barscalar bc::Barcontainer::sum() const
{
	Barscalar sm = 0;
	for (const Baritem *it : items)
	{
		if (it!=nullptr)
			sm += it->sum();
	}
	return sm;
}

void bc::Barcontainer::relen()
{
	for (Baritem *it : items)
	{
		if (it!=nullptr)
			it->relen();
	}
}

Barscalar bc::Barcontainer::maxLen() const
{
	Barscalar mx = 0;
	for (const Baritem* it : items)
	{
		if (it!=nullptr)
		{
			Barscalar curm = it->maxLen();
			if (curm > mx)
				mx = curm;
		}
	}

	return mx;
}

size_t bc::Barcontainer::count()
{
	return items.size();
}

bc::Baritem* bc::Barcontainer::getItem(size_t i)
{
	if (items.size() == 0)
		return nullptr;

	while (i < 0)
		i += items.size();

	while (i >= items.size())
		i -= items.size();

	return items[i];
}

//bc::Baritem *bc::Barcontainer::operator[](int i)
//{
//    if (items.size() == 0)
//        return nullptr;

//    while (i < 0)
//        i += items.size();

//    while (i >= (int) items.size())
//        i -= items.size();

//    return items[i];
//}


bc::Baritem* bc::Barcontainer::lastItem()
{
	if (items.size() == 0)
		return nullptr;

	return items[items.size() - 1];
}


void bc::Barcontainer::addItem(bc::Baritem* item)
{
	items.push_back(item);
}


void bc::Barcontainer::removePorog(const Barscalar porog)
{
	for (Baritem *it : items)
	{
		if (it!=nullptr)
			it->removePorog(porog);
	}
}


void bc::Barcontainer::preprocessBar(const Barscalar& porog, bool normalize)
{
	for (Baritem *it : items)
	{
		if (it!=nullptr)
			it->preprocessBar(porog, normalize);
	}
}


bc::Barbase* bc::Barcontainer::clone() const
{
	Barcontainer* newBar = new Barcontainer();

	for (Baritem* it : items)
	{
		if (it!=nullptr)
			newBar->items.push_back(new Baritem(*it));
	}
	return newBar;
}


float bc::Barcontainer::compireFull(const bc::Barbase* bc, bc::CompireStrategy strat) const
{
	const Barcontainer* bcr = dynamic_cast<const Barcontainer*>(bc);
    float res = 0;
    float s = static_cast<float>(sum() + bcr->sum());
	for (size_t i = 0; i < MIN(items.size(), bcr->items.size()); i++)
	{
		if (items[i]!=nullptr)
			res += items[i]->compireFull(bcr->items[i], strat) * static_cast<float>(items[i]->sum() + bcr->items[i]->sum()) / s;
	}

    return res;
}

float bc::Barcontainer::compireBest(const bc::Baritem* bc, bc::CompireStrategy strat) const
{
	float res = 0;
	for (size_t i = 0; i < items.size(); i++)
	{
		if (items[i] != nullptr)
			res = MAX(items[i]->compireFull(bc, strat), res);
	}

	return res;
}

bc::Barcontainer::Barcontainer::~Barcontainer()
{
	clear();
}


#ifdef USE_OPENCV

//-------------BARIMG

Barscalar bc::BarMat::get(int x, int y) const
{
	if (type == BarType::BYTE8_3)
	{
		cv::Vec3b v = mat.at<cv::Vec3b>(y, x);
		return Barscalar(v.val[0], v.val[1], v.val[2]);
	}
	else
	{
		return Barscalar(mat.at<uchar>(y, x), BarType::BYTE8_1);
	}
}

#endif // USE_OPENCV



bc::BarImg& bc::BarImg::operator+(const Barscalar& v)
{
	BarImg* box = this->getCopy();

	//box->addToMat(v);
	assert(false);

	return *box;
}

//// Overload + operator to add two Box objects.
bc::BarImg& bc::BarImg::operator-(const Barscalar& v)
{
	bc::BarImg* box = this->getCopy();

	assert(false);
	for (size_t i = 0; i < box->length(); ++i)
	{
		Barscalar val = box->getLiner(i);
		val -= v;
		assert(box->getLiner(i) == val);
	}
	return *box;
}
//
//bc::BarImg bc::BarImg::operator+(const Barscalar& c1, BarImg& c2)
//{
//	// use the Cents constructor and operator+(int, int)
//	// we can access m_cents directly because this is a friend function
//	BarImg nimg = c2;
//	nimg.addToMat(c1);
//	return nimg;
//}
//
//
//bc::BarImg bc::BarImg::operator-(const Barscalar& c1, const BarImg& c2)
//{
//	// use the Cents constructor and operator+(int, int)
//	// we can access m_cents directly because this is a friend function
//	BarImg ret(1, 1);
//	ret.assignCopyOf(c2);
//	ret.minusFrom(c1);
//	return ret;
//}


bc::Hole::Hole(point p1, BarcodeCreator* factory) : Component(factory)
{
	isValid = false;
	add(factory->GETPOFF(p1), p1, factory->curbright);
	index = factory->components.size() - 1;
}


bc::Hole::Hole(point p1, point p2, point p3, BarcodeCreator* factory) : Component(factory)
{
	isValid = true;
	//    zeroStart = p1;
		// ++factory->lastB;

	add(factory->GETPOFF(p1), p1, factory->curbright);
	add(factory->GETPOFF(p2), p2, factory->curbright);
	add(factory->GETPOFF(p3), p3, factory->curbright);
}


bc::Hole::~Hole()
{
	if (!isValid)
		Component::factory->components[index] = nullptr;
}


 bool bc::Hole::isContain(int x, int y)
{
	if (Component::factory->IS_OUT_OF_REG(x, y))
		return false;
	return Component::factory->getComp(Component::factory->GETOFF(x, y)) == this;
}


bool bc::Hole::isContain(const bc::point& p)
{
	if (Component::factory->IS_OUT_OF_REG(p.x, p.y))
		return false;
	return Component::factory->getComp(Component::factory->GETPOFF(p)) == this;
}


bool bc::Hole::tryAdd(const point& p)
{
	if (isValid == false)
	{
		/*addCoord(p);
		return true;*/
		return false;
	}

	if (this->isContain(p))
	{
		return false;
	}

	static char poss[9][2] = { { -1,0 },{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 },{ -1,0 } };
	for (char i = 0; i < 8; ++i)
	{
		//Она соединяется только с соседними ближйми ребрами
		if (this->isContain(p + poss[i]) && (this->isContain(p + poss[i + 1]) ||
		   (i % 2 == 0 && this->isContain(p + poss[i + 2])))
			)//есть ли нужное ребро
		{
			poidex px = factory->GETPOFF(p);
			this->add(px, p, factory->curbright);
			return true;
		}
	}
	return false;
}


bool bc::Hole::checkValid(const bc::point& p)
{
	if (this->getTotalSize() < 3)
		return false;

	if (isValid)
		return true;

	static char poss[9][2] = { { -1,0 },{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 },{ -1,0 } };
	//эти сочетания могу образовывать дубли, поэтому перед добавление СЛЕДУЕТ ПРОВЕРЯТЬ, был ли уже добавлен такой треугольник

	//точка p в цетре. Сканирем ребра вокруг точки
	for (size_t i = 0; i < 8; ++i)
	{
		//Она соединяется только с соседними ближйми ребрами
		//FIXME Same expression on both sides of '||'.
		if (this->isContain(p + poss[i]) && (this->isContain(p + poss[i + 1]) || this->isContain(p + poss[i + 1])))//есть ли нужное ребро
		{
			isValid = true;
			return true;
		}
	}
	return false;
}


//явяется ли точка точкой соединения двух дыр - рис1

bool bc::Hole::findCross(point p, bc::Hole* hole)
{
	static char poss[5][2] = { { -1,0 },{ 0,-1 },{ 1,0 },{ 0,1 },{ -1,0 } };
	static char poss2[5][2] = { { -1,-1 },{ 1,-1 },{ 1,1 },{ -1,1 } };
	//static char poss[9][2] = { { -1,0 },{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 }};
	if (this->isContain(p) || hole->isContain(p))
	{
		//ВАЖНО! Две дыры можно соединить если у ниъ есть 2 общие точки. Если мы знаем одну, то вторая должна быть четко над/под/слева/справа от предыдужей.
		//При дургом расопложении дыры не соединятся
		for (size_t i = 0; i < 4; ++i)
		{
			if (this->isContain(p + poss[i]) && hole->isContain(p + poss[i]))//если есть ребро, оразующиееся из разных дыр
				return true;

			//************cdoc 15**************
			if (this->isContain(p + poss[i]) && hole->isContain(p + poss[i + 1]))//если есть ребро, оразующиееся из разных дыр
				return true;
			if (this->isContain(p + poss[i + 1]) && hole->isContain(p + poss[i]))//если есть ребро, оразующиееся из разных дыр
				return true;
			//*********************************
		}
		//**********CDOC 590**************
		for (size_t i = 0; i < 4; ++i)
		{
			if (this->isContain(p + poss2[i]) && hole->isContain(p + poss2[i]))//если есть ребро, оразующиееся из разных дыр
				return true;
		}
		//********************************
	}
	return false;
}




void bc::Component::init(BarcodeCreator* factory, const Barscalar& val)
{
#ifndef POINTS_ARE_AVAILABLE
	startIndex = factory->curIndexInSortedArr;
#endif // !POINTS_ARE_AVAILABLE

	this->factory = factory;
	//	index = factory->components.size();
	factory->components.push_back(this);

	resline = new barline(factory->workingImg->wid());
	resline->start = val;
	resline->m_end = val;
	lastVal = val;

	if (factory->settings.returnType == bc::ReturnType::barcode3d ||
		factory->settings.returnType == bc::ReturnType::barcode3dold)
		resline->bar3d = new barcounter();
}


bc::Component::Component(poidex pix, const Barscalar& val, bc::BarcodeCreator* factory)
{
	init(factory, val);

	// factory->lastB++;

	add(pix, factory->getPoint(pix), val);
}


bc::Component::Component(bc::BarcodeCreator* factory, bool /*create*/)
{
	init(factory, factory->curbright);
}

Barscalar bc::Component::getStart()
{
	assert(resline != NULL);
	return resline->start;
}

bool bc::Component::justCreated()
{
	return lastVal == factory->curbright && same;
}


bool bc::Component::isContain(poidex index)
{
	return factory->getComp(index) == this;
}


bool bc::Component::add(const poidex index, const point p, const Barscalar& col, bool forsed)
{
	assert(lived);

	if (!forsed)
	{
		//if (!canBeConnected(p, true))
		//	return false;

		if (cashedSize == factory->settings.colorRange)
		{
			return false;
		}
	}

#ifndef POINTS_ARE_AVAILABLE
	assert(getMaxparent() == this);
	++getMaxparent()->totalCount;
#endif // !POINTS_ARE_AVAILABLE


	factory->setInclude(index, this);

	if (factory->settings.createBinaryMasks)
	{
		resline->addCoord(p, col);
	}
	bool eq = col == lastVal;
	same = same && eq;
	// 3d barcode/ —читаем кол-во добавленных значений
	if (!eq)
	{
		if (factory->settings.returnType == ReturnType::barcode3d)
		{
			resline->bar3d->push_back(bar3dvalue(lastVal, cashedSize)); //всего
		}
		else if (factory->settings.returnType == ReturnType::barcode3dold)
		{
			resline->bar3d->push_back(bar3dvalue(lastVal, cashedSize)); // сколкьо было доабвлено
			cashedSize = 0;
		}
	}

	++cashedSize;
	lastVal = col;

	return true;
}

void bc::Component::kill()
{
	kill(lastVal);
}

void bc::Component::kill(const Barscalar& endScalar)
{
	if (!lived)
		return;
	lived = false;

	resline->m_end = endScalar;

	//if (col < resline->start)
	//	resline->start = col;
	//if (col > resline->m_end)
	//	resline->m_end = col;

//	assert(resline->len() != 0);

	if (factory->settings.returnType == ReturnType::barcode3dold)
	{
		resline->bar3d->push_back(bar3dvalue(lastVal, cashedSize));
	}
	else if (factory->settings.returnType == ReturnType::barcode3d)
	{
		resline->bar3d->push_back(bar3dvalue(lastVal, cashedSize));
	}

	if (parent == nullptr && factory->settings.createBinaryMasks)
	{
		// Not working for radius
		//Barscalar bot = resline->start;
		//Barscalar top = resline->m_end;

		//if (bot > top)
		//{
		//	bot = resline->m_end;
		//	top = resline->start;
		//}

		for (barvalue& a : resline->matr)
		{
			//assert(bot <= a.value);
			//assert(a.value <= top);
			a.value = resline->m_end.absDiff(a.value);
		}
	}

	lastVal = endScalar;
	cashedSize = 0;
}


void bc::Component::setParent(bc::Component* parnt)
{
	assert(parent == nullptr);
	this->parent = parnt;

#ifndef POINTS_ARE_AVAILABLE
	this->parent->totalCount += totalCount;
	parnt->startIndex = MIN(parnt->startIndex, startIndex);
	//parnt->sums += this->sums;
#endif // ! POINTS_ARE_AVAILABLE

	// at moment when this must be dead
	assert(lived);

	// Мы объединяем, потому что одинаковый добавился (но для оптимизации не добавлятся в конце)
	const Barscalar& endScalar = factory->curbright;
	if (factory->settings.createBinaryMasks && resline->matr.size() > 0)
	{
		parnt->resline->matr.reserve(parnt->resline->matr.size() + resline->matr.size() + 1);

		for (barvalue& val : resline->matr)
		{
			// Записываем длину сущщетвования точки
			val.value = endScalar.absDiff(val.value);
			//val.value = col - val.value;

			//avgSr += val.value;
			// Эти точки сичтаются как только что присоединившиеся
			parnt->resline->addCoord(barvalue(val.getPoint(), endScalar));
		}
		parnt->same = false;
		// Мы объединяем, потому что одинаковый добавился, т.е. считаем, что lasVal одинаковыйы
		//parnt->lastVal = lastVal;
	}

	kill(endScalar);

	if (factory->settings.createGraph)
		resline->setparent(parnt->resline);
}


bool bc::Component::canBeConnected(const bc::point& p, bool incrSum)
{
	return true;

	//	if (factory->settings.maxRadius < (lastVal.val_distance(factory->curbright)))
	//		return false;
	//
	////	if (!factory->settings.maxLen.isCached)
	////		return true;
	//	if (totalCount == 0)
	//		return true;

	return true;
}

bc::Component::~Component()
{
	//	factory->components[index] = nullptr;
}





#ifdef __linux
#endif // __linux

using namespace bc;

#pragma warning(disable : 4996)



static inline void split(const DatagridProvider& src, std::vector<BarImg*>& bgr)
{
	size_t step = static_cast<size_t>(src.channels()) * src.typeSize();
	for (int k = 0; k < src.channels(); k++)
	{
		BarImg* ib = new BarImg(src.wid(), src.hei());
		bgr.push_back(ib);

		for (size_t i = 0; i < static_cast<unsigned long long>(src.length()) * src.typeSize(); i += step)
		{
			ib->setLiner(i, src.getLiner(i));
		}
	}
}

//template<class Barscalar, class U>
//static inline void split(const DatagridProvider<BarVec3b>& src, std::vector<DatagridProvider<U>*>& bgr)
//{
//}

enum class BarConvert
{
	BGR2GRAY,
	GRAY2BGR,
};


inline void cvtColorU1C2V3B(const bc::DatagridProvider& source, bc::BarImg& dest)
{
	assert(source.channels() == 1);

	dest.resize(source.wid(), source.hei());

	for (size_t i = 0; i < source.length(); ++i)
	{
		Barscalar u = source.getLiner(i);
		u.data.b3[0] = u.data.b1;
		u.data.b3[1] = u.data.b1;
		u.data.b3[2] = u.data.b1;
		u.type = BarType::BYTE8_3;
		dest.setLiner(i, u);
	}
}

inline void cvtColorV3B2U1C(const bc::DatagridProvider& source, bc::BarImg& dest)
{
	assert(dest.channels() == 1);
	dest.resize(source.wid(), source.hei());

	for (size_t i = 0; i < source.length(); ++i)
	{
		float accum = source.getLiner(i).getAvgFloat();
		dest.setLiner(i, (uchar)accum);
	}
}
//template<class Barscalar, class U>
//static inline void cvtColor(const bc::DatagridProvider& source, bc::DatagridProvider<U>& dest)
//{

//}

//// note: this function is not a member function!

//BarImg operator+(const Barscalar& c1, BarImg& c2);
//BarImg operator-(const Barscalar& c1, const BarImg& c2);

#ifdef USE_OPENCV

cv::Mat convertProvider2Mat(DatagridProvider* img)
{
	cv::Mat m = cv::Mat::zeros(img->hei(), img->wid(), CV_8UC1);
	for (size_t i = 0; i < img->length(); i++)
	{
		auto p = img->getPointAt(i);
		m.at<uchar>(p.y, p.x) = img->get(p.x, p.y).data.b1;
	}
	return m;
	}


cv::Mat convertRGBProvider2Mat(const DatagridProvider* img)
{
	cv::Mat m = cv::Mat::zeros(img->hei(), img->wid(), CV_8UC3);
	for (size_t i = 0; i < img->length(); i++)
	{
		auto p = img->getPointAt(i);
		m.at<cv::Vec3b>(p.y, p.x) = img->get(p.x, p.y).toCvVec();
	}
	return m;
}

#endif // USE_OPENCV

void BarcodeCreator::draw(std::string name)
{
#ifdef USE_OPENCV

	int wd = wid * 10;
	int hi = hei * 10;
	cv::Mat img(hi, wd, CV_8UC3, cv::Scalar(255, 255, 255));
	cv::Vec3b v(100, 100, 100);
	size_t size = colors.size();
	//если 2 занимают одну клтку

	for (int i = 0; i < hi; i += 10)
		for (int j = 0; j < wd; j++) {
			img.at<cv::Vec3b>(i, j) = v;
			img.at<cv::Vec3b>(i + 1, j) = v;
		}


	for (int j = 0; j < wd; j += 10)
		for (int i = 0; i < hi; i++) {
			img.at<cv::Vec3b>(i, j) = v;
			img.at<cv::Vec3b>(i, j + 1) = v;
		}

	for (size_t i = 0; i < totalSize; i++) {
		//		Hole *phole = dynamic_cast<Hole *>(included[i]);
		//		if (phole == nullptr)
		//			continue;
		COMPP comp = getInclude(i);
		if (comp == nullptr || !comp->isAlive())
			continue;
		int x = static_cast<int>(i % wid);
		int y = static_cast<int>(i / wid);
		int tic = 1;
		int marc = cv::MARKER_TILTED_CROSS;
		cv::Vec3b col;

		cv::Point p(x, y);
		//		if (!phole->isValid)
		//		{
		//			marc = cv::MARKER_DIAMOND;
		//			col = cv::Vec3b(0, 0, 255);
		//		}
		//		else if (phole->getIsOutside())
		//		{
		//			col = cv::Vec3b(0, 0, 10);
		//			tic = 2;
		//			marc = cv::MARKER_CROSS;
		//		}
		//		else
		{
			Hole* hd = dynamic_cast<Hole*>(comp);
			col = colors[(size_t)comp->startIndex % size];

			marc = cv::MARKER_TILTED_CROSS;
		}

		p.x = p.x * 10 + 5;
		p.y = p.y * 10 + 5;
		cv::drawMarker(img, p, col, marc, 10, tic, cv::LINE_4);

	}
	cv::namedWindow(name, cv::WINDOW_GUI_EXPANDED);
	cv::imshow(name, img);

	const int corWin = 600;
	const int corHei = 500;
	if (wd > hi)
	{
		float ad = (float)corWin / wd;
		cv::resizeWindow(name, corWin, (int)(hi * ad));
	}
	else
	{
		float ad = (float)corHei / hi;
		cv::resizeWindow(name, (int)(wd * ad), corHei);
	}
#endif // USE_OPENCV
}


inline COMPP BarcodeCreator::attach(COMPP main, COMPP second)
{
	bool mJC = main->justCreated();
	bool sJC = second->justCreated();
	if ((mJC || sJC) && !(sJC && sJC)) // Строго или
	{
#ifdef POINTS_ARE_AVAILABLE
		for (const auto& val : second->resline->matr)
		{
			assert(workingImg->get(val.getX(wid), val.getY(wid)) == curbright);
			assert(included[val.getIndex()] == second);

			main->add(val.getIndex());
		}
#else
		// Если главный толко создан - меням местами
		if (mJC)
		{
			COMPP temp = main;
			main = second;
			second = temp;
		}

		for (size_t rind = 0, totalm = second->resline->matr.size(); rind < totalm; ++rind)
		{
			const barvalue& val = second->resline->matr[rind];
			const bc::point p = val.getPoint();
			main->add(p.getLiner(wid), p, val.value, true);
		}

		main->startIndex = MIN(second->startIndex, main->startIndex);
#endif // POINTS_ARE_AVAILABLE
		delete second->resline;
		second->resline = nullptr;

		return main;
	}

	//float bottom = (float)main->getStart();
	//float top = (float)main->getLast();
	//if (bottom > top)
	//	std::swap(bottom, top);

	//float bottm2 = (float)second->getStart();
	//float top2 = (float)second->getLast();
	//if (bottm2 > top2)
	//	std::swap(bottm2, top2);


	if (settings.maxLen.isCached)
	{
		Barscalar fs = main->getStart();
		Barscalar sc = second->getStart();
		Barscalar diff = (fs > sc) ? (fs - sc) : (sc - fs);
		if (diff > settings.getMaxLen())
		{
			return main;
		}
	}

	/*if (!((bottom <= bottm2 && bottm2 <= top) || (bottom <= top2 && top2 <= top)))
		return main;*/

	switch (settings.attachMode)
	{
	case AttachMode::dontTouch:
		return main;

	case AttachMode::secondEatFirst:
		if (main->startIndex < second->startIndex)
		{
			COMPP temp = main;
			main = second;
			second = temp;
		}
		second->setParent(main);
		break;

	case AttachMode::createNew:
		//if ((double)MIN(main->getTotalSize(), second->getTotalSize()) / totalSize > 0.05)
	{
		COMPP newOne = new Component(this, true);
		main->setParent(newOne);
		second->setParent(newOne);
		//main->kill();
		//second->kill();
		return newOne;
	}
	case AttachMode::morePointsEatLow:
		if (main->getTotalSize() < second->getTotalSize())
		{
			COMPP temp = main;
			main = second;
			second = temp;
		}
		second->setParent(main);
		return main;
		// else pass down

	case AttachMode::firstEatSecond:
	default:
		if (main->startIndex > second->startIndex)
		{
			COMPP temp = main;
			main = second;
			second = temp;
		}
		second->setParent(main);

		if (main->getLastRowSize() >= settings.colorRange)
		{
			main->kill();
			return new Component(this, true);
		}

		break;
	}
	//возращаем единую компоненту.
	return main;
}

//****************************************B0**************************************

inline bool BarcodeCreator::checkCloserB0()
{
	COMPP first = nullptr;
	COMPP connected;// = new rebro(x, y); //included[{(int)i, (int)j}];
	//TODO выделять паять заранее
	static char poss[9][2] = { { -1,0 },{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 },{ -1,0 } };

	//first = getComp(curpix);
	// FIXME first always will be nill
	for (int i = 0; i < 8; ++i)
	{
		point IcurPoint(curpix + poss[i]);

		if (IS_OUT_OF_REG(IcurPoint.x, IcurPoint.y))
			continue;


		poidex IcurPindex = IcurPoint.getLiner(wid);

		connected = getPorogComp(IcurPoint, IcurPindex);
		if (connected != nullptr)//существует ли ребро вокруг
		{
			if (first == nullptr)
			{
				first = connected;
				// if len more then maxlen, kill the component
				bool more = settings.maxLen.isCached && curbright.absDiff(first->getStart()) > settings.maxLen.getOrDefault(0);
				if (more)
				{
					//qDebug() << first->num << " " << curbright << " " << settings.maxLen.getOrDefault(0);
					if (settings.killOnMaxLen)
					{
						first->kill(curbright); //Интересный результат
					}
					first = nullptr;
				}
				else if (!first->add(curpoindex, curpix, curbright))
					first = nullptr;
				//setInclude(midP, first);//n--nt обяз нужно
			}
			else
			{
				if (first->isContain(IcurPindex))//если в найденном уже есть этот элемент
					continue;

				//lastB -= 1;
				if (first != connected && first->canBeConnected(IcurPoint))
					first = attach(first, connected);//проверить, чему равен included[point(x, y)] Не должно, ибо first заменяется на connect
			}
		}
	}

	if (first == nullptr)
	{
		//lastB += 1;

		new Component(curpoindex, curbright, this);
		return true;
	}
	return false;
}
//********************************************************************************




COMPP BarcodeCreator::getPorogComp(const point& p, poidex index)
{
	auto* itr = included[index];
	if (itr && GETDIFF(curbright, workingImg->get(p.x, p.y)))
	{
		COMPP val = itr->getMaxparent();
		return (val != nullptr && val->isAlive() ? val : nullptr);
	}
	else
		return nullptr;
}


COMPP BarcodeCreator::getInclude(const size_t pos)
{
	assert(pos < totalSize);
	return included[pos] ? included[pos]->getMaxparent() : nullptr;
}


HOLEP BarcodeCreator::getHole(uint x, uint y)
{
	if (x < 0 || y < 0 || x >= wid || y >= hei)
		return nullptr;

	auto itr = included[wid * y + x];
	return dynamic_cast<HOLEP>(itr ? itr->getMaxparent() : nullptr);
}


HOLEP BarcodeCreator::getHole(const point& p)
{
	auto itr = included[wid * p.y + p.x];
	return dynamic_cast<HOLEP>(itr ? itr->getMaxparent() : nullptr);
}


HOLEP BarcodeCreator::tryAttach(HOLEP main, HOLEP add, point p)
{
	if (main != add && main->findCross(p, add))
	{
		//если хотя бы одна из дыр аутсайд - одна дыра умрет. Если они обе не аутсайд - соединятся в одну и одна дыра умрет.
		//еси они уба уже аутсайды, то ничего не произйдет, получается, что они живут только если обе isOut =true, т.е.
		//умирают, если хотя бы один из них false
		//flase - жива, true - мертва
		//if (main->getIsOutside() == false || add->getIsOutside() == false)
		//	--lastB;

		//как будет после соединения
		//main->setShadowOutside(main->getIsOutside() || add->getIsOutside());
		//add->setShadowOutside(main->getIsOutside());

		HOLEP ret = dynamic_cast<HOLEP>(attach(main, add));
		//if (ret->getIsOutside() && curbright != ret->end)
		//	ret->end = curbright;

		return ret;//все connected и first соединились в одну компоненту first
	}
	return main;
}

//****************************************B1**************************************


inline bool BarcodeCreator::checkCloserB1()
{
	static char poss[9][2] = { { -1,0 },{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 },{ -1,0 } };//эти сочетания могу образовывать дубли, поэтому перед добавление СЛЕДУЕТ ПРОВЕРЯТЬ, был ли уже добавлен такой треугольник
	Hole* hr = nullptr;
	point p1;
	point p2;

	// 1. Ищем дыру или делаем её из мелких;
	//после обовления дыры  к  ней можно будет присоединить все токи вокруг нее, кроме тточ, что на противоположном углу
	for (int i = 0; i < 8; ++i)
	{
		p1 = curpix + poss[i];

		if (IS_OUT_OF_REG(p1.x, p1.y))
			continue;

		poidex pind1 = p1.getLiner(wid);
		if (isContain(pind1))
		{
			if (isContain(curpix + poss[i + 1]))
				p2 = curpix + poss[i + 1];
			else if (i % 2 == 0 && isContain(curpix + poss[i + 2]))
				p2 = curpix + poss[i + 2];
			else
				continue;//если не нашли. Проверяем только потелнциальные дыры

			Hole* h1 = getHole(p1);
			Hole* h2 = getHole(p2);
			//все проверки на out в самом конце
			//вариант 1 - они принадлежат одному объекту. Не валидные могут содержать только одну компоненту, значит, этот объект валидный
			if (h1 == h2 && h1->isValid)
			{
				h1->add(curpix.getLiner(wid), curpix, curbright);
				hr = h1;
			}
			//вариант 2 - h1 - валид, h2- не валид. Мы уже проверили, что треугольник p-p1-p2 есть
			//cod 402
			else if (h1->isValid && !h2->isValid)
			{
				delete h2;
				hr = new Hole(curpix, p1, p2, this);

				h1->tryAdd(curpix);
				hr = tryAttach(hr, h1, curpix);
			}
			//вариант 3 - h1 - не  валид, h2- валид.Мы уже проверили, что треугольник p-p1-p2 есть
			//cod 402
			else if (h2->isValid && !h1->isValid)
			{
				delete h1;
				hr = new Hole(curpix, p1, p2, this);
				h2->tryAdd(curpix);
				hr = tryAttach(hr, h2, curpix);
			}
			//вариант 4 - оба не валид
			else if (!h1->isValid && !h2->isValid)//не факт, что они не валидны
			{
				//Т.К. мы уже проверили вышле, что образуется треуготльник, можно смело создаать дыру
				delete h1;
				delete h2;
				hr = new Hole(curpix, p1, p2, this);

			}
			//вариант 5 - разные дыры и они валидны CDOC{590}
			else if (h1->isValid && h2->isValid && h1 != h2)
			{
				bool add1 = h1->tryAdd(curpix);
				bool add2 = h2->tryAdd(curpix);
				if (add1 && add2)
				{
					hr = tryAttach(h1, h2, curpix);
				}
				else if (add1 && !add2)
				{
					if (h1->tryAdd(p2))
						hr = tryAttach(h1, h2, p2);
				}
				else if (!add1 && add2)
				{
					if (h2->tryAdd(p1))
						hr = tryAttach(h2, h1, p1);
				}
			}
			//isout будет false, если одна из дыр до этого не была out. Outside может поменяться из false в true, но не наоборот.
			//Т.е. isOutDo!=isoutPosle будет true, если isOutDo=false, а isOutPosle=true.
			if (hr != nullptr)
				break;
		}
	}

	if (hr == nullptr)
	{
		hr = new Hole(curpix, this);
		return false;
	}

	bool added = false;
	// 2. Добалвяем недодыры
	for (char i = 0; i < 8; ++i)
	{
		point curp = curpix + poss[i % 8];

		if (isContain(curp))
		{
			Hole* h_t = getHole(curp);
			//получена дыра
			if (h_t == hr)
				continue;

			// Сначала добавляем все недодыры
			if (h_t->isValid)
				continue;

			added = added || hr->tryAdd(curp);
		}
	}
	if (added)
	{
		// Пытаемся объединить дыры
		for (char i = 0; i < 8; ++i)
		{
			point sidep = curpix + poss[i % 8];

			if (isContain(sidep))
			{
				Hole* h_t = getHole(sidep);
				//получена дыра
				if (h_t == hr)
					continue;

				// Пытается соединить полноценные дыры
				if (!h_t->isValid)
					continue;

				if (h_t->tryAdd(curpix))
				{
					h_t = tryAttach(hr, h_t, curpix);
				}
			}
		}
	}

	return hr->isValid;
}
//********************************************************************************



bc::indexCov* sortPixelsByRadius(const bc::DatagridProvider* workingImg, bc::ProcType type, float maxRadius, size_t& toProcess)
{
	int wid = workingImg->wid();
	int hei = workingImg->hei();

	int totalSize = 4 * static_cast<size_t>(wid) * hei + wid + hei;
	float dist;
	bc::indexCov* data = new bc::indexCov[totalSize];
	// Сичтаем расстояние между всеми соседними пикселями для каждого пикселя.
	// Чтобы не считать повтороно, от текущего проверяем только уникальные - в форме отражённой по вертикали буквы "L"

	int k = 0;
	for (int h = 0; h < hei - 1; ++h)
	{
		for (int w = 0; w < wid - 1; ++w)
		{
			int offset = wid * h + w;
			Barscalar cur = workingImg->get(w, h);
			Barscalar next;

			// rigth
			// c n
			// 0 0
			next = workingImg->get(w + 1, h);
			dist = cur.val_distance(next);

			data[k++] = indexCov(offset, dist, middleRight);

			// bottom
			// c 0
			// n 0
			next = workingImg->get(w, h + 1);
			dist = cur.val_distance(next);

			data[k++] = indexCov(offset, dist, bottomCenter);

			// bottom rigth
			// c 0
			// 0 n
			next = workingImg->get(w + 1, h + 1);
			dist = cur.val_distance(next);

			data[k++] = indexCov(offset, dist, bottomRight);


			// 0 c
			// n 0
			cur = workingImg->get(w + 1, h);
			next = workingImg->get(w, h + 1);
			dist = cur.val_distance(next);
			offset = wid * h + w + 1;

			data[k++] = indexCov(offset, dist, bottomLeft);
		}
	}

	int wd = wid - 1;
	for (int h = 0; h < hei - 1; ++h)
	{
		int offset = wid * h + wd;
		Barscalar cur = workingImg->get(wd, h);
		Barscalar next;
		next = workingImg->get(wd, h + 1);
		dist = cur.val_distance(next);
		data[k++] = indexCov(offset, dist, bottomCenter);
	}

	int hd = hei - 1;
	for (int w = 0; w < wid - 1; ++w)
	{
		int offset = wid * hd + w;
		Barscalar cur = workingImg->get(w, hd);
		Barscalar next;
		next = workingImg->get(w + 1, hd);
		dist = cur.val_distance(next);
		data[k++] = indexCov(offset, dist, middleRight);
	}

	assert(k < totalSize);

	// Тип не имеет занчение потому что соединяем не по яркости
	toProcess = k;
	std::sort(data, data + toProcess, [](const indexCov& a, const indexCov& b) {
		return a.dist < b.dist;
		});

	return data;
}


bc::indexCov* sortPixelsByRadius4(const bc::DatagridProvider* workingImg, bc::ProcType type, float maxRadius, size_t& toProcess)
{
	int wid = workingImg->wid();
	int hei = workingImg->hei();

	int totalSize = 2 * static_cast<size_t>(wid) * hei;
	float dist;
	bc::indexCov* data = new bc::indexCov[totalSize];
	// Сичтаем расстояние между всеми соседними пикселями для каждого пикселя.
	// Чтобы не считать повтороно, от текущего проверяем только уникальные - в форме отражённой по вертикали буквы "L"

	int k = 0;
	for (int h = 0; h < hei - 1; ++h)
	{
		for (int w = 0; w < wid - 1; ++w)
		{
			int offset = wid * h + w;
			Barscalar cur = workingImg->get(w, h);
			Barscalar next;

			// rigth
			// c n
			// 0 0
			next = workingImg->get(w + 1, h);
			dist = cur.val_distance(next);

			data[k++] = indexCov(offset, dist, middleRight);

			// bottom
			// c 0
			// n 0
			next = workingImg->get(w, h + 1);
			dist = cur.val_distance(next);

			data[k++] = indexCov(offset, dist, bottomCenter);
		}
	}

	int wd = wid - 1;
	for (int h = 0; h < hei - 1; ++h)
	{
		int offset = wid * h + wd;
		Barscalar cur = workingImg->get(wd, h);
		Barscalar next;
		next = workingImg->get(wd, h + 1);
		dist = cur.val_distance(next);
		data[k++] = indexCov(offset, dist, bottomCenter);
	}

	int hd = hei - 1;
	for (int w = 0; w < wid - 1; ++w)
	{
		int offset = wid * hd + w;
		Barscalar cur = workingImg->get(w, hd);
		Barscalar next;
		next = workingImg->get(w + 1, hd);
		dist = cur.val_distance(next);
		data[k++] = indexCov(offset, dist, middleRight);
	}
	assert(k < totalSize);

	// Тип не имеет занчение потому что соединяем не по яркости
	toProcess = k;
	std::sort(data, data + toProcess, [](const indexCov& a, const indexCov& b) {
		return a.dist < b.dist;
		});

	return data;
}


int BarcodeCreator::sortOrtoPixels(bc::ProcType type, int rtoe, int off, int offDop) //
{
	int ret = 0;
	uint hist[256];//256
	uint offs[256];//256
	std::fill_n(hist, 256, 0);
	std::fill_n(offs, 256, 0);

	int offB = off;

	switch (rtoe)
	{
	case 0:
		for (int j = 0; j < workingImg->hei(); ++j)//hei
		{
			//assert(drawimg.get(off, j).type == BarType::NONE);
//			drawimg.set(off, j, Barscalar(255));
			auto p = (int)workingImg->get(off, j);
			++hist[p]; //можно vector, но хз
			++ret;
		}
		break;
	case 1:
		for (int i = 0; i < workingImg->wid(); ++i)//wid
		{
			//assert(drawimg.get(i, off).type == BarType::NONE);
//			drawimg.set(i, off, Barscalar(255));
			auto p = (int)workingImg->get(i, off);
			++hist[p];//можно vector, но хз
			++ret;
		}
		break;
	case 2:
		for (int j = offDop; j < workingImg->hei() && off < workingImg->wid(); ++j)//hei
		{
			//assert(drawimg.get(off, j).type == BarType::NONE);
//			drawimg.set(off, j, Barscalar(255));
			auto p = (int)workingImg->get(off++, j);
			++hist[p];//можно vector, но хз
			++ret;
		}
		break;
	case 3:
		for (int j = offDop; j < workingImg->hei() && off >= 0; ++j)//hei
		{
			//assert(drawimg.get(off, j).type == BarType::NONE);
//			drawimg.set(off, j, Barscalar(255));
			auto p = (int)workingImg->get(off--, j);
			++hist[p];//можно vector, но хз
			++ret;
		}
		break;
	default:
		break;
	}

	off = offB;
	for (size_t i = 1; i < 256; ++i)
	{
		hist[i] += hist[i - 1];
		offs[i] = hist[i - 1];
	}


	poidex* data = new poidex[ret + 1];
	std::fill_n(data, ret + 1, poidex(UINT_MAX));

	switch (rtoe)
	{
	case 0:
		for (int j = 0; j < workingImg->hei(); ++j)//hei
		{
			auto p = (int)workingImg->get(off, j);
			assert(offs[p] < ret);
			data[offs[p]++] = GETOFF(off, j);
		}
		break;
	case 1:
		for (int i = 0; i < workingImg->wid(); ++i)//wid
		{
			auto p = (int)workingImg->get(i, off);
			assert(offs[p] < ret);
			data[offs[p]++] = GETOFF(i, off);
		}
		break;
	case 2:
		for (int j = offDop; j < workingImg->hei() && off < workingImg->wid(); ++j)//hei
		{
			auto p = (int)workingImg->get(off, j);
			assert(offs[p] < ret);
			data[offs[p]++] = GETOFF(off++, j);
		}
		break;
	case 3:
		for (int j = offDop; j < workingImg->hei() && off >= 0; ++j)//hei
		{
			auto p = (int)workingImg->get(off, j);
			assert(offs[p] < ret);
			data[offs[p]++] = GETOFF(off--, j);
		}
		break;
	default:
		break;
	}

	this->sortedArr = data;
	return ret;
}

inline void BarcodeCreator::sortPixels(bc::ProcType type)
{
	switch (workingImg->getType())
	{
	case BarType::BYTE8_1:
	case BarType::BYTE8_3:
	{
		uint hist[256];//256
		uint offs[256];//256
		std::fill_n(hist, 256, 0);
		std::fill_n(offs, 256, 0);

		for (int j = 0; j < workingImg->hei(); ++j)//hei
		{
			for (int i = 0; i < workingImg->wid(); ++i)//wid
			{
				auto p = (int)workingImg->get(i, j);
				++hist[p];//можно vector, но хз
			}
		}

		for (size_t i = 1; i < 256; ++i)
		{
			hist[i] += hist[i - 1];
			offs[i] = hist[i - 1];
		}


		poidex* data = new poidex[totalSize + 1];//256
		for (size_t i = 0; i < totalSize; i++)
		{
			uchar p = workingImg->getLiner(i).getAvgUchar();
			data[offs[p]++] = i;
		}

		if (type == ProcType::f255t0)
		{
			std::reverse(data, data + totalSize);
		}
		this->sortedArr = data;
		break;
	}
	//	{
	//		assert(false);
	//		break;
	//	}
		//case BarType::FLOAT:
		//{
			//// do this hack to skip constructor calling for every point
	//poidex* data = new poidex[totalSize + 1];//256

	////point * data = new point[total];

	//for (size_t i = 0; i < totalSize; ++i)//wid
	//	data[i] = i;

	//myclassFromMin cmp;
	//cmp.workingImg = workingImg;
	//std::sort(data, &data[totalSize], cmp);
	//if (type == ProcType::f255t0)
	//{
	//	for (size_t i = 1; i < totalSize - 1; ++i)//wid
	//	{
	//		float v0 = static_cast<float>(workingImg->getLiner(data[i - 1]));
	//		float v2 = static_cast<float>(workingImg->getLiner(data[i]));
	//		assert(v0 >= v2);
	//	}
	//}

	//this->sortedArr = data;
		//	break;
		//}

	default:
		assert(false);
		break;
	}
}

struct myclassFromMin {
	const bc::DatagridProvider* workingImg;
	bool operator() (poidex& a, poidex& b)
	{
		return workingImg->getLiner(a) < workingImg->getLiner(b);
	}
};


struct myclassFromMax {
	const bc::DatagridProvider* workingImg;
	bool operator() (poidex& a, poidex& b)
	{
		return workingImg->getLiner(a) > workingImg->getLiner(b);
	}
};


//
//inline point* BarcodeCreator::sort()
//{
//	std::map<T, int> hist;
//	barmap<T, int> offs;

//	for (int i = 0; i < workingImg->wid(); ++i)//wid
//	{
//		for (int j = 0; j < workingImg->hei(); ++j)//hei
//		{
//			T& p = workingImg->get(i, j);
//			if (hist.find(p) != hist.end())
//			{
//				++hist[p];
//			}
//			else
//				hist.insert(std::pair<T, int>(p, 1));
//		}
//	}

//	Barscalar prev;
//	bool f = false;
//	//auto const& [key, val]
//	for (auto const& iter : hist)
//	{
//		auto key = iter.first;
//		if (!f)
//		{
//			prev = key;
//			f = true;
//			continue;
//		}
//		hist[key] += hist[prev];
//		offs[key] = hist[prev];
//		prev = key;
//	}
//	hist.clear();

//	size_t total = workingImg->length();

//	point* data = new point[total];//256
//	for (int i = 0; i < workingImg->wid(); ++i)//wid
//	{
//		for (int j = 0; j < workingImg->hei(); ++j)//hei
//		{
//            Barscalar p = workingImg->get(i, j);
//			data[offs[p]++] = point(i, j);
//		}
//	}
//	return data;
//}



void BarcodeCreator::init(const bc::DatagridProvider* src, ProcType& type, ComponentType& comp)
{
	wid = src->wid();
	hei = src->hei();

	needDelImg = false;

	if (type == ProcType::invertf0)
	{
		Barscalar mmin = 0, mmax = 0;
		src->maxAndMin(mmin, mmax);
		bc::BarImg* newone = new BarImg(src->wid(), src->hei());
		for (size_t i = 0; i < src->length(); ++i)
		{
			newone->setLiner(i, mmax - src->getLiner(i));
		}

		if (!originalImg)
		{
			delete src;
			src = nullptr;
		}

		originalImg = false;
		type = ProcType::f0t255;
		setWorkingImg(newone);
	}
	else
		setWorkingImg(src);

	totalSize = workingImg->length();

	//от 255 до 0
	switch (type)
	{
	case ProcType::Radius:
		geometrySortedArr.reset(sortPixelsByRadius(workingImg, type, settings.maxRadius, this->processCount));
		sortedArr = nullptr;
		break;
	case ProcType::StepRadius:
		geometrySortedArr.reset(sortPixelsByRadius4(workingImg, type, settings.maxRadius, this->processCount));
		sortedArr = nullptr;
		break;
	case ProcType::ValueRadius:
		break;
	case ProcType::f0t255:
	case ProcType::f255t0:
	case ProcType::invertf0:
	case ProcType::experiment:
		sortPixels(type);
		break;
	default:
		assert(false);
	}
	// lastB = 0;

	included = new Include[totalSize];
	memset(included, NULL, totalSize * sizeof(Include));

#ifdef USE_OPENCV
	if (colors.size() == 0 && settings.visualize)
	{
		for (int b = 0; b < 255; b += 20)
			for (int g = 255; g > 20; g -= 20)
				for (int r = 0; r < 255; r += 100)
					colors.push_back(cv::Vec3b(b, g, r));
	}
#endif // USE_OPENCV
}
//#include <QDebug>


void BarcodeCreator::processHole(Barcontainer* item)
{
	for (curIndexInSortedArr = 0; curIndexInSortedArr < totalSize; ++curIndexInSortedArr)
	{
		curpix = getPoint(sortedArr[curIndexInSortedArr]);
		curbright = workingImg->get(curpix);

		/*	if (i == 25)
				qDebug() << "";*/
#ifdef VDEBUG
		VISUAL_DEBUG();
#else
		checkCloserB1();
#endif
	}
	// assert(((void)"ALARM! B1 is not zero", lastB == 1));

	addItemToCont(item);
	clearIncluded();
	// lastB = 0;
}


void BarcodeCreator::processComp(Barcontainer* item)
{
	for (curIndexInSortedArr = 0; curIndexInSortedArr < totalSize; ++curIndexInSortedArr)
	{
		curpoindex = sortedArr[curIndexInSortedArr];
		//if (curpoindex == UINT_MAX)
		//	continue;

		curpix = getPoint(curpoindex);
		assert(curpoindex == wid * curpix.y + curpix.x);

		curbright = workingImg->get(curpix.x, curpix.y);

#ifdef VDEBUG
		VISUAL_DEBUG_COMP();
#else
		checkCloserB0();

#endif
		assert(included[wid * curpix.y + curpix.x]);
	}

	//assert(((void)"ALARM! B0 is not one", lastB == 1));
	if (item)
	{
		addItemToCont(item);
		clearIncluded();
	}
	// lastB = 0;
}

// Parallel execution with function object.
struct Operator
{
	void operator()(short& pixel, const int* /*position*/) const
	{
		// Perform a simple threshold operation
		assert(pixel != 256);
		if (pixel == -1)
			pixel = 0;
		else
			pixel = 256 - pixel;
	}
};


void BarcodeCreator::addItemToCont(Barcontainer* container)
{
	if (container != nullptr)
	{
		Baritem* lines = new Baritem(workingImg->wid(), type);

		switch (settings.returnType)
		{
		case ReturnType::barcode2d:
			computeNdBarcode(lines, 2);
			break;
		case ReturnType::barcode3d:
		case ReturnType::barcode3dold:
			computeNdBarcode(lines, 3);
			break;
		default:
			assert(false);
		}
		container->addItem(lines);
	}
}


void BarcodeCreator::VISUAL_DEBUG()
{
	checkCloserB1();
#ifdef USE_OPENCV
	if (settings.visualize)
	{
		draw("main");
		cv::waitKey(0);
	}
#endif // DEBUG
}


void BarcodeCreator::VISUAL_DEBUG_COMP()
{
	checkCloserB0();
#ifdef USE_OPENCV
	if (settings.visualize)
	{
		draw("main");
		cv::waitKey(settings.waitK);
	}
#endif // DEBUG
}


void BarcodeCreator::clearIncluded()
{
	for (COMPP c : components)
	{
		//assert(c != nullptr);
		if (c != nullptr)
			delete c;
	}
	components.clear();

	if (included != nullptr)
	{
		//memset(included, 0, totalSize * sizeof(Include));
		delete[] included;
		included = nullptr;
	}


	if (needDelImg)
	{
		delete workingImg;
	}
	workingImg = nullptr;

	geometrySortedArr.reset(nullptr);
	if (sortedArr != nullptr)
	{
		delete[] sortedArr;
		sortedArr = nullptr;
	}
}



bool compareLines(const barline* i1, const barline* i2)
{
	if (i1->len() == i2->len())
		return i1->start > i2->start;

	return (i1->len() > i2->len());
}



void BarcodeCreator::computeNdBarcode(Baritem* lines, int n)
{
	assert(n == 2 || n == 3);

	// якорная линия
	BarRoot* rootNode = nullptr;
	if (settings.createGraph)
	{
		rootNode = new BarRoot(0, 0, workingImg->wid(), nullptr, 0);
		lines->setRootNode(rootNode);
	}

	for (COMPP c : components)
	{
		if (c == nullptr || c->resline == nullptr)
			continue;

		if (c->parent == nullptr)
		{
			//assert(c->isAlive() || settings.killOnMaxLen);
			c->kill(curbright);
			if (settings.createGraph)
				c->resline->setparent(rootNode);
		}
		assert(!c->isAlive());
		lines->add(c->resline);
	}
}


void BarcodeCreator::processTypeF(barstruct& str, const bc::DatagridProvider* src, Barcontainer* item)
{
	init(src, str.proctype, str.comtype);

	switch (str.comtype)
	{
	case  ComponentType::Component:
	{
		switch (str.proctype)
		{
		case ProcType::Radius:
			processCompByRadius(item);
			break;
		case ProcType::experiment:
		{
			//			cv::namedWindow("drawimg", cv::WINDOW_NORMAL);

			//			drawimg = BarImg(workingImg->wid(), workingImg->hei(), 1);
			totalSize = sortOrtoPixels(str.proctype, 2, 0); // go down-right
			processComp();
			delete[] sortedArr;
			sortedArr = nullptr;

			int swid = workingImg->wid();
			int hwid = swid / 2 + swid % 2;
			for (int i = 0; i < swid; ++i)//wid Идям по диагонали
			{
				totalSize = sortOrtoPixels(str.proctype, 0, i); // go down
				processComp();
				delete[] sortedArr;
				sortedArr = nullptr;

				if (i < workingImg->hei())
				{
					totalSize = sortOrtoPixels(str.proctype, 1, i); // go rigth
					processComp();
					delete[] sortedArr;
					sortedArr = nullptr;
				}

				//sortOrtoPixels(str.proctype, 2, i); // go down-right
				//processComp(item);
				if (i < hwid)
				{
					totalSize = sortOrtoPixels(str.proctype, 3, i * 2);// go down-left
					processComp();
					delete[] sortedArr;
					sortedArr = nullptr;
				}
				else
				{
					totalSize = sortOrtoPixels(str.proctype, 3, swid - 1, i - hwid);// go down-left
					processComp();
					delete[] sortedArr;
					sortedArr = nullptr;
				}

				// Нинии паралелные остновной, ихсодят в разыне стороны (одна по ширине, другая по высоте)

				totalSize = sortOrtoPixels(str.proctype, 2, i);// go down-right
				processComp();
				delete[] sortedArr;
				sortedArr = nullptr;

				totalSize = sortOrtoPixels(str.proctype, 2, 0, i);// go down-right
				processComp();
				delete[] sortedArr;
				sortedArr = nullptr;

				//				cv::imshow("drawimg", bc::convertProvider2Mat(&drawimg));
				//				cv::waitKey(0);
			}

			addItemToCont(item);
			clearIncluded();

			//			cv::waitKey(0);
		}
		break;
		case ProcType::StepRadius:
			processCompByStepRadius(item);
			break;
		case ProcType::ValueRadius:
			processByValueRadius(item);
			break;
		default:
			processComp(item);
			break;
		}
		break;
	}
	case  ComponentType::Hole:
		if (str.proctype == ProcType::Radius)
			processCompByRadius(item);
		else
			processHole(item);
		break;
		//case  ComponentType::FullPrepair:
		//	ProcessFullPrepair(b, item);
		//	break;
		//case  ComponentType::PrepairComp:
		//	ProcessPrepComp(b, item);
		//	break;
	default:
		break;
	}

}


void BarcodeCreator::processFULL(barstruct& str, const bc::DatagridProvider* img, Barcontainer* item)
{
	bool rgb = (img->channels() != 1);


	//	if (str.comtype == ComponentType::Component && rgb)
	//	{
	//		BarImg* res = new BarImg();
	//		cvtColorV3B2U1C(*img, *res);
	//		originalImg = false;
	//		needDelImg = true;
	//		type = BarType::BYTE8_1;
	//		processTypeF(str, res, item);
	//		return;
	//	}

	switch (str.coltype)
	{
		// To RGB
	case ColorType::rgb:
	{
		if (img->type == BarType::BYTE8_3)
			break;

		BarImg* res = new BarImg(-1, -1, 3);
		cvtColorU1C2V3B(*img, *res);
		originalImg = false;
		needDelImg = true;
		type = BarType::BYTE8_3;
		processTypeF(str, res, item);
		return;
	}
	case ColorType::gray:
	{
		if (img->type == BarType::BYTE8_1)
			break;

		BarImg* res = new BarImg();
		cvtColorV3B2U1C(*img, *res);
		originalImg = false;
		needDelImg = true;
		type = BarType::BYTE8_1;
		processTypeF(str, res, item);
		return;
	}
	default:
		break;
	}

	type = img->getType();
	originalImg = true;
	needDelImg = false;
	processTypeF(str, img, item);
}


bc::Barcontainer* BarcodeCreator::createBarcode(const bc::DatagridProvider* img, const BarConstructor& structure)
{
	this->settings = structure;
	this->settings.createBinaryMasks = true;

	settings.checkCorrect();
	Barcontainer* cont = new Barcontainer();

	for (auto& it : settings.structure)
	{
		processFULL(it, img, cont);
	}
	return cont;
}

// ***************************************************


uchar dif(uchar a, uchar b)
{
	if (a > b)
		return a - b;
	else
		return b - a;
}



//Barcontainer* BarcodeCreator::searchHoles(float* /*img*/, int /*wid*/, int /*hei*/, float/* nullVal*/)
//{
//	return nullptr;
//}

#ifdef _PYD

bc::Barcontainer* bc::BarcodeCreator::createBarcode(bn::ndarray& img, bc::BarConstructor& structure)
{
	//auto shape = img.get_shape();

	/*	int type = cv_8uc1;
	if (img.get_nd() == 3 && img.shape[2] == 3)
	image = &barimg<bcvec3b>(shape[0], shape[1], img.get_data());
	else if float
	barimg image(shape[0], shape[1], img.get_data());*/

	//cv::imshow("test", image);
	//cv::waitkey(0);
	bc::BarNdarray image(img);
	//try
	//{
	return createBarcode(&image, structure);
	//}
	//catch (const std::exception& ex)
	//{
	//printf("ERROR");
	//printf(ex.what());
	//}

	//bc::BarImg image(img.shape(1), img.shape(0), img.get_nd(), (uchar*)img.get_data(), false, false);
	//return createBarcode(&image, structure);
}
#endif
// GEOMERTY



void BarcodeCreator::processCompByRadius(Barcontainer* item)
{
	for (curIndexInSortedArr = 0; curIndexInSortedArr < processCount; ++curIndexInSortedArr)
	{
		const indexCov& val = geometrySortedArr.get()[curIndexInSortedArr];
		if (val.dist > settings.maxRadius)
		{
			break;
		}

		processRadar(val, true);

#ifdef USE_OPENCV
		if (settings.visualize)
		{
			const BarMat* mat = reinterpret_cast<const BarMat*>(this->workingImg);

			if (mat)
			{
				Mat wimg;
				bool is3d = this->type == BarType::BYTE8_3;
				if (!is3d)
					cv::cvtColor(mat->mat, wimg, cv::COLOR_GRAY2BGR);
				else
					mat->mat.copyTo(wimg);

				int size = colors.size();

				for (size_t i = 0; i < totalSize; i++)
				{
					COMPP comp = getInclude(i);
					if (comp == nullptr || !comp->isAlive())
						continue;
					int x = static_cast<int>(i % wid);
					int y = static_cast<int>(i / wid);
					int tic = 1;
					int marc = cv::MARKER_TILTED_CROSS;

					cv::Point p(x, y);
					cv::Vec3b col = colors[(size_t)comp->startIndex % size];

					wimg.at<cv::Vec3b>(y, x) = col;
				}
				//wimg.at<cv::Vec3b>(curpix.y, curpix.x) = cv::Vec3b(255, 0, 0);
				//wimg.at<cv::Vec3b>(NextPoint.y, NextPoint.x) = cv::Vec3b(0,0,255);

				cv::namedWindow("img", cv::WINDOW_NORMAL);
				cv::imshow("img", wimg);
			}
			else
			{
				draw("radius");
			}
			cv::waitKey(settings.waitK);
		}
#endif // USE_OPENCV
	}


	//totalSize = workingImg->length();
	//bc::ProcType type = ProcType::f0t255;
	//sortPixels(type);
	//processComp(item);

	//assert(((void)"ALARM! B0 is not one", lastB == 1));
	addItemToCont(item);
	clearIncluded();
}


//template bc::Barcontainer<ushort>* BarcodeCreator<ushort>::createBarcode(const bc::DatagridProvider<ushort>*, const BarConstructor<ushort>&)



void BarcodeCreator::processCompByStepRadius(Barcontainer* item)
{
	Barscalar samax, sadym;
	workingImg->maxAndMin(sadym, samax);
	int smax = (int)samax.getAvgUchar();
	int foundSt = 0;
	settings.maxRadius = 0.f;
	for (float& i = settings.maxRadius; i <= smax; ++i)
	{
		curIndexInSortedArr = foundSt;
		foundSt = 0;
		for (; curIndexInSortedArr < processCount; ++curIndexInSortedArr)
		{
			const indexCov& val = geometrySortedArr.get()[curIndexInSortedArr];

			if (val.dist > settings.maxRadius && foundSt == 0)
				foundSt = curIndexInSortedArr;

			processRadar(val, foundSt == 0);
		}
	}
	addItemToCont(item);
	clearIncluded();
}


void BarcodeCreator::processRadar(const indexCov& val, bool allowAttach)
{
	curpoindex = val.offset;
	curpix = getPoint(curpoindex);

	bc::point NextPoint = val.getNextPoint(curpix);
	poidex NextPindex = NextPoint.getLiner(workingImg->wid());

	Component* first = getComp(curpoindex);
	Component* connected = getComp(NextPindex);

	if (first != nullptr)
	{
		Barscalar Nscalar = workingImg->get(NextPoint.x, NextPoint.y);
		//curpoindex = NextPindex;
		//curpix = NextPoint;

		//если в найденном уже есть этот элемент
		//существует ли ребро вокруг
		if (connected != nullptr && first != connected)
		{
			if (allowAttach)
				attach(first, connected);//проверить, чему равен included[point(x, y)] Не должно, ибо first заменяется на connect
		}
		else if (connected == nullptr)
		{
			if (!first->add(NextPindex, NextPoint, Nscalar))
			{
				connected = new Component(NextPindex, Nscalar, this);
			}
		}
	}
	else
	{
		curbright = workingImg->get(curpix.x, curpix.y);
		// Ребро не создано или не получилось присоединить
		if (connected == nullptr)
		{
			first = new Component(curpoindex, curbright, this);

			Barscalar Nscalar = workingImg->get(NextPoint.x, NextPoint.y);
			//curpoindex = NextPindex;
			//curpix = NextPoint;

			if (!first->add(NextPindex, NextPoint, Nscalar))
			{
				connected = new Component(NextPindex, Nscalar, this);
			}
		}
		else if (!connected->add(curpoindex, curpix, curbright))
		{
			first = new Component(curpoindex, curbright, this);
		}
	}
}

void BarcodeCreator::processByValueRadius(Barcontainer* item)
{
	Barscalar samax, sadym;
	workingImg->maxAndMin(sadym, samax);
	int smax = (int)samax.getAvgUchar();

	settings.maxRadius = 0.f;
	BarImg img(workingImg->wid(), workingImg->hei(), workingImg->channels());
	for (size_t i = 0; i < workingImg->length(); i++)
		img.setLiner(i, workingImg->getLiner(i));

	for (float& i = settings.maxRadius; i <= smax; ++i)
	{
		for (auto* comp : components)
		{
			if (comp->resline == nullptr)
				continue;

			float avg[3]{ 0, 0, 0 };
			auto& matr = comp->resline->matr;
			size_t msize = matr.size();
			for (size_t i = 0; i < msize; i++)
			{
				auto sc = workingImg->get(matr[i].getPoint());
				avg[0] += sc.data.b3[0];
				avg[1] += sc.data.b3[1];
				avg[2] += sc.data.b3[2];
			}

			avg[0] /= msize;
			avg[1] /= msize;
			avg[2] /= msize;

			Barscalar avgSCl(avg[0], avg[1], avg[2]);

			for (size_t i = 0; i < msize; i++)
			{
				img.set(matr[i].getPoint(), avgSCl);
			}
		}

		geometrySortedArr.reset(sortPixelsByRadius4(&img, ProcType::ValueRadius, settings.maxRadius, processCount));
		for (curIndexInSortedArr = 0; curIndexInSortedArr < processCount; ++curIndexInSortedArr)
		{
			const indexCov& val = geometrySortedArr.get()[curIndexInSortedArr];

			if (val.dist > settings.maxRadius)
				break;

			processRadar(val, true);
		}

		if (curIndexInSortedArr == processCount)
			break;
	}

	addItemToCont(item);
	clearIncluded();
}


// Radius

bc::Barcontainer* CloudPointsBarcode::createBarcode(const CloudPoints* points)
{
	Barcontainer* cont = new Barcontainer();
	if (points->points.size() > 0)
		processFULL(points, cont);
	return cont;
}

void CloudPointsBarcode::processFULL(const CloudPoints* points, Barcontainer* item)
{
	processTypeF(points, item);
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
	size_t wid = cloud->points.size();

	// n(n+1)/2
	totalSize = wid * (wid + 1) / 2 - wid;

	//	if (totalSize > 500000)
	//		throw std::exception();

	int s = sizeof(PointIndexCov);
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

			dist = cloud->points[i].distanse(cloud->points[j]);
			data[k++] = PointIndexCov(i, j, dist);
		}
	}
	assert(k == totalSize);

	// Тип не имеет занчение потому что соединяем не по яркости
	std::sort(data, data + k, [](const PointIndexCov& a, const PointIndexCov& b) {
		return a.dist < b.dist;
		});

	if (useHolde)
	{
		std::vector<PointIndexCov> newVals;
		newVals.push_back(data[0]);

		for (size_t i = 1; i < totalSize; i++)
		{
			const PointIndexCov& a = data[i];
			Point3D a1 = cloud->points[a.points[0]];
			Point3D a2 = cloud->points[a.points[1]];
			bool f = true;
			if (newVals.size() == 50)
				f = true;
			for (size_t j = 0; j < newVals.size(); j++)
			{
				const PointIndexCov& b = newVals[j];
				Point3D b1 = cloud->points[b.points[0]];
				Point3D b2 = cloud->points[b.points[1]];
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
}

void bc::CloudPointsBarcode::sortTriangulate()
{
}


void CloudPointsBarcode::process(Barcontainer* item)
{
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

	addItemToCont(item);
	clearIncluded();
}


void CloudPointsBarcode::processComp(const  CloudPointsBarcode::PointIndexCov& val)
{
	const int curIndex = val.points[0];
	CloudPoint curCloudPoint = cloud->points[curIndex];
	bc::point curPoint(curCloudPoint.x, curCloudPoint.y);
	poidex curPoindex = curPoint.getLiner(MAX_WID);

	int nextIndex = val.points[1];
	CloudPoint nextCloudPoint = cloud->points[nextIndex];
	bc::point nextPoint(nextCloudPoint.x, nextCloudPoint.y);
	poidex nextPoindex = nextPoint.getLiner(MAX_WID);

	barline* first = getComp(curPoindex);
	barline* connected = getComp(nextPoindex);

	if (first != nullptr)
	{
		assert(first->m_end == 0);
		//если в найденном уже есть этот элемент
		//существует ли ребро вокруг
		if (connected != nullptr && first != connected)
		{
			assert(connected->m_end == 0);

			barline* main = first;
			barline* sub = connected;
			// Attach
			if (first->start > connected->start)
			{
				main = connected;
				sub = first;
			}

			for (size_t i = 0; i < sub->matr.size(); i++)
			{
				auto& pas = sub->matr[i];
				bc::point p = pas.getPoint();
				main->addCoord(p, pas.value);
				setInclude(p.getLiner(MAX_WID), main);
			}

			sub->setparent(main);
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
			connected = new barline(Barscalar(val.dist, BarType::FLOAT32_1), 0, 0);
			components.push_back(connected);
		}
		assert(connected->m_end == 0);

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

	HoleInfo(Barscalar start)
	{
		line = new barline(start, 0, 0);
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


std::function<void(const point&, const point&, bool)> CloudPointsBarcode::drawLine;
std::function<void(bc::PloyPoints&, bool)> CloudPointsBarcode::drawPlygon;

struct Graph
{
	barmap<poidex, GraphPoint> graphPoints;

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
		float ang = std::atan2f(_det, _dot);// atan2(y, x) or atan2(sin, cos)
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
			passedPoints.insert(startPoint->point->p.getLiner(MAX_WID));
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

				const poidex pind = curVec.getLiner(MAX_WID);
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


	bool findHole(poidex p1, poidex p2, float dist, bc::CloudPointsBarcode::ComponentsVector& vect, bc::CloudPointsBarcode::ComponentsVector& instakilled)
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

		std::shared_ptr<HoleInfo> main = std::make_shared<HoleInfo>(dist);

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

		if (CloudPointsBarcode::drawPlygon)
			CloudPointsBarcode::drawPlygon(pew, true);

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
	Graph grath;

	ComponentsVector instakilled;

	for (curIndexInSortedArr = 0; curIndexInSortedArr < totalSize; ++curIndexInSortedArr)
	{
		const PointIndexCov& val = sortedArr.get()[curIndexInSortedArr];

		//const uint curIndex = val.points[0];
		CloudPoint curCloudPoint = cloud->points[val.points[0]];
		bc::point curPoint(curCloudPoint.x, curCloudPoint.y);
		poidex curPoindex = curPoint.getLiner(MAX_WID);

		//int unextIndex = val.points[1];
		CloudPoint nextCloudPoint = cloud->points[val.points[1]];
		bc::point nextPoint(nextCloudPoint.x, nextCloudPoint.y);
		poidex nextPoindex = nextPoint.getLiner(MAX_WID);

		GraphPoint* first = grath.getGrath(curPoindex);
		GraphPoint* connected = grath.getGrath(nextPoindex);

		if (drawLine)
		{
			drawLine(nextPoint, curPoint, false);
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
						if (drawLine)
							drawLine(curPoint, nextPoint, true);
						//using namespace std::chrono_literals;
						//std::this_thread::sleep_for(1000ms);
						bool h = grath.findHole(curPoindex, nextPoindex, val.dist, components, instakilled);
						//endPoints();
						assert(h);
					}
				}
				else
				{
					//assert(connected->hole == nullptr);
					if (drawLine)
						drawLine(curPoint, nextPoint, true);
					//using namespace std::chrono_literals;
					//std::this_thread::sleep_for(1000ms);

					barline* fLines[2];
					fLines[0] = first->hole ? first->hole->line : nullptr;
					fLines[1] = connected->hole ? connected->hole->line : nullptr;
					bool h = grath.findHole(curPoindex, nextPoindex, val.dist, components, instakilled);
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

							while (p->parent)
							{
								p = p->parent;
							}

							if (p != newHole)
								p->setparent(newHole);
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

	for (size_t i = 0; i < instakilled.size(); i++)
	{
		delete instakilled[i];
	}
}


void CloudPointsBarcode::addItemToCont(Barcontainer* container)
{
	if (container != nullptr)
	{
		Baritem* lines = new Baritem(0, BarType::FLOAT32_1);
		lines->barlines = std::move(components);
		container->addItem(lines);
	}
}


void CloudPointsBarcode::clearIncluded()
{
	for (size_t i = 0; i < components.size(); ++i)
	{
		delete components[i];
	}

	components.clear();
	included.clear();
	sortedArr.reset(nullptr);
}


export
{
	BarType;
	Barscalar;
	//using ::uchar;
	//using ::uint;
}

export namespace StateBinFile
{
	BinState;
	BinStateReader;
	BinStateWriter;
}

export namespace bc
{
	const unsigned int BAR_MAX_WID = 65535;

	poidex;

	point;
	pointHash;
	pmap;

	CachedValue;
	BarConstructor;
	DatagridProvider;
	BarRect;
	barline;
	using bc::BarRoot;

	using bc::barlinevector;
	using bc::barvector;
	using bc::barcounter;
	using bc::PloyPoints;
	barvalue;
	CachedValue;

	barstruct;

	CompireStrategy;
	ComponentType;
	ProcType;
	ColorType;
	ReturnType;
	ReturnType;
	AttachMode;
	ProcessStrategy;

	BarRect;
	bc::BarRect getBarRect(const bc::barvector& matrix)
	{
		int l, r, t, d;
		r = l = matrix[0].getX();
		t = d = matrix[0].getY();
		for (size_t j = 0; j < matrix.size(); ++j)
		{
			if (l > matrix[j].getX())
				l = matrix[j].getX();
			if (r < matrix[j].getX())
				r = matrix[j].getX();

			if (t > matrix[j].getY())
				t = matrix[j].getY();
			if (d < matrix[j].getY())
				d = matrix[j].getY();
		}
		return bc::BarRect(l, t, r - l + 1, d - t + 1);
	}

	Barcontainer;
	BarcodeCreator;
	Baritem;
	CloudPointsBarcode;
}
