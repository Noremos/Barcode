#pragma once
#include <vector>
#include "presets.h"
#include "barline.h"

namespace bc
{
	template<class T>
	class EXPORT Barbase
	{
	public:
		virtual void removePorog(T const porog) = 0;
		virtual void preprocessBar(T const& porog, bool normalize) = 0;
		virtual float compireFull(const Barbase<T>* Y, bc::CompireStrategy strat) const = 0;
		virtual Barbase<T>* clone() const = 0;
		virtual T sum() const = 0;
		virtual void relen() = 0;
		//    virtual void fullCompite(barbase const *bc, CompireFunction fn, float poroc = 0.5f) = 0;
		virtual ~Barbase();
	};

	
	template<class T>
	using barlinevector = std::vector<bc::barline<T>*>;

	template<class T>
	class EXPORT Baritem : public Barbase<T>
	{
	public:
		barlinevector<T> barlines;

	private:
		bc::BarRoot<T>* rootNode = nullptr;

	public:
		Baritem();
		Baritem(const Baritem<T>& obj);
		//    cv::Mat binmap;
		void add(T st, T len);
		void add(barline<T>* line);

		T sum() const;
		void relen();
		T maxLen() const;
		Baritem<T>* clone() const;

		void getBettyNumbers(int* bs);

		// remove lines than less then passed value
		void removePorog(T const porog);
		void preprocessBar(T const& porog, bool normalize);
		float compireFull(const Barbase<T>* bc, bc::CompireStrategy strat) const;
		float compireBestRes(Baritem<T> const* bc, bc::CompireStrategy strat) const;
		float compareOccurrence(Baritem<T> const* bc, bc::CompireStrategy strat) const;
		//    void fullCompite(const barbase *bc, CompireFunction fn, float poroc = 0.5f);
		~Baritem();

		bc::BarRoot<T>* getRootNode()
		{
			return rootNode;
		}
		void setRootNode(bc::BarRoot<T>* root)
		{
			rootNode = root;
		}

		T getMax()
		{
			T max = 0;
			for (auto* b : this->barlines)
			{
				if (b->start + b->len > max)
					max = b->start + b->len;
			}
			return max;
		}

#ifdef _PYD
		// only for uchar
		bp::list calcHistByBarlen(/*T maxLen*/)
		{
			int maxLen = 256;
			int* hist = new int[maxLen];
			memset(hist, 0, maxLen * sizeof(int));

			for (size_t i = 0; i < barlines.size(); i++)
				++hist[(uchar)barlines[i]->len];

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
		bp::list calcHistByPointsSize(/*T maxLen*/)
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
		
		float cmp(const Baritem<T>* bitem, bc::CompireStrategy strat) const
		{
			return compireFull((const Baritem<T>*)bitem, strat);
		}


#endif // _PYD

		void sortByLen();
		void sortBySize();
	};

	//template<size_t N>
	template<class T>
	class EXPORT Barcontainer : public Barbase<T>
	{
		std::vector<Baritem<T>*> items;
	public:
		Barcontainer();

		T sum() const;
		void relen();
		Barbase<T>* clone() const;
		T maxLen() const;
		size_t count();
		//    Baritem *operator [](int i);
		Baritem<T>* getItem(size_t i);
		Baritem<T>* lastItem();
		void addItem(Baritem<T>* item);
		// remove lines than less then passed value
		void removePorog(T const porog);
		void preprocessBar(T const& porog, bool normalize);

		float compireFull(const Barbase<T>* bc, bc::CompireStrategy strat) const;

		//    void fullCompite(const barbase *bc, CompireFunction fn, float poroc = 0.5f);
		~Barcontainer();

		// Barbase interface
	};

}
