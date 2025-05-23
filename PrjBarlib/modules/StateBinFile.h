#ifndef SKIP_M
#pragma once

#include <bitset>
#include <fstream>
#include <functional>
#include <sstream>

#include "../include/barscalar.h"
#endif


MEXP namespace StateBinFile
{
	using uint = unsigned int;
	using uchar = unsigned char;

	struct uint_6bit
	{
		// simple cases
		uchar x1 : 1;
		uchar x2 : 1;
		uint8_t v : 6;
		uint_6bit(uint v = 0) : x1(0), x2(0), v(v) {}

		const char* ptr() { return reinterpret_cast<const char*>(this); }
	};

	struct uint_14bit
	{
		// simple cases
		uint16_t x1 : 1;
		uint16_t x2 : 1;
		uint16_t v : 14;
		uint_14bit(uint val = 0) : x1(0), x2(1), v(val) {}

		const char* ptr() { return reinterpret_cast<const char*>(this); }
	};

	struct uint_22bit
	{
		// simple cases
		uchar x1 : 1;
		uchar x2 : 1;
		uchar v0 : 6, v1, v2;
		uint_22bit(uint val) : x1(1), x2(0) { set(val); }

		void set(uint val)
		{
			v0 = (uchar)((val >> 16) & 0xff);
			v1 = (uchar)((val >> 8));
			v2 = (uchar)(val & 0xff);
		}

		uint get()
		{
			uint out = v0 << 16;
			out = out | (v1 << 8);
			out = out | v1;
			return out;
		}

		const char* ptr()
		{
			return reinterpret_cast<const char*>(this);
		}
	};

	struct uint_30bit
	{
		// simple cases
		uint x1 : 1;
		uint x2 : 1;
		uint v : 30;
		uint_30bit(uchar val) : x1(1), x2(1), v(val)
		{}

		const char* ptr()
		{
			return reinterpret_cast<const char*>(this);
		}
	};

	static_assert(sizeof(uint_6bit) == 1, "not 1 byte");
	static_assert(sizeof(uint_14bit) == 2, "not 2 byte");
	static_assert(sizeof(uint_22bit) == 3, "not 3 byte");
	static_assert(sizeof(uint_30bit) == 4, "not 4 byte");


	class BinState
	{
	public:
		using act = uint;

		virtual bool open(const std::string& path) = 0;

		virtual bool isReading() = 0;
		//
		virtual void beginItem() = 0;
		virtual void endItem() = 0;


		virtual int pType(BarType type) = 0;
		virtual bool pBool(bool value) = 0;
		virtual short pShort(short) = 0;
		virtual act pInt(act value) = 0;
		virtual float pFloat(float value) = 0;
		virtual size_t pInt64(size_t value) = 0;
		virtual Barscalar pBarscalar(const Barscalar& value) = 0;
		virtual BackString pString(const BackString& value) = 0;

		virtual uchar* pElement(const uchar* arrayData, size_t elId, size_t elSize) = 0;
		//virtual void pElementDynamic(const uchar* arrayData, size_t elId, size_t elSize) = 0;

		virtual act pArray(act arrSize) = 0;
		//virtual act pFixedArray(act arraySize, act elementSize) = 0;
		//virtual void endFixedArray() = 0;


		template<class D>
		void beginArray(D& array, const size_t& size)
		{
			if (isReading())
			{
				array.resize(size);
			}
		}


		//template<class T*, class D>
		//T* beginPtrArrayElement(D& array, size_t& index)
		//{
		//	if (isReading())
		//	{
		//		array[index] = new T();
		//	}

		//	return array[index];
		//}

		//template<class T, class D>
		//T beginArrayElement(D& array, size_t& index)
		//{
		//	return array[index];
		//}

		//template<class T*, class D, class A1>
		//T* beginPtrArrayElement(D& array, size_t& index, A1 arg)
		//{
		//	if (isReading())
		//	{
		//		array[index] = new T(arg);
		//	}
		//	return array[index];
		//}

		//template<class T*, class D, class A1, class A2>
		//T* beginPtrArrayElement(D& array, size_t& index, A1 arg1, A2 arg2)
		//{
		//	if (isReading())
		//	{
		//		array[index] = new T(arg1, arg2);
		//	}
		//	return array[index];
		//}

		//template<class T*, class D, class A1, class A2, class A3>
		//T* beginPtrArrayElement(D& array, size_t& index, A1 arg1, A2 arg2, A3 arg3)
		//{
		//	if (isReading())
		//	{
		//		array[index] = new T(arg1, arg2, arg3);
		//	}
		//	return array[index];
		//}

		virtual bool ended() = 0;
		virtual void close() = 0;
		virtual ~BinState()
		{ }
	};


	class BinStateReader : public BinState
	{
		std::function<Barscalar(std::istream& stream)> parseBarscal;

		std::ifstream mainStream;
		std::istream& stream;
		bool inside;
		//std::ifstream backup;
		//std::unique_ptr<bc::Baritem> itemPtr;

		template<typename RTY>
		void readRaw(RTY& val)
		{
			const char size = sizeof(RTY);
			stream.read(reinterpret_cast<char*>(&val), size);
		}

		std::vector<size_t> memoffs;
		size_t itemsEndPos = 0;

	public:

		BinStateReader() : stream(mainStream), inside(true)
		{ }

		BinStateReader(std::istream& text) : stream(text), inside(false)
		{
			intiOnOpen();
		}

		virtual ~BinStateReader()
		{
			close();
		}

		void beginItem() override
		{
			//itemPtr.reset(new bc::Baritem());
		}

		void endItem() override
		{
			//return itemPtr.release();
		}

		virtual bool isReading() override
		{
			return true;
		}

		void intiOnOpen()
		{
			readRaw(itemsEndPos);

			size_t curPos = stream.tellg();

			stream.seekg(itemsEndPos);

			uint size;
			readRaw(size);
			for (uint i = 0; i < size; ++i)
			{
				size_t off;
				readRaw(off);
				memoffs.push_back(off);
			}

			stream.seekg(curPos);
		}


		bool open(const std::string& path, bool trivial)
		{
			if (!inside)
				throw;

			//binFile.setFileName(path);
			mainStream.open(path, std::ios::binary);

			if (mainStream.is_open())
			{
				if (!trivial)
					intiOnOpen();

				return true;
			}
			else
				return false;
		}

		bool open(const std::string& path) override
		{
			return open(path, false);
		}

		void moveIndex(int index)
		{
			assert(index < memoffs.size());
			stream.seekg(memoffs[index]);
		}

		size_t getIndexSize()
		{
			return memoffs.size();
		}

		virtual bool ended() override
		{
			return stream.eof() || stream.tellg() >= itemsEndPos;
		}
		virtual void close() override
		{
			if (inside)
				mainStream.close();
		}

		int pType(BarType) override
		{
			int ysize = 0;
			BarType bt = (BarType)pInt(0);
			switch (bt)
			{
			case BarType::BYTE8_1:
				parseBarscal = [](std::istream& stream) {
					Barscalar scal;
					scal.type = BarType::BYTE8_1;
					stream.read((char*)&scal.data.b1, 1);
					return scal;
				};
				ysize = 1;
				break;
			case BarType::BYTE8_3:
			case BarType::BYTE8_4:
				parseBarscal = [](std::istream& stream) {
					Barscalar scal;
					scal.type = BarType::BYTE8_3;
					stream.read((char*)scal.data.b3, 3);
					return scal;
				};
				ysize = 3;
				break;
			case BarType::FLOAT32_1:
				parseBarscal = [](std::istream& stream) {
					Barscalar scal;
					scal.type = BarType::FLOAT32_1;
					stream.read((char*)&scal.data.f, 4);

					return scal;
				};
				ysize = 4;
				break;
			default:
				throw std::exception();
				break;
			}

			return ysize;
		}

		bool pBool(bool) override
		{
			uchar value;
			readRaw<uchar>(value);
			return value != 0;
		}

		short pShort(short) override
		{
			short vale;
			readRaw(vale);
			return vale;
		}

		act pInt(act) override
		{
			act vale;
			readRaw(vale);
			return vale;
		}

		float pFloat(float) override
		{
			float val;
			readRaw(val);
			return val;
		}

		size_t pInt64(size_t) override
		{
			size_t vale;
			readRaw(vale);
			return vale;
		}

		Barscalar pBarscalar(const Barscalar&) override
		{
			return parseBarscal(stream);
		}

		BackString pString(const BackString&) override
		{
			ushort len;
			readRaw(len);

			BackString buff;
			buff.resize(len);
			stream.read(buff.data(), len);
			return buff;
		}

		uchar* pElement(const uchar* begin, size_t elId, size_t elSize) override
		{
			char* start = (char*)begin + elId * elSize;
			stream.read(start, elSize);
			return (uchar*)start;
		}

		act pArray(act arrSize) override
		{
			return pInt(arrSize);
		}

		//act pFixedArray(act arraySize, act elementSize)
		//{

		//	raw.resize(arraySize * elementSize);
		//	stream->read(raw.data(), raw.length());
		//	assert(backup == nullptr);
		//	backup = stream;
		//	stream = new linestream(&raw, QIODevice::ReadOnly);
		//}

		//void endFixedArray()
		//{
		//	assert(backup);
		//	delete stream;
		//	stream = backup;
		//}

	};


	class BinStateWriter : public BinState
	{
		std::function<void(std::ostream& stream, const Barscalar& scal)> valueFunction;
		std::vector<size_t> memoffs;
		bool inside;
		template<typename RTY>
		void writeRaw(RTY val)
		{
			const char size = sizeof(RTY);
			stream.write(reinterpret_cast<char*>(&val), size);
		}

		std::ostream& stream;
		std::ofstream filestream;

	public:

		BinStateWriter() : inside(true), stream(filestream)
		{ }

		BinStateWriter(std::ostream& outStream) : inside(false), stream(outStream)
		{
			size_t memOff = 0;
			writeRaw(memOff);
		}

		virtual ~BinStateWriter()
		{
			close();
		}

		virtual bool isReading() override
		{
			return false;
		}

		bool open(const std::string& path) override
		{
			//binFile.setFileName(path);
			filestream.open(path, std::ios::binary | std::ios::trunc);

			size_t memOff = 0;
			writeRaw(memOff);
			// for (int i = 0; i < itemsSize; ++i)
			// {
			// 	writeRaw(memOff);
			// }
			return filestream.is_open();
		}

		virtual bool ended() override
		{
			throw;
		}

		int pType(BarType bt) override
		{
			pInt((int)bt);

			switch (bt)
			{
			case BarType::BYTE8_1:
				valueFunction = [](std::ostream& stream, const Barscalar& scal) { stream.write((const char*)&scal.data.b1, 1); };
				break;
			case BarType::BYTE8_3:
			case BarType::BYTE8_4:
				valueFunction = [](std::ostream& stream, const Barscalar& scal) { stream.write((const char*)scal.data.b3, 3); };
				break;
			case BarType::FLOAT32_1:
				valueFunction = [](std::ostream& stream, const Barscalar& scal) {stream.write((char*)&scal.data.f, 4); };
				break;
			default:
				break;
			}

			return (int)bt;
		}

		bool pBool(bool value) override
		{
			writeRaw<uchar>(value ? uchar(1) : uchar(0));
			return value;
		}

		short pShort(short value) override
		{
			writeRaw(value);
			return value;
		}

		act pInt(act value) override
		{
			writeRaw(value);
			return value;
		}

		float pFloat(float value) override
		{
			writeRaw(value);
			return value;
		}

		size_t pInt64(size_t value) override
		{
			writeRaw(value);
			return value;
		}

		Barscalar pBarscalar(const Barscalar& value) override
		{
			valueFunction(stream, value);
			return value;
		}

		BackString pString(const BackString& value) override
		{
			assert(value.length() < USHRT_MAX);
			const ushort len = static_cast<ushort>(value.length());

			pShort(len);
			stream.write(value.data(), len);
			return value;
		}

		uchar* pElement(const uchar* begin, size_t elId, size_t elSize) override
		{
			char* start = (char*)begin + elId * elSize;
			stream.write(start, elSize);
			return (uchar*)start;
		}

		act pArray(act arrSize) override
		{
			return pInt(arrSize);
		}


		void beginItem() override
		{
			//item = input;
			memoffs.push_back(stream.tellp());
		}

		void endItem() override
		{
			//return item;
		}

		void close() override
		{
			if (filestream.is_open() || !inside)
			{
				size_t curPos = stream.tellp();

				writeRaw((uint)memoffs.size());
				for (int i = 0; i < memoffs.size(); ++i)
				{
					writeRaw(memoffs[i]);
				}

				stream.seekp(0);
				writeRaw(curPos);
			}

			filestream.close();
		}
	};
}
