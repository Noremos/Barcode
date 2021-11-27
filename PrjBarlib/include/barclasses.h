#ifndef BARCONTAINER_H
#define BARCONTAINER_H
#include "barstrucs.h"

namespace bc {


    class EXPORT Baritem :public Barbase
    {
    public:

        Baritem();
        Baritem(const Baritem& obj);
        //    cv::Mat binmap;
        void add(uchar st, uchar len);
        //    void add(uchar st, uchar len, cv::Mat binmat);
        void add(uchar st, uchar len, pmap* binmat);

        //    uchar type = 1;
        int sum() const;
        void relen();
        uchar maxLen() const;

        Baritem* clone() const;

        void removePorog(uchar const porog);
        void preprocessBar(int const& porog, bool normalize);

        float compireCTML(const Barbase* bc) const;
        float compireCTS(Barbase const* bc) const;
        //    void fullCompite(const barbase *bc, CompireFunction fn, float poroc = 0.5f);
        ~Baritem();

        std::vector<bc::bline*> bar;
    };

    //template<size_t N>
    class EXPORT Barcontainer : public Barbase
    {
        std::vector<Baritem*> items;
    public:
        Barcontainer();

        int sum() const;
        void relen();
        Barbase* clone() const;
        uchar maxLen() const;
        size_t count();
        //    Baritem *operator [](int i);
        Baritem* get(int i);
        Baritem* lastItem();
        void addItem(Baritem* item);
        void removePorog(uchar const porog);
        void preprocessBar(int const& porog, bool normalize);

        float compireCTML(const Barbase* bc) const;
        float compireBest(const Barbase* bc) const;
        float compireCTS(Barbase const* bc) const;


        Baritem* exractItem(size_t index)
        {
            if (index < items.size())
            {
                auto* item = items[index];
                items[index] = nullptr;
                return item;
            }
            return nullptr;
        }
        //    void fullCompite(const barbase *bc, CompireFunction fn, float poroc = 0.5f);
        ~Barcontainer();

        // Barbase interface
    };

    enum class ComponentType { Component, Hole };
    enum class ProcType { f0t255, f255t0 };
    enum class ColorType { gray, rgb, native };

    struct barstruct
    {
        ComponentType comtype;
        ProcType proctype;
        ColorType coltype;
        barstruct(ProcType pt, ColorType colT, ComponentType comT)
        {
            this->comtype = comT;
            this->proctype = pt;
            this->coltype = colT;
        }
    };

}
#endif // BARCONTAINER_H
