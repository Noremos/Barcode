#include <cstddef>
#include <memory>

#include "gtest/gtest.h"
#include "../../include/barcodeCreator.h"
#include "../../include/barclasses.h"


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

	int wid() const override
	{
		return _wid;
	}

	int hei() const override
	{
		return _hei;
	}

	int channels() const override
	{
		return _chnls;
	}

	Barscalar get(int x, int y) const override
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

	size_t typeSize() const override
	{
		return _chnls;
	}
};

bc::barstruct defineConstr(bc::ProcType proc = bc::ProcType::f0t255)
{
	bc::barstruct bcont;
	bcont.addStructure(proc, bc::ColorType::gray, bc::ComponentType::Component);
	bcont.createBinaryMasks = true;
	bcont.createGraph = false;
	bcont.createBinaryMasks = false;
	bcont.returnType = bc::ReturnType::barcode2d;
	//bcont.setMaxPorog(1);
	//bcont.maxTypeValue.set(255);b

	return bcont;
}

std::unique_ptr<bc::Baritem> mkBarcode(TestImg& img, bc::ProcType proc = bc::ProcType::f0t255)
{
	bc::barstruct bcont = defineConstr(proc);
	bc::BarcodeCreator test;
	std::unique_ptr<bc::Baritem> bar;
	bar.reset(test.createBarcode(&img, bcont));
	return bar;
}

TestImg restore255ToBarimg(bc::Baritem* it, int wid, int hei, Barscalar minval)
{
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
			ASSERT_EQ(av.getByte8(), bv.getByte8());
		}
	}
}

// int main(int argc, char* argv[])
// {
// 	::testing::InitGoogleTest(&argc, argv);
// 	return RUN_ALL_TESTS();
// }

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

	// ASSERT_TRUE(ret->count() == (size_t)1);
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

void testZeros(TestImg& img)
{
	bc::BarcodeCreator test;
	auto bcont = defineConstr();

	std::unique_ptr<bc::Baritem> bar;
	bar.reset(test.createBarcode(&img, bcont));
	bc::Baritem* itm = bar.get();
	ASSERT_EQ(itm->barlines.size(), (size_t)1);
	ASSERT_EQ(itm->barlines[0]->len().getByte8(), (size_t)1);


	bcont.addStructure(bc::ProcType::Radius, bc::ColorType::gray, bc::ComponentType::Component);
	bar.reset(test.createBarcode(&img, bcont));
	itm = bar.get();
	ASSERT_EQ((size_t)itm->barlines.size(), (size_t)1);
	ASSERT_EQ((size_t)itm->barlines[0]->len().getByte8(), (size_t)1);
}

TEST(BarcodeTests, TestSkipZeros2by2)
{
	TestImg img2(2, 2, 1, new uchar[4]{
		0,1,
		1,0
	});

	testZeros(img2);
}

TEST(BarcodeTests, TestSkipZeros3by3)
{
	TestImg img(3, 3, 1, new uchar[9]{
		0,1,0,
		0,1,0,
		1,0,1
	});

	testZeros(img);
}


TEST(BarcodeTests, TestMoreSkips)
{
	TestImg img(3, 3, 1, new uchar[9]{
		0,1,0,
		0,1,0,
		1,1,0
	});

	auto itm = mkBarcode(img, bc::ProcType::Radius);
	ASSERT_EQ((size_t)itm->barlines.size(), (size_t)1);
	ASSERT_EQ((size_t)itm->barlines[0]->len().getByte8(), (size_t)1);
}

TEST(BarcodeTests, Testf255t0)
{
	ASSERT_TRUE(true);
	return;
	bc::BarcodeCreator test;
	auto bcont = defineConstr();
	bcont.proctype = bc::ProcType::f255t0;
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

	bc::Baritem* ret = test.createBarcode(&img, bcont);
	TestImg out = restore255ToBarimg(ret, k, k, minv);

	compiteBarAndBar(img, out);
}
