#pragma once

#include <bitset>
#include "barcodeCreator.h"
#include <fstream>
#include <functional>
#include <sstream>

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


struct BarBinFile
{
public:
	using act = uint;

private:
	std::fstream stream;
	//QFile binFile;

	bool writeMode;

	template<typename RTY>
	void readRaw(RTY& val)
	{
		const char size = sizeof(RTY);
		stream.read(reinterpret_cast<char*>(&val), size);
	}

	template<typename RTY>
	void writeRaw(RTY val)
	{
		const char size = sizeof(RTY);
		stream.write(reinterpret_cast<char*>(&val), size);
	}

	uint readInt(std::iostream& localStream)
	{
		uint vale;
		localStream.read(reinterpret_cast<char*>(&vale), 4);
		return vale;

		uchar data[4]{ 0, 0, 0, 0 };
		readRaw(data[0]);

		std::bitset<2> flag(data[0]);
		int asf = flag.to_ulong();
		switch (asf)
		{
		case 0:
			return static_cast<uint_6bit>(data[0]).v;
		case 1:
			localStream.read((char*)data, 1);
			return static_cast<uint_14bit>(*data).v;
		case 2:
			localStream.read((char*)data + 1, 2);
			return static_cast<uint_22bit>(*data).get();
		case 3:
			stream.read((char*)data + 1, 3);
			return static_cast<uint_30bit>(*data).v;
		default:
			throw;
		}
	}

	void writeInt(std::fstream& localStream, uint val)
	{
		localStream.write(reinterpret_cast<char*>(&val), 4);
		return;

		if (val < 64)
		{
			uint_6bit v(val);
			localStream.write(v.ptr(), 1);
		}
		else if (val < 16384)
		{
			uint_14bit v(val);
			localStream.write(v.ptr(), 2);
		}
		else if (val < 16777216)
		{
			uint_22bit v(val);
			localStream.write(v.ptr(), 3);
		}
		else
		{
			uint_30bit v(val);
			localStream.write(v.ptr(), 4);
		}
	}

public:
	bool openRead(const std::string& path)
	{
		writeMode = false;
		//binFile.setFileName(path);
		stream.open(path, std::ios::in | std::ios::binary);

		if (stream.is_open())
		{
			uint size;
			readRaw(size);
			for (uint i = 0; i < size; ++i)
			{
				size_t off;
				readRaw(off);
				memoffs.push_back(off);
			}
			return true;
		}
		else
			return false;
	}

	bool openWrite(std::string path, int maxBufferSize = 10000)
	{
		writeMode = true;
		stream.open(path, std::ios::out | std::ios::binary);

		return stream.is_open();
	}


	std::vector<size_t> memoffs;
	void writeHeaderProto(int itemsSize)
	{
		writeRaw((uint)itemsSize);
		for (int i = 0; i < itemsSize; ++i)
		{
			size_t memOff = 0;
			writeRaw(memOff);
		}
	}

	struct sets
	{
		int totalSize;
	};

	bool ended()
	{
		return stream.eof();
	}

	BarcodeHolder* readItem(const int index)
	{
		for (size_t var = 0; var < memoffs.size(); ++var)
		{
			// seekg for input
			stream.seekg(memoffs[var]);

			int sindex;
			readRaw(sindex);
			if (sindex == index)
			{
				stream.seekg(memoffs[var]);
				BarcodesHolder tmp = read(sindex);
				assert(tmp.lines.size() > 0);
				auto* line = tmp.lines[0];
				tmp.lines.clear();
				return line;
			}
		}
		return nullptr;
	}

	BarcodesHolder read(int& index)
	{
		barmap<uint, bc::barline*> ids;

		readRaw(index);
		//qDebug() << "ID:" << index;

		BarType bt;
		int btRaw;
		readRaw(btRaw);
		bt = (BarType)btRaw;

		BarcodesHolder rsitem;

		std::function<Barscalar(std::iostream& stream)> parseBarscal;

		int ysize;
		switch (bt)
		{
		case BarType::BYTE8_1:
			parseBarscal = [](std::iostream& stream) {
				Barscalar scal;
				scal.type = BarType::BYTE8_1;
				stream.read((char*)&scal.data.b1, 1);
				return scal;
			};
			ysize = 1;
			break;
		case BarType::BYTE8_3:
			parseBarscal = [](std::iostream& stream) {
				Barscalar scal;
				scal.type = BarType::BYTE8_3;
				stream.read((char*)scal.data.b3, 3);
				return scal;
			};
			ysize = 3;
			break;
		case BarType::FLOAT32_1:
			break;
			parseBarscal = [](std::iostream& stream) {
				Barscalar scal;
				scal.type = BarType::FLOAT32_1;
				stream.read(reinterpret_cast<char*>(&scal.data.f), 4);
				return scal;
			};
			ysize = 4;
		default:
			break;
		}

		auto& vec = rsitem.lines;
		size_t vecSize;
		readRaw(vecSize);

		for (size_t i = 0; i < vecSize; ++i)
		{
			BarcodeHolder* barlines = new BarcodeHolder();
			vec.push_back(barlines);

			uint id = readInt(stream);					// Read 4 (id)
			Barscalar start = parseBarscal(stream);		// Read barscalar
			Barscalar end = parseBarscal(stream);		// Read barscalar
			barlines->depth = readInt(stream);			// Read 4 (id)

			bc::barline* line = new bc::barline(start, end, 0);
			barlines->lines.push_back(line);

			//			ids.insert(id, line);
			//			qDebug() << "ID:" << id;

			act arrSize = readInt(stream);				// Read 4 (N)
			assert(arrSize < 3294967295);
			if (arrSize > 0)
			{
				std::vector<char> raw;
				raw.resize(static_cast<size_t>(arrSize) * (4ull + ysize));
				stream.read(raw.data(), raw.size());
				std::stringstream linestream;
				linestream.write(raw.data(), raw.size());

				for (act j = 0; j < arrSize; ++j)
				{
					uint index = readInt(linestream);			// Read 4 * N
					Barscalar val = parseBarscal(linestream);	// Read barscalar * N

					bc::barvalue v(bc::barvalue::getStatPoint(index), val);
					barlines->matrix.push_back(v);
				}
			}

			act arrSize2 = readInt(stream);			// Read 4 (N)
			assert(arrSize2 < 3294967295);

			//qDebug() << "arr:" << arrSize2;
			for (size_t jk = 0; jk < arrSize2; ++jk)
			{
				Barscalar start2 = parseBarscal(stream);	// Read barscalar * N
				Barscalar end2 = parseBarscal(stream);		// Read barscalar * N

				bc::barline* line2 = new bc::barline(start2, end2, 0);
				barlines->lines.push_back(line2);
			}
		}

		//		for (size_t i = 0; i < vecSize; ++i)
		//		{
		//			uint chlSize = readInt(stream);

		//			//			QByteArray raw2;
		//			//			raw2.resize(chlSize * (sizeof(size_t)));
		//			//			stream.read(raw2.data(), raw2.length());
		//			//			std::fstream ctream(&raw2, QIODevice::ReadOnly);

		//			auto *prnt = vec[i];
		//			for (act j = 0; j < chlSize; ++j)
		//			{
		//				uint idc = readInt(stream);
		//				assert(ids.contains(idc));
		////				if (ids.contains(idc))
		//				ids[idc]->setparent(prnt);
		//			}
		//		}

		return rsitem;
	}

	void write(const bc::Baritem* item, int index, sets& set)
	{
		barmap<bc::barline*, uint> ids;
		//		uint counter = 0;

		memoffs.push_back((size_t)stream.tellp());

		writeRaw(index);
		BarType bt = item->barlines[0]->matr[0].value.type;
		writeRaw((int)bt);

		std::function<void(std::fstream& stream, const Barscalar& scal)> valueFunction;

		switch (bt)
		{
		case BarType::BYTE8_1:
			valueFunction = [](std::fstream& stream, const Barscalar& scal) { stream.write((const char*)&scal.data.b1, 1); };
			break;
		case BarType::BYTE8_3:
			valueFunction = [](std::fstream& stream, const Barscalar& scal) { stream.write((const char*)scal.data.b3, 3); };
			break;
		case BarType::FLOAT32_1:
			break;
			valueFunction = [](std::fstream& stream, const Barscalar& scal) { stream.write(reinterpret_cast<const char*>(&scal.data.f), 4); };
		default:
			break;
		}

		size_t afterPos = stream.tellp();
		const bc::barlinevector& vec = item->barlines;
		writeRaw(vec.size());

		size_t realIndex = 0;
		for (size_t i = 0; i < vec.size(); ++i)
		{
			bc::barline* line = vec[i];

			float proc = 100.f * (float)line->matr.size() / set.totalSize;
			//			if (proc < 0.01 || proc > 10)
			//			{
			//				continue;
			//			}

			uint rid = realIndex++;
			ids.insert(std::make_pair(line, rid));

			writeInt(stream, rid);						// Write int (id)
			valueFunction(stream, line->start);			// Write barscalar
			valueFunction(stream, line->end());			// Write barscalar
			writeInt(stream, line->getDeath());			// Write int (depth)

			auto& arr = line->matr;
			assert(arr.size() < 4294967295);
			writeInt(stream, (act)arr.size());		// Write 4 bytes (N)
			for (act j = 0; j < arr.size(); ++j)
			{
				writeInt(stream, arr[j].index);			// Write 4 * N
				valueFunction(stream, arr[j].value);	// Write barscalar * N
			}

			{
				BarcodeHolder barhold;

				line->getChilredAsList(barhold.lines, false, false);
				assert(barhold.lines.size() < 4294967295);
				act arrSize2 = barhold.lines.size();
				writeInt(stream, arrSize2); 	// Write 4 bytes (N)
				//qDebug() << arrSize2;

				for (size_t jk = 0; jk < arrSize2; ++jk)
				{
					bc::barline* line2 = barhold.lines[jk];

					valueFunction(stream, line2->start);	// Write barscalar * N
					valueFunction(stream, line2->end());	// Write barscalar * N
				}
				barhold.lines.clear();
			}
		}

		size_t curPos = stream.tellp();
		stream.seekp(afterPos);
		writeRaw(realIndex);
		stream.seekp(curPos);

		//		for (size_t i = 0; i < vec.size(); ++i)
		//		{
		//			bc::barline *line = vec[i];
		//			auto &chl = line->children;
		//			act chlSize = chl.size();
		//			writeInt(stream, chlSize);
		//			for (act i = 0; i < chlSize; ++i)
		//			{
		//				assert(ids.contains(chl[i]));
		//				writeInt(stream, ids[chl[i]]);
		//			}
		//		}
	}

	void close()
	{
		if (writeMode)
		{
			stream.seekp(0);

			writeRaw((uint)memoffs.size());
			for (size_t i = 0; i < memoffs.size(); ++i)
			{
				writeRaw(memoffs[i]);
			}
			writeMode = false;
		}
		stream.close();
	}
};
