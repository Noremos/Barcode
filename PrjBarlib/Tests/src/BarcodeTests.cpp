#include <cstddef>
#include <memory>

#include "gtest/gtest.h"
#include "../../include/barcodeCreator.h"
#include "../../include/barclasses.h"
#include "../../include/barscalar.h"


class TestImg : public bc::DatagridProvider
{
public:
	std::unique_ptr<unsigned char> data;
	int _wid, _hei, _chnls;
	TestImg(int inwid, int inhei, int inchls, unsigned char* in) : data(in)
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

	void set(int x, int y, const Barscalar& val)
	{
		setLiner(y * _wid + x, val);
	}

	void setLiner(int i, const Barscalar& val)
	{
		for (int a = 0; a < _chnls; a++)
		{
			data.get()[i + a] = val[a];
		}
	}

	void add(int i, const Barscalar& val)
	{
		for (int a = 0; a < _chnls; a++)
		{
			data.get()[i + a] += val[a];
		}
	}

	void minus(int i, const Barscalar& val)
	{
		for (int a = 0; a < _chnls; a++)
		{
			data.get()[i + a] -= val[a];
		}
	}

	Barscalar max() const
	{
		if (length() == 0)
			return Barscalar(uchar(0));

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

		min = this->getLiner(0);
		max = this->getLiner(0);

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
	TestImg img(wid, hei, 1, new unsigned char[wid * hei]);
	for (size_t i = 0; i < wid * hei; i++)
	{
		img.setLiner(i, minval);
	}

	for (size_t i = 0; i < lines.size(); i++)
	{
		auto& matr = lines[i]->matr;
		for (size_t k = 0; k < matr.size(); k++)
		{
			auto& p = matr[k];
			//assert(start <= p.value && p.value <= end);
			img.add(p.getPoint().getLiner(wid), p.value);
		}
	}
	return img;
}

TestImg restoreToBarimg(bc::Baritem* it, int wid, int hei, Barscalar maxval)
{
	auto& lines = it->barlines;
	TestImg img(wid, hei, 1, new unsigned char[wid * hei]);
	for (size_t i = 0; i < wid * hei; i++)
	{
		img.setLiner(i, maxval);
	}

	for (size_t i = 0; i < lines.size(); i++)
	{
		auto& matr = lines[i]->matr;
		for (size_t k = 0; k < matr.size(); k++)
		{
			auto& p = matr[k];
			//assert(start <= p.value && p.value <= end);
			img.minus(p.getPoint().getLiner(wid), p.value);
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
			ASSERT_EQ(size_t(av.getByte8()), size_t(bv.getByte8()));
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
	ASSERT_EQ(itm->barlines[0]->len().getAvgUchar(), (size_t)1);


	bcont.addStructure(bc::ProcType::Radius, bc::ColorType::gray, bc::ComponentType::Component);
	bar.reset(test.createBarcode(&img, bcont));
	itm = bar.get();
	itm->sortBySize();
	ASSERT_EQ((size_t)itm->barlines.size(), (size_t)2);
	ASSERT_EQ((size_t)itm->barlines[0]->len().getAvgUchar(), (size_t)1);
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
	itm->sortBySize();
	ASSERT_EQ((size_t)itm->barlines.size(), (size_t)3);
	ASSERT_EQ((size_t)itm->barlines[0]->len().getAvgUchar(), (size_t)1);
}

TEST(BarcodeTests, Testf255t0)
{
	bc::BarcodeCreator test;
	auto bcont = defineConstr();
	const int k = 5;

	TestImg img(k,k, 1, new uchar[k * k]{
	63,121,73,14,120,
	237,90,194,136,4,
	90,212,193,199,88,
	51,150,98,239,42,
	65,141,145,34,203
	});
	Barscalar maxv;
	Barscalar minv;
	img.maxAndMin(minv, maxv);

	bc::Baritem* ret = test.createBarcode(&img, bcont);
	TestImg out = restoreToBarimg(ret, k, k, maxv);
	compiteBarAndBar(img, out);

	bcont.proctype = bc::ProcType::f255t0;
	ret = test.createBarcode(&img, bcont);
	out = restore255ToBarimg(ret, k, k, minv);
	compiteBarAndBar(img, out);
}



TEST(BarcodeTests, TestRestore)
{
	bc::BarcodeCreator test;
	auto bcont = defineConstr();
	const int n = 4;
	const int m = 3;

	TestImg img(n,m, 1, new uchar[n * m]{
	255,255,255,255,
	200,50,150,200,
	50,50,50,50
	});

	Barscalar maxv;
	Barscalar minv;
	img.maxAndMin(minv, maxv);

	bc::Baritem* ret = test.createBarcode(&img, bcont);
	TestImg out = restoreToBarimg(ret, n, m, maxv);
	compiteBarAndBar(img, out);

	bcont.proctype = bc::ProcType::f255t0;
	ret = test.createBarcode(&img, bcont);
	out = restore255ToBarimg(ret, n, m, minv);
	compiteBarAndBar(img, out);
}


TEST(BarcodeTests, TestRestore12)
{
	bc::BarcodeCreator test;
	auto bcont = defineConstr();
	const int n = 6;
	const int m = 4;

	TestImg img(n,m, 1, new uchar[n * m]{
	'9','9','9','9','9','9',
	'9','8','7','6','7','8',
	'9','6','6','6','6','6',
	'9','8','6','6','6','8'
	});

	Barscalar maxv;
	Barscalar minv;
	img.maxAndMin(minv, maxv);

	bc::Baritem* ret = test.createBarcode(&img, bcont);
	TestImg out = restoreToBarimg(ret, n, m, maxv);
	compiteBarAndBar(img, out);

	bcont.proctype = bc::ProcType::f255t0;
	ret = test.createBarcode(&img, bcont);
	out = restore255ToBarimg(ret, n, m, minv);
	compiteBarAndBar(img, out);
}