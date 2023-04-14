#ifndef TIFFREADER_H
#define TIFFREADER_H
//

#include <exception>
#include <stdio.h>
#include <string>
#include <vector>
#include <queue>

#include "ImageReader.h"
#include "convertion.h"
#include "decoder.h"
#include "sidesrc/flat_hash_map/unordered_map.hpp"

enum class Tags : int {
	//A general indication of the kind of data contained in this subfile.
	NewSubfileType = 254,

	//A general indication of the kind of data contained in this subfile.
	SubfileType = 255,

	//The number of columns in the image, i.e., the number of pixels per row.
	ImageWidth = 256,

	//The number of rows of pixels in the image.
	ImageLength = 257,

	//Number of bits per component.
	BitsPerSample = 258,

	//Compression scheme used on the image data.
	Compression = 259,

	//The color space of the image data.
	PhotometricInterpretation = 262,

	//For black and white TIFF files that represent shades of gray, the technique used to convert from gray to black and white pixels.
	Threshholding = 263,

	//The width of the dithering or halftoning matrix used to create a dithered or halftoned bilevel file.
	CellWidth = 264,

	//The length of the dithering or halftoning matrix used to create a dithered or halftoned bilevel file.
	CellLength = 265,

	//The logical order of bits within a byte.
	FillOrder = 266,

	//A string that describes the subject of the image.
	ImageDescription = 270,

	//The scanner manufacturer.
	Make = 271,

	//The scanner model name or number.
	Model = 272,

	//For each strip, the byte offset of that strip.
	StripOffsets = 273,

	//The orientation of the image with respect to the rows and columns.
	Orientation = 274,

	//The number of components per pixel.
	SamplesPerPixel = 277,

	//The number of rows per strip.
	RowsPerStrip = 278,

	//For each strip, the number of bytes in the strip after compression.
	StripByteCounts = 279,

	//The minimum component value used.
	MinSampleValue = 280,

	//The maximum component value used.
	MaxSampleValue = 281,

	//The number of pixels per ResolutionUnit in the ImageWidth direction.
	XResolution = 282,

	//The number of pixels per ResolutionUnit in the ImageLength direction.
	YResolution = 283,

	//How the components of each pixel are stored.
	PlanarConfiguration = 284,

	//For each string of contiguous unused bytes in a TIFF file, the byte offset of the string.
	FreeOffsets = 288,

	//For each string of contiguous unused bytes in a TIFF file, the number of bytes in the string.
	FreeByteCounts = 289,

	//The precision of the information contained in the GrayResponseCurve.
	GrayResponseUnit = 290,

	//For grayscale data, the optical density of each possible pixel value.
	GrayResponseCurve = 291,

	//The unit of measurement for XResolution and YResolution.
	ResolutionUnit = 296,

	//Name and version number of the software package(s) used to create the image.
	Software = 305,

	//Date and time of image creation.
	DateTime = 306,

	//Person who created the image.
	Artist = 315,

	//The computer and/or operating system in use at the time of image creation.
	HostComputer = 316,

	//A color map for palette color images.
	ColorMap = 320,

	//Description of extra components.
	ExtraSamples = 338,

	//Specifies how to interpret each data sample in a pixel.
	// 1 = uint; 2=int; 3=IEEEFP(float); 4=void
	SampleFormat = 339,
	//Copyright notice.
	Copyright = 33432,

	TileWidth = 322,
	TileLength = 323,
	TileOffsets = 324,

	TileByteCounts = 325,

	ModelPixelScaleTag = 33550,

	ModelTiepointTag = 33922,

	GeoKeyDirectoryTag = 34735,

	ModelTransformationTag = 34264,

	GeoPorjection = 34736, // unnemed| WGS 84|

	NoData = 42113
};

enum GeotiffTags: ushort
{
	GTModelTypeGeoKey = 1024,//SHORT
	GTRasterTypeGeoKey = 1025, //DEPR
	GTCitationGeoKey = 1026, //ASCII
	GeographicTypeGeoKey = 2048, //SHORT
	GeogCitationGeoKey = 2049, //ascii
	ProjectedCSTypeGeoKey = 3072,//SHORT
};

struct ModelTiepoint
{
	double I, J, K, X, Y, Z;
	ModelTiepoint()
	{}
	ModelTiepoint(double i, double j, double k, double x, double y, double z)
	{
		I = i;
		J = j;
		K = k;
		X = x;
		Y = y;
		Z = z;
	}
};


struct ModelTiepoints
{
	std::unique_ptr<ModelTiepoint[]> points;
	int c = 0;


	void resize(int n) { points.reset(new ModelTiepoint[n]); }

	void add(uchar* buffer)
	{
		double temp[6];
		int o = 0;
		temp[o++] = toDouble(buffer);
		buffer += sizeof(double);
		temp[o++] = toDouble(buffer);
		buffer += sizeof(double);
		temp[o++] = toDouble(buffer);
		buffer += sizeof(double);
		temp[o++] = toDouble(buffer);
		buffer += sizeof(double);
		temp[o++] = toDouble(buffer);
		buffer += sizeof(double);
		temp[o++] = toDouble(buffer);

		add(temp[0], temp[1], temp[2], temp[3], temp[4], temp[5]);
	}

	void add(double i, double j, double k, double x, double y, double z)
	{
		points[c++] = ModelTiepoint(i, j, k, x, y, z);
	}
};
struct Scale3d
{
	double x, y, z;
	Scale3d(double X =0, double Y = 0, double Z = 0)
	{
		x = X;
		y = Y;
		z = Z;
	}
};

enum class tifftype : short
{
	tiff_uint = 1,	// unsigned integer data
	tiff_short,		// two's complement signed integer data
	tiff_float,		// IEEE floating point data
	tiff_void,		// undefined data format
	SAMPLEFORMAT_COMPLEXINT,
	SAMPLEFORMAT_COMPLEXIEEEFP

};
#include <string>

struct GeoTiffTags
{
	short GTModelTypeGeoKey = 0;
	std::string GTCitationGeoKey; //ASCII Projection  text|3253
	short GeographicTypeGeoKey = 0; //SHORT
	std::string GeogCitationGeoKey;
	short ProjectedCSTypeGeoKey = 0; //SHORT
	short ProjectionGeoKey = 0;
	short ProjLinearUnitsGeoKey = 0;
	short ProjCoordTransGeoKey = 0;
};

struct TiffTags
{
	//A general indication of the kind of data contained in this subfile.
	size_t NewSubfileType = 0;

	//A general indication of the kind of data contained in this subfile.
	size_t SubfileType = 0;

	//The number of columns in the image, i.e., the number of pixels per row.
	size_t ImageWidth = 0;

	//The number of rows of pixels in the image.
	size_t ImageLength = 0;

	//Number of bits per component.
	size_t BitsPerSample = 0;

	//Compression scheme used on the image data.
	size_t Compression = 0;

	//The color space of the image data.
	short PhotometricInterpretation = 0;

	//For black and white TIFF files that represent shades of gray, the technique used to convert from gray to black and white pixels.
	size_t Threshholding = 0;

	//The width of the dithering or halftoning matrix used to create a dithered or halftoned bilevel file.
	size_t CellWidth = 0;

	//The length of the dithering or halftoning matrix used to create a dithered or halftoned bilevel file.
	size_t CellLength = 0;

	//The logical order of bits within a byte.
	size_t FillOrder = 0;

	//A string that describes the subject of the image.
	size_t ImageDescription = 0;

	//The scanner manufacturer.
	size_t Make = 0;

	//The scanner model name or number.
	size_t Model = 0;

	//For each strip, the byte offset of that strip.
	size_t StripOffsets = 0;
	char StripOffsetsType = 0;
	//The orientation of the image with respect to the rows and columns.
	size_t Orientation = 0;

	//The number of components per pixel.
	size_t SamplesPerPixel = 0;

	//The number of rows per strip.
	size_t RowsPerStrip = 0;

	//For each strip, the number of bytes in the strip after compression.
	size_t StripByteCounts = 0;
	char StripByteCountsType = 0;

	//The minimum component value used.
	size_t MinSampleValue = 0;

	//The maximum component value used.
	size_t MaxSampleValue = 0;

	//The number of pixels per ResolutionUnit in the ImageWidth direction.
	size_t XResolution = 0;

	//The number of pixels per ResolutionUnit in the ImageLength direction.
	size_t YResolution = 0;

	//How the components of each pixel are stored.
	size_t PlanarConfiguration = 0;

	//For each string of contiguous unused bytes in a TIFF file, the byte offset of the string.
	size_t FreeOffsets = 0;

	//For each string of contiguous unused bytes in a TIFF file, the number of bytes in the string.
	size_t FreeByteCounts = 0;

	//The precision of the information contained in the GrayResponseCurve.
	size_t GrayResponseUnit = 0;

	//For grayscale data, the optical density of each possible pixel value.
	size_t GrayResponseCurve = 0;

	//The unit of measurement for XResolution and YResolution.
	size_t ResolutionUnit = 0;

	//Name and version number of the software package(s) used to create the image.
	size_t Software = 0;

	//Date and time of image creation.
	size_t DateTime = 0;

	//Person who created the image.
	size_t Artist = 0;

	//The computer and/or operating system in use at the time of image creation.
	size_t HostComputer = 0;

	//A color map for palette color images.
	size_t ColorMap = 0;

	//Description of extra components.

	//Copyright notice.
	size_t Copyright = 0;

	size_t TileWidth = 0;

	size_t TileLength = 0;

	size_t TileOffsets = 0;
	char TileOffsetsType = 0;

	size_t TileByteCounts = 0;
	char TileByteCountsType = 0;

	float NoDataValue = -9999;

	// raster->model tiepoint pairs in the order
	// ModelTiepointTag = (...,I,J,K, X,Y,Z...)
	ModelTiepoints ModelTiepointTag;

	Scale3d ModelPixelScaleTag;

	double ModelTransformationTag[16];

	tifftype SampleFormat = tifftype::tiff_void;

	std::string GeoProjection;

	short ExtraSamples[16];

	struct ExtraData
	{
		union EDVariant
		{
			long long ll;
			double d;
		};
		EDVariant extra[4];
		ExtraData() { memset(extra, 0, 4 * sizeof(EDVariant));
		}

		//cpy
		ExtraData(const ExtraData& other)
		{
			memcpy(extra, other.extra, 4 * sizeof(EDVariant));
		}

		// mv
		ExtraData(ExtraData&& other)
		{
			memcpy(extra, other.extra, 4 * sizeof(EDVariant));
			memset(extra, 0, 4 * sizeof(EDVariant));
		}
	};

	ska::unordered_map<ushort, ExtraData> extraTegs;
};


class TiffReader;

class TiffIFD
{
public:
	TiffIFD()
	{
	}

	void setReader(TiffReader *reader)
	{
		this->reader = reader;
	}

	int getSampleTypeSize() const
	{
		assert(tags.BitsPerSample % 8 == 0);
		return tags.BitsPerSample / 8;
//		switch (tags.SampleFormat)
//		{
//		case tifftype::tiff_short:
//			return 2;
//		case tifftype::tiff_uint:
//			return 4;
//		case tifftype::tiff_float:
//			return tags.BitsPerSample == 64 ? 8 : 4;
//		case tifftype::tiff_void:
//		default:
//			std::exception();
//		}
		//return 0;
	}

	float getNullValue() const
	{
		return tags.NoDataValue;
	}

	void printTag(uchar *buffer, bool is64);
	int getBytesInRowToTile() const;

private:

	size_t getTagIntValue(size_t offOrValue, size_t count, char format, bool is64);

	void processGeoHeader(uchar *buffer);
	void processGeoentry(uchar *buffer);

	void printValue(int x, int y);
	size_t getTagValue(size_t offOrValue, size_t count, char format, bool is64, ushort tag);

public:
	TiffTags tags;
	GeoTiffTags geotags;
	ImageType getType() const;

	int getFirstNormProj() const
	{
		const std::string* st;
		if (geotags.GTCitationGeoKey.length() > 0)
		{
			st = &geotags.GTCitationGeoKey;
		}
		else
		{
			st = &tags.GeoProjection;
		}

		const std::string& ref = *st;
		if (ref.length() == 0)
			return -1;


		std::string prt;
		int eped = 0;
		for (size_t i = 0; i < ref.length(); i++)
		{
			if (ref[i] == '|' && i > 0)
			{
				prt = ref.substr(eped, i);
				if (prt != "unnamed" && prt.length() > 0)
					break;;
				eped = i + 1;
				i++;
			}
		}

		if (prt.length() == 0)
			return -1;

		std::string num_str = "";
		for (int i = 0; i < prt.length(); i++)
		{
			if (isdigit(prt[i]))
			{
				num_str += prt[i];
			}
			else if (num_str.length() > 0)
			{
				break;
			}
		}

		return stoi(num_str);
	}
private:
	TiffReader *reader = nullptr;
	// tile
	uint tilesCount = 0;
	bool isTiled = true;
};


struct SubImgInfo
{
	int width;
	int height;
};

class TiffReader: public ImageReader
{
	friend class TiffIFD;
	FILE * pFile;
	int compressType = 1;
	RowptrCache cachedTiles;

	bytebuffer tempbuffer;
	std::vector<TiffIFD*> subImages;
	unsigned int currentSubImageIndex = 0;


public:
	const TiffIFD* curSubImage = nullptr;

	inline void setMaxTilesCacheSize(size_t size)
	{
		cachedTiles.setMaxSize(size);
	}

	TiffIFD* getSubImg(int id) const
	{
		return subImages[id];
	}

	void setCurrentSubImage(uint index)
	{
		if (currentSubImageIndex != index)
		{
			cachedRows.clear();
			cachedTiles.clear();
		}
		currentSubImageIndex = index;
		curSubImage = subImages[index];
	}

	int getSubImageSize()
	{
		return subImages.size();
	}

	const TiffTags& getTags()
	{
		return curSubImage->tags;
	}

	int getProjection() const
	{
		return curSubImage->getFirstNormProj();
	}

	std::vector<SubImgInfo> getSumImageInfos()
	{
		std::vector<SubImgInfo> info;
		for(TiffIFD* tfd : subImages)
		{
			info.push_back({ (int)tfd->tags.ImageWidth, (int)tfd->tags.ImageLength });
		}

		return info;
	}

	template<class T>
	T* setData(uchar *in, int len)
	{
		T outT;
		T *out = reinterpret_cast<T*>(in);
		for (int i = 0; i < len; ++i)
		{
			convert(in + i * sizeof(T), outT);
			out[i] = outT;
		}

		return out;
	}

public:
	int MODE = 0;
	TiffReader();

	bool open(const std::string& path) override;
	void close() override;
	virtual ~TiffReader();

private:
//	std::string dectode(uchar* bf, offu64 len);
	int printHeader(uchar *buffer);

	void printIFD(offu64 offset);
	void printBigIFD(size_t offset);

	void read(uchar *buffer, offu64 offset, offu64 len);

	size_t getTagIntValue(size_t offOrValue, size_t count, char format, bool is64);
	rowptr getTiffTileInner(int ind, int samples);
public:
	void setTitleCacheSize(size_t n);
	void setRowsCacheSize(size_t n);

	// ImageReader interface
	rowptr getRowData(int ri) override;
	DataRect getRect(int stX, int stRow, int wid, int hei) override;

	int width() override;
	int height() override;

	ImageType getType() override;
	int getSamples() override;

	cachedRow getTiffTile(int x, int y, bool asConst = false);
	cachedRow getTiffTile(int ind, bool asConst = false);
	void removeTileFromCache(int ind);


	int getTileWid(int rowNum);
	float getFloatFromAscii(size_t offOrValue, size_t count, char format, bool is64);

private:
	rowptr processData(uchar *bytes, int len, int samplesInOne = 1);
protected:
	size_t getTagValue(size_t offOrValue, size_t count, char format, bool is64, ushort tag);
	void readAsciiFromBuffer(std::string &output, int offset, int Count);


	// ImageReader interface
public:
	float getNullValue() override
	{
		return curSubImage->getNullValue();
	}
};

#endif // TIFFREADER_H
