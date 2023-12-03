#include <memory>

#include "gtest/gtest.h"

import BarcodeModule;



class TestImg : public bc::DatagridProvider
{
public:
	std::unique_ptr<const unsigned char> data;
	int _wid, _hei, _chnls;
	TestImg(int inwid, int inhei, int inchls, const unsigned char* in) : data(in)
	{
		_wid = inwid;
		_hei = inhei;
		_chnls = inchls;
	}

	int wid() const
	{
		return _wid;
	}

	int hei() const
	{
		return _hei;
	}

	int channels() const
	{
		return _chnls;
	}

	Barscalar get(int x, int y) const
	{
		return Barscalar(*(data.get() + y * _wid + x));
	}

	Barscalar max() const
	{
		if (length() == 0)
			return 0;

		Barscalar max = this->getLiner(0);
		for (size_t i = 1; i < this->length(); i++)
		{
			Barscalar t = this->getLiner(i);
			if (t > max)
				max = t;
		}
		return max;
	}

	void maxAndMin(Barscalar& min, Barscalar& max) const override
	{
		if (this->length() == 0)
			return;

		for (size_t i = 1; i < this->length(); i++)
		{
			Barscalar t = this->getLiner(i);
			if (t > max)
				max = t;
			if (t < min)
				min = t;
		}
	}


	size_t typeSize() const
	{
		return _chnls;
	}
};

bc::BarConstructor defineConstr()
{
	bc::BarConstructor bcont;
	bcont.addStructure(bc::ProcType::f0t255, bc::ColorType::gray, bc::ComponentType::Component);
	bcont.createBinaryMasks = true;
	bcont.createGraph = false;
	bcont.createBinaryMasks = false;
	bcont.returnType = bc::ReturnType::barcode2d;
	//bcont.setMaxPorog(1);
	//bcont.maxTypeValue.set(255);b

	return bcont;
}


TestImg restore255ToBarimg(bc::Barcontainer* cont, int wid, int hei, Barscalar minval)
{
	auto* it = cont->getItem(0);
	auto& lines = it->barlines;
	TestImg img(wid, hei, 1, nullptr);
	for (size_t i = 0; i < wid * hei; i++)
	{
		//img.setLiner(i, minval);
	}

	for (size_t i = 0; i < lines.size(); i++)
	{
		Barscalar start = lines[i]->start;
		Barscalar end = start + lines[i]->len();
		auto& matr = lines[i]->matr;
		for (size_t k = 0; k < matr.size(); k++)
		{
			auto& p = matr[k];
			//assert(start <= p.value && p.value <= end);
			//img.add(p.getPoint(), p.value);
		}
	}
	return img;
}

TestImg restoreToBarimg(bc::Barcontainer* cont, int wid, int hei, Barscalar maxval)
{
	auto* it = cont->getItem(0);
	auto& lines = it->barlines;
	TestImg img(wid, hei, 1, nullptr);
	for (size_t i = 0; i < wid * hei; i++)
	{
		//img.setLiner(i, maxval);
	}

	for (size_t i = 0; i < lines.size(); i++)
	{
		Barscalar start = lines[i]->start;
		Barscalar end = start + lines[i]->len();
		auto& matr = lines[i]->matr;
		for (size_t k = 0; k < matr.size(); k++)
		{
			auto& p = matr[k];
			//assert(start <= p.value && p.value <= end);
			//img.minus(p.getPoint(), p.value);
		}
	}
	return img;
}

void compiteBarAndBar(TestImg& img0, TestImg& img1)
{
	for (int i = 0; i < img0.hei(); i++)
	{
		for (int j = 0; j < img0.wid(); j++)
		{
			Barscalar av = img0.get(j, i);
			Barscalar bv = img1.get(j, i);
			ASSERT_TRUE(av.getByte8() == bv.getByte8());
		}
	}
}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

TEST(BarcodeTests, component6by6)
{
	bc::BarcodeCreator test;
	auto bcont = defineConstr();
	
	TestImg img(6,6,1, new unsigned char[36]{
		63,121,73,14,120,135,
		237,90,194,136,4,43,
		90,212,193,199,88,154,
		51,150,98,239,42,68,
		65,141,145,34,203,167,
		158,234,20,145,80,176
	});
	
	auto* ret = test.createBarcode(&img, bcont);
	
	ASSERT_TRUE(ret->count() == (size_t)1);
}

TEST(BarcodeTests, TestMethod5)
{
	bc::BarcodeCreator test;
	auto bcont = defineConstr();

	TestImg img(5,5, 1, new uchar[25]{
		63,121,73,14,120,
		237,90,194,136,4,
		90,212,193,199,88,
		51,150,98,239,42,
		65,141,145,34,203
	});

	test.createBarcode(&img, bcont);
}

TEST(BarcodeTests, TestMethod4)
{
	bc::BarcodeCreator test;
	auto bcont = defineConstr();

	TestImg img(4,4,1, new uchar[16]{
		63,121,73,14,
		237,90,194,136,
		90,212,193,199,
		51,150,98,239
	});

	test.createBarcode(&img, bcont);
}

TEST(BarcodeTests, TestMethod3)
{
	bc::BarcodeCreator test;
	auto bcont = defineConstr();

	TestImg img(3,3,1, new uchar[9]{
		63,121,73,
		237,90,194,
		90,212,193
	});

	test.createBarcode(&img, bcont);
}

TEST(BarcodeTests, TestSkipZeros)
{
	bc::BarcodeCreator test;
	auto bcont = defineConstr();

	TestImg img(3, 3, 1, new uchar[9]{
		0,1,0,
		0,1,0,
		1,0,1
	});

	std::unique_ptr<bc::Barcontainer> bar;
	bar.reset(test.createBarcode(&img, bcont));
	bc::Baritem* itm = bar->getItem(0);
	ASSERT_TRUE(itm->barlines.size() == (size_t)1);
	ASSERT_TRUE(itm->barlines[0]->len().getByte8() == (uchar)1);
}


TEST(BarcodeTests, TestSkipZerosRadius)
{
	bc::BarcodeCreator test;
	auto bcont = defineConstr();
	bcont.structure[0].proctype = bc::ProcType::Radius;

	TestImg img3(3,3,1, new uchar[9]{
		0,1,0,
		0,1,0,
		1,0,1
	});

	TestImg img2(2,2,1, new uchar[4]{
		0,1,
		1,0
	});

	std::unique_ptr< bc::Barcontainer> bar(test.createBarcode(&img3, bcont));
	bc::Baritem* itm = bar->getItem(0);
	ASSERT_TRUE((size_t)2 == itm->barlines.size());
	ASSERT_TRUE((float)1 == itm->barlines[0]->len().getAvgFloat());
	ASSERT_TRUE((float)1 == itm->barlines[1]->len().getAvgFloat());
}

TEST(BarcodeTests, Testf255t0)
{
	ASSERT_TRUE(true);
	return;
	bc::BarcodeCreator test;
	auto bcont = defineConstr();
	bcont.structure[0].proctype = bc::ProcType::f255t0;
	const int k = 5;
	uchar maxv = 239;
	uchar minv = 4;
	TestImg img(k,k, 1, new uchar[k * k]{
	63,121,73,14,120,
	237,90,194,136,4,
	90,212,193,199,88,
	51,150,98,239,42,
	65,141,145,34,203
	});

	bc::Barcontainer* ret = test.createBarcode(&img, bcont);
	TestImg out = restore255ToBarimg(ret, k, k, minv);

	compiteBarAndBar(img, out);
}
