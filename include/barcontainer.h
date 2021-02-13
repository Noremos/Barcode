#ifndef BARCONTAINER_H
#define BARCONTAINER_H

#include "barcode.h"
#include "point.h"

namespace bc {

template<class T>
struct BarNode
{
	BarNode() {}
	BarNode(bc::bline<T> *comp) { this->comp = comp; }
	bc::bline<T> *comp = nullptr;
	BarNode<T> *parent = nullptr;
	//bc::Component *comp;
	std::vector<BarNode<T> *> childrens;
	void setParrent(BarNode<T> *node)
	{
		node->childrens.push_back(this);
		this->parent = node;
	}
	~BarNode()
	{
		for (int i = 0; i < childrens.size(); ++i)
		{
			delete childrens[i];
		}
	}
};

template<class T>
class EXPORT Baritem: public Barbase
{
public:

    Baritem();
    Baritem(const Baritem<T> &obj);
//    cv::Mat binmap;
    void add(T st, T len);
//    void add(uchar st, uchar len, cv::Mat binmat);
	void add(T st, T len, pmap<T> *binmat);
	void add(bline<T> *line);

    double sum() const;
    void relen();
    T maxLen() const;
    Baritem<T> *clone() const;


    // remove lines than less then passed value
    void removePorog(T const porog);
    void preprocessBar(int const &porog, bool normalize);
    float compireCTML(const Barbase *bc) const;
    float compireCTS(Barbase const *bc) const;
    //    void fullCompite(const barbase *bc, CompireFunction fn, float poroc = 0.5f);
	~Baritem();

	bc::BarNode<T>* rootNode;

	T getMax()
	{
		T max = 0;
		for (auto &b : this->bar)
		{
			if (b->start + b->len > max)
				max = b->start + b->len;
		}
		return max;
	}

    std::vector<bc::bline<T>*> bar;

#ifdef _PYD
    bp::list getBar()
    {
        bp::list lines;
        for (auto line : bar)
        {
            lines.append(line);
        }
        return lines;
    }
#endif // _PYD

};

//template<size_t N>
template<class T>
class EXPORT Barcontainer : public Barbase
{
    std::vector<Baritem<T>*> items;
public:
    Barcontainer();

    double sum() const;
    void relen();
    Barbase *clone() const;
    T maxLen() const;
    size_t count();
    //    Baritem *operator [](int i);
    Baritem<T> *get(int i);
    Baritem<T> *lastItem();
    void addItem(Baritem<T>* item);
    // remove lines than less then passed value
    void removePorog(T const porog);
    void preprocessBar(T const &porog, bool normalize);

    float compireCTML(const Barbase *bc) const;
    float compireCTS(Barbase const *bc) const;

#ifdef _PYD
    float cmpCTML(const Barcontainer* bc) const;
    float cmpCTS(Barcontainer const* bc) const;

#endif // _PYD


    //    void fullCompite(const barbase *bc, CompireFunction fn, float poroc = 0.5f);
    ~Barcontainer();

    // Barbase interface
};


}
#endif // BARCONTAINER_H
