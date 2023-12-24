#ifndef SKIP_M_INC
#include "tiffreader.h"
#endif // !SKIP_M_INC

#ifndef QSLOT_CODE
#include <iostream>
#define OUT cout
#else
#include <QDebug>
#define OUT qDebug()
#endif // !_QT

#include <stdio.h>

#include <algorithm>

#include <string>
#include <cassert>
#include "sidesrc/fast_float.h"



using std::cout;
using std::string;
using std::vector;

//barmap<int, CoordProjection> CoordProjection::map{{4326, {1,-1}}};


//string dectodeTEST(uchar* buffer, int len);

size_t TiffIFD::getTagIntValue(size_t offOrValue, size_t count, char format, bool is64)
{
	if (is64)
	{
		if (count > 8)
		{
			uchar buffer[8]{ 0,0,0,0,0,0,0,0 };
			reader->read(buffer, offOrValue, format == 4 ? 4 : format == 8 ? 8 : 2);
			return toInt64(buffer);
		}
		else
			return offOrValue;
	}
	else
	{
		if (count > 4)
		{
			uchar buffer[4]{ 0,0,0,0 };
			reader->read(buffer, offOrValue, format == 4 ? 4 : 2);
			return toInt(buffer);
		}
		else
			return offOrValue;
	}
}

int getTypeSize(uchar type);


size_t TiffIFD::getTagValue(size_t offOrValue, size_t count, char format, bool is64, ushort tag)
{
	int formatSize = getTypeSize(format);
	int totalLen = count * formatSize;

	reader->tempbuffer.allocate(totalLen);
	if (is64)
	{
		if (totalLen > 8)
		{
			reader->read(reader->tempbuffer.buffer, offOrValue, totalLen);
		}
		else
			return offOrValue;
	}
	else
	{
		if (totalLen > 4)
		{
			reader->read(reader->tempbuffer.buffer, offOrValue, totalLen);
		}
		else
			return offOrValue;
	}

	TiffTags::ExtraData ex;
	int cou = 0;
	assert(count <= 4);
	for (ushort i = 0; i < count; ++i)
	{
		TiffTags::ExtraData::EDVariant val;
		switch (format)
		{
		case 1: // byte
		case 6: // byte
		case 16:
			val.ll = (long long)reader->tempbuffer.buffer[0];
			break;
		case 3:
		case 8:
			val.ll = (long long) toShort(reader->tempbuffer.buffer);
			break;
		case 4:
		case 9:
			val.ll = (long long)toInt(reader->tempbuffer.buffer);
			break;
		default:
			throw std::exception();
		}

		ex.extra[cou++] = val;
	}

	tags.extraTegs.insert(std::pair<ushort, TiffTags::ExtraData>(tag, ex));

	return ex.extra[0].ll;
}

/*
 * http://geocurSubImage->tags.maptools.org/spec/geotiff2.7.html#2.7
		 * Header={KeyDirectoryVersion, KeyRevision, MinorRevision, NumberOfKeys}
		 * KeyEntry = { KeyID, TIFFTagLocation, Count, Value_Offset }
		 * If TIFFTagLocation=0, then Value_Offset contains the actual (SHORT)
		 * TIFFTagLocation=GeoKeyDirectoryTag
		 * Example:
  GeoKeyDirectoryTag=(   1,     1, 2,     6,
					  1024,     0, 1,     2,
					  1026, 34737,12,     0,
					  2048,     0, 1, 32767,
					  2049, 34737,14,    12,
					  2050,     0, 1,     6,
					  2051, 34736, 1,     0 )
		 */
void TiffIFD::processGeoHeader(uchar *buffer)
{
//	ushort KeyDirectoryVersion = toShort(buffer);
//	ushort KeyRevision = toShort(buffer + 2);
//	ushort MinorRevision = toShort(buffer + 4);
	ushort NumberOfKeys = toShort(buffer + 6);
//	OUT << "";
//	OUT << "KeyDirectoryVersion: " << KeyDirectoryVersion;
//	OUT << "KeyRevision: " << KeyRevision;
//	OUT << "MinorRevision: " << MinorRevision;
//	OUT << "NumberOfKeys: " << NumberOfKeys;
//	OUT << "";

	for (int i = 0; i < NumberOfKeys; ++i)
	{
		processGeoentry(buffer + 8 + i * 8);
	}
//	OUT << "";
}


void TiffIFD::processGeoentry(uchar *buffer)
{
	ushort KeyID = toShort(buffer);
	ushort TIFFTagLocation = toShort(buffer + 2);
	ushort Count = toShort(buffer + 4);
	ushort Value_Offset = toShort(buffer + 6);

	size_t value = 0;

	if (TIFFTagLocation == 0)
		value = Value_Offset;

	switch (KeyID)
	{
	case GeotiffTags::GTModelTypeGeoKey:
		geotags.GTModelTypeGeoKey = value;
//		OUT << "GTModelTypeGeoKey:" << value;
		break;
	case GeotiffTags::GTCitationGeoKey:
		reader->readAsciiFromBuffer(geotags.GTCitationGeoKey, Value_Offset, Count);
//		OUT << "GTCitationGeoKey:" << geotags.GTCitationGeoKey.c_str();
		break;
	case GeotiffTags::GTRasterTypeGeoKey:
//		geotags.GTRasterTypeGeoKey = value;
//		OUT << "GTRasterTypeGeoKey (DEPR):" << value;
		break;
	case GeotiffTags::GeogCitationGeoKey:
		reader->readAsciiFromBuffer(geotags.GeogCitationGeoKey, Value_Offset, Count);
//		OUT << "GeogCitationGeoKey:" << geotags.GeogCitationGeoKey.c_str();
		break;
	case GeotiffTags::GeographicTypeGeoKey:
		geotags.GeographicTypeGeoKey = value;
//		OUT << "GeographicTypeGeoKey:" << value;
		break;
	case GeotiffTags::ProjectedCSTypeGeoKey:
		geotags.ProjectedCSTypeGeoKey = value;
//		OUT << "ProjectedCSTypeGeoKey:" << value;
		break;
	default:
//		OUT << "Unknown geotag:" << KeyID << ", Value_Offset = " << Value_Offset;
//		return;
		break;
	}

//	OUT << "KeyID: " << KeyID;
//	OUT << "TIFFTagLocation: " << TIFFTagLocation;
//	OUT << "Count: " << Count;
//	OUT << "Value_Offset: " << Value_Offset;
}


int TiffIFD::getBytesInRowToTile() const
{
	if (tags.TileWidth != 0)
	{
		return tags.TileWidth * tags.TileLength * getSampleTypeSize() * tags.SamplesPerPixel;
	}
	else
	{
		return tags.ImageWidth * getSampleTypeSize() * tags.SamplesPerPixel;
	}
}

void TiffIFD::printValue(int x, int y)
{
	uchar buffer[4];
	reader->read(buffer, tags.StripOffsets + y * 4, getTypeSize(tags.StripOffsetsType));
	int off = toInt(buffer);

	reader->read(buffer, tags.StripByteCounts + y * 4, getTypeSize(tags.StripByteCountsType));
	int count = toInt(buffer);

	reader->tempbuffer.allocate(count + 3);
	reader->read(reader->tempbuffer.buffer, off, count);
	reader->tempbuffer.setLast3ToSero();

	string st = "";// dectode(buff, count);
	vbuffer ret;
	decorder decod(tags.Compression);
	decod.decompress(reader->tempbuffer.buffer, count, ret, getBytesInRowToTile());

	//tiff_lzw_decode(buff, count, st);
	//OUT << st.length();
	//string vals = st.substr(x * 4, 4);
	//float temp = toFloat((uchar*)vals.c_str());

	uchar *dataStart = ret.data() + x * getSampleTypeSize() * tags.SamplesPerPixel;
	switch (getType())
	{
	case ImageType::int8:
		OUT << dataStart[0];
		break;
	case ImageType::int16:
		OUT << toShort(dataStart);
		break;
	case ImageType::int32:
		OUT << toInt(dataStart);
		break;
	case ImageType::float32:
	{
		double temp = toFloat(dataStart);
		int val = (int) round(temp * 10000000);
		OUT << val;
		OUT << (val) / 10000000.;
		break;
	}
	case ImageType::float64:
	{
		double temp = toDouble(dataStart);
		int val = (int) round(temp * 10000000);
		OUT << val;
		OUT << (val) / 10000000.;
		break;
	}
	default:
		std::exception();
	}
}

void TiffIFD::printTag(uchar* buffer, bool is64)
{
	//	OUT << "----Tag data----";
	ushort tag = toShort(buffer);
	ushort type = toShort(buffer + 2);
	long long count = is64 ? toInt64(buffer + 4) : toInt(buffer + 4);
	long long value = is64 ? toInt64(buffer+12) : toInt(buffer + 8);

	bool print = true;
	switch ((Tags)tag) // ModelPixelScaleTag
	{
	case Tags::TileWidth:
		tags.TileWidth = getTagIntValue(value, count, type, is64);
		break;
	case Tags::TileLength:
		tags.TileLength = getTagIntValue(value, count, type, is64);
		break;
	case Tags::ImageWidth:
		tags.ImageWidth = getTagIntValue(value, count, type, is64);
		//		OUT << "ImageWidth";
		break;
	case Tags::ImageLength:
		tags.ImageLength = getTagIntValue(value, count, type, is64);
		//		OUT << "ImageLength";
		break;
	case Tags::PlanarConfiguration:
		tags.PlanarConfiguration = getTagIntValue(value, count, type, is64);
		//		OUT << "PlanarConfiguration";
		break;
	case Tags::TileOffsets:
		isTiled = true;
		tilesCount = count;
		tags.TileOffsets = value;
		tags.TileOffsetsType = type;
		break;

	case Tags::StripOffsets:
		tags.StripOffsets = value;
		tags.StripOffsetsType = type;
		//		compressedLen = count;
		//		tags.ImageLength = count;
		//		OUT << "StripOffsets";
		break;

	case Tags::TileByteCounts:
		tags.TileByteCounts = value;
		tags.TileByteCountsType = type;
		tilesCount = count;
		break;

	case Tags::StripByteCounts:
		tags.StripByteCounts = value;
		tags.StripByteCountsType = type;
		//		tags.ImageLength = count;

		//		OUT << "StripByteCounts";
		break;

	case Tags::BitsPerSample:
		tags.BitsPerSample = getTagValue(value, count, type, is64, tag);
		//		OUT << "BitsPerSample";
		break;
	case Tags::SamplesPerPixel:
		tags.SamplesPerPixel = getTagIntValue(value, count, type, is64);
		//		OUT << "SamplesPerPixel";
		break;
	case Tags::Compression:
		tags.Compression = getTagIntValue(value, count, type, is64);
		//		OUT << "Compression";
		break;
	case Tags::SampleFormat:
		tags.SampleFormat = (tifftype)getTagValue(value, count, type, is64, tag);
		break;

	case Tags::ModelTiepointTag:
	{
		uchar *buffer2 = new uchar[count * sizeof(double)];
		reader->read(buffer2, value, count * sizeof(double));
		int k = count / 6;
		tags.ModelTiepointTag.resize(k);
		for (int lk = 0; lk < k; ++lk)
		{
			tags.ModelTiepointTag.add(buffer2 + lk * 6 * sizeof(double));
		}
		delete[] buffer2;
		//		OUT << "ModelTiepointTag";
		break;
	}
	case Tags::ModelPixelScaleTag: {
		assert(count == 3);

		uchar buffer[3 * sizeof(double)];
		reader->read(buffer, value, count * sizeof(double));
		tags.ModelPixelScaleTag.x = toDouble(buffer);
		tags.ModelPixelScaleTag.y = toDouble(buffer +  sizeof(double));
		tags.ModelPixelScaleTag.z = toDouble(buffer +  sizeof(double) * 2);

		//		OUT << "ModelPixelScaleTag";
		break;
	}
	case Tags::ModelTransformationTag: {
		assert(count == 16);
		uchar buffer[16 * sizeof(double)];
		reader->read(buffer, value, count * sizeof(double));
		for (size_t i = 0; i < (size_t)count; ++i)
		{
			tags.ModelTransformationTag[i] = toDouble(buffer + sizeof(double) * i);
		}

		//		OUT << "ModelTransformationTag" << tags.ModelTransformationTag;
		break;
	}

	case Tags::GeoPorjection:
	{
		uchar buffer[16];
		reader->read(buffer, value, count);
		tags.GeoProjection = (const char*)buffer;
		//		OUT << "ModelTransformationTag" << tags.ModelTransformationTag;
		break;
	}

	case Tags::NoData:
		tags.NoDataValue = reader->getFloatFromAscii(buffer, value, count, type, is64);
		//		OUT << "Nodata";
		break;

	case Tags::GeoKeyDirectoryTag:
	{
		uchar* buffer2 = new uchar[count * sizeof(short)];
		reader->read(buffer2, value, count * sizeof(short));
		processGeoHeader(buffer2);
		break;
	}

	case Tags::PhotometricInterpretation: {
		tags.PhotometricInterpretation = getTagIntValue(value, count, type, is64);
		break;
	}
	case Tags::ExtraSamples: {
		tags.ExtraSamples[0] = value;

//		count = MIN(16, count);
//		reader->read(buffer, value, count * sizeof(short));
//		for (int it = 0; it < count; ++it)
//		{
//			tags.ExtraSamples[it] = toShort(buffer + it * sizeof(short));
//		}
		break;
	}
	default:
		//		OUT << "Tag:" << (tag) << "; Value: " << value;
		print = false;
		break;
	}
	if (print && false)
	{

		//		OUT << "Tag identifying code:" << (tag);
		//		OUT << "Datatype of tag data:" << type;//3 - short; 4 -int 12 --ascii
		//		OUT << "Number of values:" << count;
		//		//	wid = toInt(buffer + 4);
		//		//dataOffset = toInt(buffer + 8);
		//		OUT << "Tag data or offset to tag data see below:" << value;
	}
	//In other words, if the tag data is smaller than or equal to 4 bytes, it fits. Otherwise, it is stored elsewhere and pointed to.
}



ImageType TiffIFD::getType() const
{
	switch (tags.SampleFormat)
	{
	case tifftype::tiff_uint:
	{
		switch (tags.BitsPerSample)
		{
		case 8:
			return ImageType::int8;
		case 16:
			return ImageType::int16;
		case 32:
			return ImageType::int32;
		default:
			std::exception();
			break;
		}
	}
	case tifftype::tiff_short:
		return ImageType::int16;
	case tifftype::tiff_float:
		return tags.BitsPerSample == 64 ? ImageType::float64 : ImageType::float32;
	case tifftype::tiff_void:
	default:
		std::exception();
	}
	return ImageType::float32;
}

TiffReader::TiffReader()
	:pFile(NULL)
{
}

int TiffReader::printHeader(uchar* buffer)
{
	bool print = true;
	short versNum = toShort(buffer + 2);
	if (!print)
		return versNum;
	OUT << "----Header----";
	OUT << "Byte order:" << buffer[0] << buffer[1]; //“II”(4949.H)“MM” (4D4D.H).

	OUT << "Version number :" << (toShort(buffer + 2));
	if (versNum == 42)
	{
		// Standart tiff
		OUT << "Offset to first IFD:" << (toInt(buffer + 4));
	}
	else
	{
		// BigTiff
		OUT << "Bytesize of offsets :" << (toShort(buffer + 4));

		OUT << "Constant :" << (toShort(buffer + 6));
	}

	return versNum;
}

void TiffReader::readAsciiFromBuffer(std::string &output, int offset, int Count)
{
	output.resize(Count, ' ');
	uchar *buff = (uchar *) output.c_str();
	read(buff, offset, Count);
}

float TiffReader::getFloatFromAscii(uchar* sourceBuffer, size_t offOrValue, size_t count, char format, bool is64)
{
	assert(format == 2);
	std::string buffer;
	const size_t ptrSize = is64 ? 8 : 4;
	if (count > ptrSize)
	{
		readAsciiFromBuffer(buffer, offOrValue, count);
	}
	else
		buffer = offOrValue;

	for (size_t i = 0; i < buffer.length(); ++i)
	{
		if (buffer[i] == ',')
			buffer[i] = '.';
	}
//	fast_float::from_chars((const char*)buffer, (const char*)(buffer + count), val);
	return std::stof(buffer);
}

void TiffReader::read(uchar* buffer, offu64 offset, offu64 len)
{
#ifdef _MSC_VER
	_fseeki64(pFile, offset, SEEK_SET);
#else
	fseeko64(pFile, offset, SEEK_SET);
#endif
	if (!fread(buffer, 1, len, pFile))
		OUT << "ERROR?";
	if (feof(pFile))
		OUT << "ERROR2?";
}

void TiffReader::setTitleCacheSize(size_t n)
{
	cachedTiles.setMaxElems(n);
}

void TiffReader::setRowsCacheSize(size_t n)
{
	cachedRows.setMaxElems(n);
}


int TiffReader::width()
{
	return curSubImage->tags.ImageWidth;
}

int TiffReader::height()
{
	return curSubImage->tags.ImageLength;
}

ImageType TiffReader::getType()
{
	return curSubImage->getType();
}

int TiffReader::getSamples()
{
	return curSubImage->tags.SamplesPerPixel;
}


//void **TiffReader::checkTileInCache(int tileX, int tileY)
//{
//	int TilesAcross = (curSubImage->tags.ImageWidth + curSubImage->tags.TileWidth - 1) / curSubImage->tags.TileWidth;

//	int index = tileY * TilesAcross + tileX;
//	return cachedTiles.getData(tileNum, nullptr);
//}

int getTypeSize(uchar type)
{
	//https://www.itu.int/itudoc/itu-t/com16/tiff-fx/docs/tiff6.pdf
	//  0			 1		  2			   3		 4
	//  nullptr,     "BYTE",  "ASCII",     "SHORT",  "LONG",
	//  5			 6		  7			   8		 9
	//	"RATIONAL",  "SBYTE", "UNDEFINED", "SSHORT", "SLONG",
	//  10			 11		  12		   13		 14
	//	"SRATIONAL", "FLOAT", "DOUBLE",    "IFD",    0,
	//  15			 16		  17		   18
	//  0,			 "LONG8", "SLONG8",    "IFD8"
	static const int sizes[]{0, 1, 1, 2, 4,/**/
							 0, 1, 0, 2, 4,/**/
							 0, 4, 8, 0, 0,/**/
							 0, 8, 8, 0};
	return sizes[type];
//	return type == 4 ? 4 : 2;
}

int getAddnl(int main, int minor)
{
	//  (ImageWidth + TileWidth - 1) / TileWidth
	//	return (main / minor) + (main % minor == 0 ? 0 : 1);
	return (main + minor - 1) / minor;
}

int TiffReader::getTileWid(int x)
{
	//curSubImage->tags.ImageWidth = 1001
	//curSubImage->tags.TileWidth = 100
	//rowNum = 10
	//(1001 - 100*10) = 0001 < 100 = 1
	//(1001 - 100*5) = 501 > 100= 10
	auto &tags = curSubImage->tags;
	const size_t &TileWid = tags.TileWidth;
	size_t widleft = (tags.ImageWidth - TileWid * x);
	return widleft < TileWid ? widleft : TileWid;
}

//Offsets are ordered left-to-right and top-to-bottom.
cachedRow TiffReader::getTiffTile(int x, int y, bool asConst)
{
	int TilesInWid = getAddnl(curSubImage->tags.ImageWidth, curSubImage->tags.TileWidth);
	int TilesInHei = getAddnl(curSubImage->tags.ImageLength, curSubImage->tags.TileLength);
	int tileNum = y * TilesInWid + x;
	assert(tileNum < TilesInWid * TilesInHei);
	return getTiffTile(tileNum, asConst);
}

rowptr TiffReader::getTiffTileInner(int ind, int samples)
{
	//offset tile
	uchar bbuffer[8];

	ushort sie = getTypeSize(curSubImage->tags.TileOffsetsType);
	// SamplesPerPixel * TilesPerImage * N,  N is a size of type e.g. bytes to read
	uint indexOffset = ind;
	read(bbuffer, curSubImage->tags.TileOffsets + indexOffset * sie, sie);
	size_t off = (sie <= 4 ? toInt(bbuffer) : toInt64(bbuffer));
	//************

	//Count
	sie = getTypeSize(curSubImage->tags.TileByteCountsType);
	read(bbuffer, curSubImage->tags.TileByteCounts + indexOffset * sie, sie);
	size_t count = (sie <= 4 ? toInt(bbuffer) : toInt64(bbuffer));

	//data
	tempbuffer.allocate(count + 3);
	read(tempbuffer.buffer, off, count);
	tempbuffer.setLast3ToSero();

	{
		decorder decod(curSubImage->tags.Compression);

		vbuffer returnTemp;
		decod.decompress(tempbuffer.buffer, count, returnTemp, curSubImage->getBytesInRowToTile()); // (rowInTile + 1) * bytsInTileWid

		int cou = MIN(returnTemp.size(), curSubImage->tags.TileWidth * curSubImage->tags.TileLength);
		//		size_t ft = bytsInTileWid * curSubImage->tags.TileLength;
		return processData(returnTemp.extract(), cou, samples);
	}
}


cachedRow TiffReader::getTiffTile(int ind, bool)
{
	RowPtrHolder *null = nullptr;
	RowPtrHolder* data = cachedTiles.getData(ind, null);
	//	const int bytsInTileWid = curSubImage->tags.TileWidth * sizeof(float);

	if (data != nullptr)
	{
		return data;
	}

	int samples = curSubImage->tags.SamplesPerPixel;
	if (curSubImage->tags.PlanarConfiguration == 1)
	{
		rowptr row = getTiffTileInner(ind, samples);
		data = new RowPtrHolder(row);
	}
	else
	{
		std::vector<rowptr> rows(samples);
		for (int i = 0; i < samples; ++i)
			rows.emplace_back(getTiffTileInner(ind * samples + i, 1));

		{
			rowptr r;
			r.set(rows[0].count, rows[0].type, samples);
			data = new RowPtrHolder(r);
		}
		for (size_t i = 0; i < data->row.count; ++i)
		{
			for (int j = 0; j < samples; ++j)
			{
				data->row.setValue(i, j, rows[j].getValue(i));
			}
		}
	}

	cachedTiles.storeData(ind, data);

	return data;
}

void TiffReader::removeTileFromCache(int ind)
{
	if (cachedTiles.isInCache(ind))
	{
		cachedTiles.setNull(ind, nullptr);
	}
}

rowptr TiffReader::processData(uchar* bytes, int len, int samplesInOne)
{
	rowptr data;
	data.samples = samplesInOne;
	data.count = len;
	data.type = getType();

	const int fullLen = len * samplesInOne;
	switch (data.type)
	{
	case ImageType::int8:
		data.ptr.b = bytes;
		break;
	case ImageType::int16:
		data.ptr.s = setData<short>(bytes, fullLen);
		break;
	case ImageType::int32:
		data.ptr.i = setData<int>(bytes, fullLen);
		break;
	case ImageType::float32:
		data.ptr.f = setData<float>(bytes, fullLen);
		break;
	case ImageType::float64:
	{
		data.ptr.d = setData<double>(bytes, fullLen);

		//uchar* raw = new uchar[fullLen / 2];
		//for (size_t i = 0, k = 0; i < fullLen; i += 8)
		//{
		//	raw[k++] = bytes[i];
		//	raw[k++] = bytes[i + 1];
		//	raw[k++] = bytes[i + 2];
		//	raw[k++] = bytes[i + 3];
		//}
		//data.type = ImageType::float32;
		//data.ptr.f = setData<float>(raw, fullLen / 2);
		break;
	}
		//	case ImageType::float64:
		//		data = setData<double>(bytes, len);
		//		break;
//	case ImageType::argb8:
//		data = (int)bytes;
//		break;

	default:
		//		data = reinterpret_cast<rowptr>(bytes);
		throw;
		break;
	}
	data.deletePtr = true;
	return data;
}

rowptr TiffReader::getRowData(int y)// rowNum
{
	if (curSubImage->tags.TileWidth != 0)
	{
		//Offsets are ordered left-to-right and top-to-bottom.

		// Сначала вычисляет в каком тайле есть нужная строка,
		//
		int TilesInRow = getAddnl(curSubImage->tags.ImageWidth, curSubImage->tags.TileWidth); // TilesAcross
//		int TilesDown = getAddnl(curSubImage->tags.ImageLength, curSubImage->tags.TileLength);
//		int TilesPerImage = TilesAcross * TilesDown;
		cachedTiles.setMaxElems(TilesInRow+2);

		/*
				col0  col1  col2
		row0	/\ /\ /\ /\ /\
				\/ \/ \/ \/ \/
		row1	/\ /\ /\ /\ /\
				\/ \/ \/ \/ \/
		row2	/\ /\ /\ /\ /\
				\/ \/ \/ \/ \/
		*/

		//|------|------$-x----|-----|----|-----
		//-------------rowTileNum----

		//Номер тайла Y
		int tileY = (y / curSubImage->tags.TileLength);

		// Строка в тайле row == widht
		int rowInTile = y % curSubImage->tags.TileLength;

		// У нас N строк. В каждой W занчений. Мы должны взять
		// i-тую строку из тайла, лля этого умножаем W * i == TileWidth * rowInTile
		const int rowStartedPos = curSubImage->tags.TileWidth * rowInTile;
		rowptr imageRow;
		imageRow.set(curSubImage->tags.ImageWidth, getType(), curSubImage->tags.SamplesPerPixel);
		for (int i = 0; i < TilesInRow; ++i)
		{
			rowptr& data = getTiffTile(i, tileY, true)->row;
			// Add to row
			//			ret.reserve( ret.size() + bytsInTileWid );
			const unsigned long dataLen = getTileWid(i);
			const unsigned long imageRowOffset = i * curSubImage->tags.TileWidth;

			assert(imageRowOffset + dataLen <= curSubImage->tags.ImageWidth);

			imageRow.copyDataFrom(data.getOffset(rowStartedPos), imageRowOffset, dataLen);
		}
		return imageRow;
	}
	else
	{
		uchar bbuffer[4];

		offu64 sie = getTypeSize(curSubImage->tags.StripOffsetsType);
		read(bbuffer, curSubImage->tags.StripOffsets + y * sie, sie);
		uint off =  sie == 2 ? toShort(bbuffer) : toInt(bbuffer);

		sie = getTypeSize(curSubImage->tags.StripByteCountsType);
		read(bbuffer, curSubImage->tags.StripByteCounts + y * sie, sie);
		uint count = sie == 2 ? toShort(bbuffer) : toInt(bbuffer);


		tempbuffer.allocate(count + 3);
		read(tempbuffer.buffer, off, count);
		tempbuffer.setLast3ToSero();

		string st = ""; // dectode(buff, count);
		vbuffer ret;
		decorder decod(static_cast<int>(curSubImage->tags.Compression));
		decod.decompress(tempbuffer.buffer, count, ret, curSubImage->getBytesInRowToTile());
//		vbuffer ret2;
//		decompressLZW(tempbuffer.tempbuffer, count, ret2, getBytesInRowToTile());
//		int r = memcmp(ret.data(), ret2.data(), getBytesInRowToTile());

//		assert(r == 0);

		return processData(ret.extract(), width(), static_cast<int>(curSubImage->tags.SamplesPerPixel));
	}
}

void TiffReader::printIFD(offu64 offset)
{
	//OUT << "----IDF----";
	uchar* buffer;
	uchar temp[2];

	read((uchar*)temp, offset, 2);
	ushort idfNums = toShort(temp);
	//OUT << "Number of tags in IFD:" << tagNums << endl;

	//--- after all entries 2+n*12	32-bit	offset to next directory or zero
	// 2 for tagNums and 4 for offset to next directory or zero

	buffer = new uchar[12 * idfNums + 4];
	read(buffer, offset + 2, 12 * idfNums + 4);

	subImages.push_back(new TiffIFD());
	TiffIFD *idf = subImages.back();
	idf->setReader(this);
	for (int i = 0; i < idfNums; ++i)
	{
		idf->printTag(buffer + 12 * i, false);
	}
	uint oofset = toInt(buffer + 12 * idfNums);

	delete[] buffer;

	if (oofset != 0)
		printIFD(oofset);
}

void TiffReader::printBigIFD(size_t offset)
{
	//OUT << "----IDF----";
	uchar *buffer;
	uchar temp[8];

	// number of directory entries
	read((uchar *) temp, offset, 8);
	size_t idfNums = toInt64(temp);
	//OUT << "Number of tags in IFD:" << tagNums << endl;


	//--- after		all entries
	// 8+n*20	64-bit	offset to next directory or zero
	int TAG_SIZE = 20;
	int OFFSET_TO_FIRST_TEG = 8;
	int SIZE_OF_NEXT_IFD = 8;

	const int toReadCount = TAG_SIZE * idfNums + SIZE_OF_NEXT_IFD;

	buffer = new uchar[toReadCount];
	read(buffer, offset + OFFSET_TO_FIRST_TEG, toReadCount);

	subImages.push_back(new TiffIFD());
	TiffIFD* idf = subImages.back();
	idf->setReader(this);
	for (size_t i = 0; i < idfNums; ++i)
	{
		idf->printTag(buffer + TAG_SIZE * i, true);
	}
	size_t oofset = toInt64(buffer + idfNums * TAG_SIZE);

	delete[] buffer;

	if (oofset != 0)
		printBigIFD(oofset);
}

bool TiffReader::open(const std::string& path)
{
	uchar buffer[8];
	ready = false;
	isTile = false;

	fopen_s(&pFile, path.c_str(), "rb");
	if (pFile == NULL)
	{
		perror("Error opening file");
		return false;
	}
	else
	{
		read(buffer, 0, 8);
		::GLOBALloclByteOreder = buffer[0];
		int verNum = printHeader(buffer);
		if (verNum==42)
		{
			// Standart tiff
			uint idfOffset = toInt(buffer + 4);
			printIFD(idfOffset);
		}
		else
		{
			assert(verNum == 43);
			read(buffer, 8, 8);
			size_t idfOffset = toInt64(buffer);
			OUT << "Offset to first IFD:" << idfOffset;

			// BigTiff tiff
			printBigIFD(idfOffset);
		}

		if (subImages.size() > 0)
			curSubImage = subImages[0];
	}
//	tempbuffer = new uchar[count]
	ready = true;
	return true;
}

TiffReader::~TiffReader()
{
	TiffReader::close();
	for (int i = 0, total = subImages.size(); i < total; ++i)
		delete subImages[i];
}

void TiffReader::close()
{
	if (pFile)
		fclose(pFile);

	cachedTiles.clear();
}

struct ReadRange
{
	int start;
	int end;
	ReadRange(int st, int wid, int maxEnd)
	{
		start = st;
		end = st + wid;
		if (end > maxEnd)
			end = maxEnd;
	}
	ReadRange(int st, int ed)
	{
		start = st;
		end = ed;
	}

	ReadRange getDeler(int deler)
	{
		return ReadRange(start / deler, end / deler);
	}
};
struct ReadOffset
{
	int row = 0;
	int x = 0;
};

struct ReadSize
{
	int wid = 0, len = 0;
};
struct ReadTile
{
	const TiffTags& tags;
	// ReadSize srcTileSize; // Реальный размер тайла (на окраине вместо tag.TileWidth)
	int rowOffset;
	int xOffset;
	ReadSize size;
	ReadTile(const TiffTags& tags) : tags(tags)
	{
		xOffset = 0;
		rowOffset = 0;
	}

	void setXOffset(int off)
	{
		xOffset = off;
		size.wid = tags.TileWidth - off;
	}

	void setRowOffset(int off)
	{
		rowOffset = off;
		size.len = tags.TileLength - off;
	}

	size_t calcOffset()
	{
		return tags.TileWidth * rowOffset + xOffset;
	}
};


enum GetForState { gfs_first = 0, gfs_middle = 1, gfs_last = 2, gfs_single = 3 };

DataRect TiffReader::getRect(int stX, int stRow, int wid, int hei)
{
	if (stX < 0 || stX >= this->width() || stRow < 0 || stRow >= height() || wid <= 0 || hei <=0)
		return nullptr;

	ReadRange widPosRange(stX, wid, this->width());
	ReadRange rowPosRange(stRow, hei, this->height());

	DataRect output(wid, hei, getType(), curSubImage->tags.SamplesPerPixel);
	if (curSubImage->tags.TileWidth != 0)
	{
		auto& tags = curSubImage->tags;
		const int tagTileWid = curSubImage->tags.TileWidth;
		const int tagTileLen = curSubImage->tags.TileLength;

		// ReadTile lastTile(tags, widPosRange.end % tagTileWid, rowPosRange.end % tagTileLen);
		const int lastTileWid = widPosRange.end % tagTileWid;
		const int lastTileLen = rowPosRange.end % tagTileLen;

		const ReadRange widsRange = widPosRange.getDeler(tagTileWid);
		const ReadRange rowsRange = rowPosRange.getDeler(tagTileLen);

		// 1500 % 256 = 220; 256 - 220 = !36!
		const int xOffsetInFirstTile = widPosRange.start % tagTileWid;
		const int rowOffsetInFirstTile = rowPosRange.start % tagTileLen;

		// ReadOffset
		ReadTile tilePart(tags);
		ReadOffset outuptCoordOffset;

//		QImage remp(wid, hei, QImage::Format_RGB888);

		const bool oneRow = (rowOffsetInFirstTile + hei <= tagTileLen);
		const bool oneWid = (xOffsetInFirstTile + wid <= tagTileWid);
		GetForState rowState = oneRow ? gfs_single : gfs_first;
		for (int tileRowInd = rowsRange.start; tileRowInd <= rowsRange.end; ++tileRowInd, outuptCoordOffset.row += tilePart.size.len)
		{
			// Начальная строка в выходном изображении.
			// Деланием смещение на height строк т.к. записываем весь считанный тайл

			switch (rowState)
			{
			case gfs_first:
				tilePart.setRowOffset(rowOffsetInFirstTile);
				break;
			case gfs_middle:
				tilePart.setRowOffset(0);
				break;
			case gfs_last:
				tilePart.setRowOffset(0);
				tilePart.size.len = lastTileLen;
				break;
			case gfs_single:
				tilePart.setRowOffset(rowOffsetInFirstTile);
				tilePart.size.len = hei;
				break;
			}

			outuptCoordOffset.x = 0;
			GetForState xState = oneWid ? gfs_single : gfs_first;
			for (int tileXind = widsRange.start; tileXind <= widsRange.end; ++tileXind, outuptCoordOffset.x += tilePart.size.wid)
			{
				cachedRow data = getTiffTile(tileXind, tileRowInd);

				// Начальное смещение и сколько остлось считать данных
				switch (xState)
				{
				case gfs_first:
					tilePart.setXOffset(xOffsetInFirstTile);
					break;
				case gfs_middle:
					tilePart.setXOffset(0);
					break;
				case gfs_last:
					tilePart.setXOffset(0);
					tilePart.size.wid = lastTileWid;
					break;
				case gfs_single:
					tilePart.setXOffset(xOffsetInFirstTile);
					tilePart.size.wid = wid;
					break;
				}

				int offsetInReadRow = tilePart.calcOffset();
				for (int h = 0; h < tilePart.size.len; ++h, offsetInReadRow += tagTileWid)
				{
					output.setInRow(outuptCoordOffset.row + h, outuptCoordOffset.x, data->row.getOffset(offsetInReadRow), tilePart.size.wid);
				}

//				for (uint r = 0; r < data->row.count; ++r)
//				{
//					const auto v = data->row.getValue(r);
//					const auto* p = v.rgba.samples;
//					remp.setPixel(outuptCoordOffset.x + (r % tagTileWid), outuptCoordOffset.row + (r / tagTileWid), qRgb(p[0].s,p[1].s,p[2].s));
//				}
//				remp.save("D:\\temp.png");

				// Сравниваем слудующий индекс с последним валидным
				if (tileXind == widsRange.end - 1)
					xState = gfs_last;
				else
					xState = gfs_middle;
			}

			if (tileRowInd == rowsRange.end - 1)
				rowState = gfs_last;
			else
				rowState = gfs_middle;
		}
#ifdef IMPPART
		exportDataAsBeaf(Project::getProject()->getTilePath(index), img.wid, img.hei, img.data);
#endif


		//load 9
		// * * *
		// * * *
		// * * *
		return output;
		//		reader->getRowData()
	}
	else
	{
		size_t rowInDest = 0;
		for (; stRow < rowPosRange.end; ++stRow)
		{
			cachedRow data = getRow(stRow, true);
			output.setInRow(rowInDest++, 0, data->row.getOffset(stX), wid);
		}
	}
#ifdef IMPPART
	exportDataAsBeaf(Project::getProject()->getTilePath(index), img.wid, img.hei, img.data);
#endif

	return output;
}
