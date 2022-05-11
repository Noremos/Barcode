#pragma once
#include <vector>
#include "presets.h"
#include "barline.h"

namespace bc
{
	
	class EXPORT Barbase
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

	
	
	using barlinevector = std::vector<bc::barline*>;

	
	class EXPORT Baritem : public Barbase
	{
	public:
		barlinevector barlines;

	private:
		bc::BarRoot* rootNode = nullptr;
		int wid;
	public:
		Baritem(int wid = 0);

		//copy constr
		Baritem(Baritem const& obj) {
			this->rootNode = obj.rootNode;
			this->wid = obj.wid;

			for (auto* barval : obj.barlines)
			{
				this->barlines.push_back(barval->clone());
			}
		}

		// copy
		void operator=(Baritem const& obj)
		{
			this->rootNode = obj.rootNode;
			this->wid = obj.wid;

			for (auto* barval : obj.barlines)
			{
				this->barlines.push_back(barval->clone());
			}
		}

		// move constr
		Baritem(Baritem&& obj)
		{
			this->rootNode = std::exchange(obj.rootNode, nullptr);
			this->wid = obj.wid;

			this->barlines = obj.barlines;
			obj.barlines.clear();
		}

		// move assign
		void operator=(Baritem&& obj)
		{
			this->rootNode = std::exchange(obj.rootNode, nullptr);
			this->wid = obj.wid;

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

		void getBettyNumbers(int* bs);

		// remove lines than less then passed value
		void removePorog(Barscalar const porog);
		void preprocessBar(Barscalar const& porog, bool normalize);
		float compireFull(const Barbase* bc, bc::CompireStrategy strat) const;
		float compireBestRes(Baritem const* bc, bc::CompireStrategy strat) const;
		float compareOccurrence(Baritem const* bc, bc::CompireStrategy strat) const;
		//    void fullCompite(const barbase *bc, CompireFunction fn, float poroc = 0.5f);

		void normalize();
		void getJsonObejct(std::string &out);
		void getJsonLinesArray(std::string &out);
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
	
	class EXPORT Barcontainer : public Barbase
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
		Baritem* getItem(size_t i);


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

		//    void fullCompite(const barbase *bc, CompireFunction fn, float poroc = 0.5f);
		~Barcontainer();

		// Barbase interface
	};

}
