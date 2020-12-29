#ifndef BARCONTAINER_H
#define BARCONTAINER_H
#include "barcode.h"
#include "point.h"
namespace bc {

struct BarNode
{
	BarNode() {}
	BarNode(bc::bline *comp) { this->comp = comp; }
	bc::bline *comp = nullptr;
	BarNode *parent = nullptr;
	//bc::Component *comp;
	std::vector<BarNode *> childrens;
	void setParrent(BarNode *node)
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

class EXPORT Baritem:public Barbase
{
public:

    Baritem();
    Baritem(const Baritem &obj);
//    cv::Mat binmap;
    void add(uchar st, uchar len);
//    void add(uchar st, uchar len, cv::Mat binmat);
	void add(uchar st, uchar len, pmap *binmat);
	void add(bline *line);

    int sum() const;
    void relen();
    uchar maxLen() const;
    Baritem *clone() const;

    void removePorog(uchar const porog);
    void preprocessBar(int const &porog, bool normalize);
    float compireCTML(const Barbase *bc) const;
    float compireCTS(Barbase const *bc) const;
    //    void fullCompite(const barbase *bc, CompireFunction fn, float poroc = 0.5f);
	~Baritem();

	bc::BarNode* rootNode;

	uchar getMax()
	{
		uchar max = 0;
		for (auto &b : this->bar)
		{
			if (b->start + b->len > max)
				max = b->start + b->len;
		}
		return max;
	}

    std::vector<bc::bline*> bar;

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
class EXPORT Barcontainer : public Barbase
{
    std::vector<Baritem*> items;
public:
    Barcontainer();

    int sum() const;
    void relen();
    Barbase *clone() const;
    uchar maxLen() const;
    size_t count();
    //    Baritem *operator [](int i);
        Baritem *get(int i);
    Baritem *lastItem();
    void addItem(Baritem* item);
    void removePorog(uchar const porog);
    void preprocessBar(int const &porog, bool normalize);

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
