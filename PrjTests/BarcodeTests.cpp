#include "pch.h"
#include "CppUnitTest.h"

#include "../PrjBarlib/include/barcodeCreator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace BarcodeTests
{
	TEST_CLASS(BarcodeTests)
	{
		bc::BarConstructor defineConstr()
		{

			bc::BarConstructor bcont;
			bcont.addStructure(bc::ProcType::f0t255, bc::ColorType::gray, bc::ComponentType::Component);
			bcont.createBinaryMasks = true;
			bcont.createGraph = false;
			bcont.createBinaryMasks = false;
			bcont.returnType = bc::ReturnType::barcode2d;
			//bcont.setMaxPorog(1);
			//bcont.maxTypeValue.set(255);

			return bcont;
		}
	public:

		TEST_METHOD(TestMethod6)
		{
			bc::BarImg img(1, 1);
			bc::BarcodeCreator test;
			auto bcont = defineConstr();

			uchar* data6 = new uchar[36]{
			63,121,73,14,120,135,
			237,90,194,136,4,43,
			90,212,193,199,88,154,
			51,150,98,239,42,68,
			65,141,145,34,203,167,
			158,234,20,145,80,176
			};

			img.setDataU8(6, 6, data6);
			auto* ret = test.createBarcode(&img, bcont);

			Assert::AreEqual(ret->count(), (size_t)1);
		}

		//TEST_METHOD(TestMethod5)
		//{
		//	bc::BarImg img(1, 1);
		//	bc::BarcodeCreator test;
		//	auto bcont = defineConstr();

		//	uchar* data5 = new uchar[25]{
		//	63,121,73,14,120,
		//	237,90,194,136,4,
		//	90,212,193,199,88,
		//	51,150,98,239,42,
		//	65,141,145,34,203
		//	};

		//	img.setDataU8(5, 5, data5);
		//	test.createBarcode(&img, bcont);

		//	
		//}

		//TEST_METHOD(TestMethod4)
		//{
		//	bc::BarImg img(1, 1);
		//	bc::BarcodeCreator test;
		//	auto bcont = defineConstr();

		//	uchar* data4 = new uchar[16]{
		//	63,121,73,14,
		//	237,90,194,136,
		//	90,212,193,199,
		//	51,150,98,239
		//	};

		//	img.setDataU8(4, 4á data4);
		//	test.createBarcode(&img, bcont);
		//}

		//TEST_METHOD(TestMethod3)
		//{
		//	bc::BarImg img(1, 1);
		//	bc::BarcodeCreator test;
		//	auto bcont = defineConstr();

		//	uchar* data3 = new uchar[9]{
		//	63,121,73,
		//	237,90,194,
		//	90,212,193
		//	};

		//	img.setDataU8(3, 3, data3);
		//	test.createBarcode(&img, bcont);
		//}

		TEST_METHOD(TestSkipZeros)
		{
			bc::BarImg img(1, 1);
			bc::BarcodeCreator test;
			auto bcont = defineConstr();

			uchar* data3 = new uchar[9]{
			0,1,0,
			0,1,0,
			1,0,1
			};

			img.setDataU8(3, 3, data3);
			std::unique_ptr<bc::Barcontainer> bar;
			bar.reset(test.createBarcode(&img, bcont));
			bc::Baritem* itm = bar->getItem(0);
			Assert::AreEqual(itm->barlines.size(), (size_t)1);
			Assert::AreEqual(itm->barlines[0]->len().getByte8(), (uchar)1);
		}


		TEST_METHOD(TestSkipZerosRadius)
		{
			bc::BarImg img(1, 1);
			bc::BarcodeCreator test;
			auto bcont = defineConstr();
			bcont.structure[0].comtype = bc::ComponentType::RadiusComp;

			uchar* data3 = new uchar[9]{
			0,1,0,
			0,1,0,
			1,0,1
			};
			uchar* data2 = new uchar[4]{
				0,1,
				1,0
			};

			img.setDataU8(2, 2, data2);
			std::unique_ptr<bc::Barcontainer> bar;
			bar.reset(test.createBarcode(&img, bcont));
			bc::Baritem* itm = bar->getItem(0);
			Assert::AreEqual((size_t)2, itm->barlines.size());
			Assert::AreEqual((float)1, itm->barlines[0]->len().getAvgUchar());
			Assert::AreEqual((float)1, itm->barlines[1]->len().getAvgUchar());
		}
	};
}
