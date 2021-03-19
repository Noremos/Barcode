#include "pch.h"
#include "CppUnitTest.h"

#include "../Barcode/include/barcodeCreator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace BarcodeTests
{
	TEST_CLASS(BarcodeTests)
	{
		bc::BarConstructor<uchar> defineConstr()
		{

			bc::BarConstructor<uchar> bcont;
			bcont.addStructure(bc::ProcType::f0t255, bc::ColorType::gray, bc::ComponentType::Component);
			bcont.createBinayMasks = false;
			bcont.createGraph = false;
			bcont.createBinayMasks = false;
			bcont.returnType = bc::ReturnType::barcode2d;
			//bcont.setMaxPorog(1);
			//bcont.maxTypeValue.set(255);

			return bcont;
		}
	public:
		
		TEST_METHOD(TestMethod6)
		{
			bc::BarImg<uchar> img(1,1);
			bc::BarcodeCreator<uchar> test;
			auto bcont = defineConstr();

			uchar* data6 = new uchar[36]{
			63,121,73,14,120,135,
			237,90,194,136,4,43,
			90,212,193,199,88,154,
			51,150,98,239,42,68,
			65,141,145,34,203,167,
			158,234,20,145,80,176
			};

			img.assignRawData(6, 6, 1, data6);
			auto* ret = test.createBarcode(&img, bcont);

			Assert::AreEqual(ret->count(), (size_t)1);
		}

		TEST_METHOD(TestMethod5)
		{
			bc::BarImg<uchar> img(1, 1);
			bc::BarcodeCreator<uchar> test;
			auto bcont = defineConstr();

			uchar* data5 = new uchar[25]{
			63,121,73,14,120,
			237,90,194,136,4,
			90,212,193,199,88,
			51,150,98,239,42,
			65,141,145,34,203
			};

			img.assignRawData(5, 5, 1, data5);
			test.createBarcode(&img, bcont);

			
		}

		TEST_METHOD(TestMethod4)
		{
			bc::BarImg<uchar> img(1, 1);
			bc::BarcodeCreator<uchar> test;
			auto bcont = defineConstr();

			uchar* data4 = new uchar[16]{
			63,121,73,14,
			237,90,194,136,
			90,212,193,199,
			51,150,98,239
			};

			img.assignRawData(4, 4, 1, data4);
			test.createBarcode(&img, bcont);
		}

		TEST_METHOD(TestMethod3)
		{
			bc::BarImg<uchar> img(1, 1);
			bc::BarcodeCreator<uchar> test;
			auto bcont = defineConstr();

			uchar* data3 = new uchar[9]{
			63,121,73,
			237,90,194,
			90,212,193
			};

			img.assignRawData(3, 3, 1, data3);
			test.createBarcode(&img, bcont);
		}
	};
}
