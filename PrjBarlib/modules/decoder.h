#pragma once
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "base.h"

// #define ENABLE_STOPS

using std::cout;

#define CHECKRET                   \
	if (added + 1 >= maxSize)      \
	{                              \
		qDebug() << "Out of tile"; \
		return;                    \
	}

/*
read a character k;
   output k;
   w = k;
   while ( read a character k )    
  // k could be a character or a code.
        {
         if k exists in the dictionary
			entry = dictionary entry for k;
			output entry;
			add w + entry[0] to dictionary;
			w = entry;
         else
			output entry = w + firstCharacterOf(w);
			add entry to dictionary;
			w = entry;
        }

*/
struct stopPair
{
	size_t posInSource;
	size_t posInRes;
};

typedef std::vector<uchar> revbuffer;

class decorder
{
	static const int MIN_BITS = 9;
	static const int CLEAR_CODE = 256; // clear code
	static const int EOI_CODE = 257;   // end of information
	static const int MAX_BYTELENGTH = 12;

	const int st[MAX_BYTELENGTH + 1]{
		1,	  //0
		2,	  //1
		4,	  //2
		8,	  //3
		16,	  //4
		32,	  //5
		64,	  //6
		128,  //7
		256,  //8
		512,  //9
		1024, //10
		2048, //11
		4096  //12
	};

	int comprType;

public:
	size_t arrLen;

	decorder(int compressType) : comprType(compressType)
	{
	}

	ushort getByte3()
	{
		//9 bit
		size_t k = bitPositionInInput / 8; // Get the offset to byte in input array
		size_t l = bitPositionInInput % 8; // Get the bit offset of the byte

		if (k >= arrLen)
		{
			cout << ("ran off the end of the buffer before finding EOI_CODE (end on input code)");
			return EOI_CODE;
		}
		if (l + byteLength <= 16) // l + len <= 16
		{
			//			ushort r2 = ushort(arry[k] << 8 | ((k + 1) < arrLen ? arry[k + 1] : 0));
			ushort r = (ushort(inputArray[k]) << 8) | ushort(inputArray[k + 1]);
			//			if (r2 != r)
			//				qDebug() << "";
			r = r << l; // 0010100010101 -> 1010100000000
			r = r >> (16 - byteLength); // 1010100000000 -> 0000000010101
			return r;
		}
		else
		{
			//			 uint r2 = uint(0 | arry[k] << 16 | ((k + 1)<arrLen? arry[k + 1] << 8 : 0) | ((k + 2) < arrLen? arry[k + 2]: 0));
			uint r = (uint(inputArray[k]) << 16) | (uint(inputArray[k + 1]) << 8) | uint(inputArray[k + 2]);
			//			if (r2 != r)
			//				qDebug() << "";
			r = r << (l + 8);
			r = r >> (32 - byteLength);
			return ushort(r);
		}
	}

	// void swap(uchar&a, uchar&b)
	// {
	// 	uchar temp =a;
	// 	a =b;
	// 	b = temp;
	// }
	revbuffer revTemp;

	uchar getAppedRev(ushort cod2e)
	{
		revTemp.clear();
		for (int i = cod2e; i != 4096; i = dictionaryIndex[i])
		{
			revTemp.push_back(dictionaryChar[i]);
		}
		for (int i = (int)revTemp.size() - 1; i >= 0; --i)
			this->result[++resultSize] = (revTemp[i]);
		return revTemp.back();
	}

	void appendReversed()
	{
		//  maxSize - resultSize -- left to write
		// revTemp.size() - left to write.
		const int minValConst = revTemp.size() - (maxSize - resultSize);
		for (int i = (int)revTemp.size() - 1, minVal = MAX(0, minValConst); i >= minVal; --i)
		{
			result[++resultSize] = (revTemp[i]);
			//			if (resultSize >= 100000)
			//				qDebug() << "Overflow:" << resultSize;
		}
	}

	void getDictionaryReversed(int n)
	{
		revTemp.clear();
		for (int i = n; i != 4096; i = dictionaryIndex[i])
		{
			revTemp.push_back(dictionaryChar[i]);
		}
	}

	inline void initDictionary()
	{
		dictionaryLength = 258;
		byteLength = MIN_BITS;
	}

	void getNext()
	{
		//		if (position == 75953)			qDebug() << "";
		//		const ushort byte2 = getByte(arry, position, byteLength);
		code = getByte3();
		bitPositionInInput += byteLength;
	}

	void addToDictionary(int i, ushort c)
	{
		assert(dictionaryLength < 4093); //864689
		//		if (position == 864689)
		//			qDebug() << "";
		dictionaryChar[dictionaryLength] = c;
		dictionaryIndex[dictionaryLength++] = i;
	}

	//ushort because index always less than 4096
	int dictionaryIndex[4093];

	// short because code can be more than 256
	short dictionaryChar[4093];

	ushort dictionaryLength = 258;
	int byteLength = MIN_BITS;
	size_t bitPositionInInput = 0, maxSize = 0;

	uchar* result;
	size_t resultSize = 0;
	// uchar *result;
	uchar* inputArray;
	short code;
	int debugD = 0;

#ifdef ENABLE_STOPS
	std::vector<stopPair> stops;
public:
	void loadStops(stopPair& _stops, int n)
	{
		stops.insert(stops.begin(), n, _stops);
	}
#endif

public:

	void decompress(uchar* input, offu64 size, vbuffer& _result, size_t bytesPerRow, size_t offset = 0, size_t maxSize = UINT64_MAX)
	{
		_result.allocate(bytesPerRow + 12);
		if (comprType == 1)
		{
			memcpy(_result.data(), input + offset, (maxSize == UINT64_MAX ? size : MIN(maxSize, size)));
			//            result.insert(result.begin(), input, input + size);
			//			output = input;
			return;
		}
#ifdef ENABLE_STOPS
		if (stops.size() > 0)
		{
			size_t i = 0;
			for (i = 0; i < stops.size(); ++i)
			{
				if (stops[i].posInRes > offset)
				{
					break;
				}
			}
			//			resultSize = stops[i - 1].posInRes;
			positionInInput = stops[i - 1].posInSource;
		}
		else
#endif
		{
			if (maxSize == UINT64_MAX)
				maxSize = bytesPerRow;
			this->bitPositionInInput = 0;
		}

		this->resultSize = -1;
		// Чтобы обратиться к текущему индексу, пришлось бы делать resultSize++, т.е.
		// resultSize==0; result[resultSize++] = 1; resultSize==1; <=> resultSize==0; result[0] = 1; resultSize==1;
		// Но resultSize++ -- плохо, поэтому используем хак. Делаем result указателем на -1 элемнет и зменяем resultSize++ на ++resultSize
		// Таким образом получается, что: resultSize==0; result[++resultSize] =1 ; resultSize==1 <=> resultSize==0; result[1] = 1; resultSize==1;
		// result[1] <=> _result[0]
		this->result = _result.data();
		this->inputArray = input;

		this->maxSize = maxSize;
		this->arrLen = size;

		memset(&dictionaryIndex, 0, 4093 * 4);
		memset(&dictionaryChar, 0, 4093 * 2);
		for (ushort i = 0; i <= 257; i++)
		{
			dictionaryIndex[i] = 4096;
			dictionaryChar[i] = i;
		}
		initDictionary();
		getNext();
		short oldCode = 0;

		//		revbuffer val;

		while (code != EOI_CODE)
		{
			if (code == CLEAR_CODE)
			{
				initDictionary();
				getNext();
				while (code == CLEAR_CODE)
				{
					getNext();
				}

#ifdef ENABLE_STOPS
				stops.push_back({ resultSize, positionInInput });
#endif
				if (code == EOI_CODE)
				{
					revTemp.clear();
					break;
				}
				else if (code > CLEAR_CODE)
				{
					revTemp.clear();
					throw std::exception(); //"corrupted code at scanline ${ code }");
				}
				else
				{
					// getAppedRev(code);
					// CHECKRET

					getDictionaryReversed(code);
					appendReversed();
					if (resultSize >= maxSize)
						throw std::exception();
					// return;

					oldCode = code;
				}
			}
			else if (code < dictionaryLength)
			{
				getDictionaryReversed(code);
				appendReversed();
				if (resultSize >= maxSize)
					return;

				addToDictionary(oldCode, revTemp.back());

				// CHECKRET
				oldCode = code;
			}
			else
			{
				getDictionaryReversed(oldCode);

				if (revTemp.empty())
				{
					throw std::exception(); //"Bogus entry.Not in dictionary, ${ oldCode } / ${ dictionaryLength }, position: ${ position }");
				}
				appendReversed();
				result[++resultSize] = revTemp.back();
				if (resultSize >= maxSize)
					return;

				addToDictionary(oldCode, revTemp.back());

				// CHECKRET

				oldCode = code;
			}

			if (dictionaryLength + 1 >= st[byteLength])
			{
				if (byteLength == MAX_BYTELENGTH)
				{
					oldCode = -1;
				}
				else
				{
					++byteLength;
				}
			}
			getNext();
		}
	}
};


	//// A function to decode an LZW encoded string
	//std::string DecodeLZW(const std::string& encoded_str)
	//{
	//	// The dictionary used to store the decoded strings
	//	std::map<uint16_t, std::string> dictionary;

	//	// Initialize the dictionary with the single character strings
	//	for (uint16_t i = 0; i <= 255; ++i)
	//	{
	//		dictionary[i] = std::string(1, static_cast<char>(i));
	//	}

	//	// The current code being decoded
	//	uint16_t code = 0;

	//	// The previous code
	//	uint16_t previous_code = 0;

	//	// The decoded string
	//	std::string decoded_str;

	//	// The string stream used to parse the encoded string
	//	std::stringstream stream(encoded_str);

	//	// Read the first code from the stream
	//	stream >> code;

	//	// While there are more codes to read
	//	while (!stream.eof())
	//	{
	//		// Check if the code is in the dictionary
	//		if (dictionary.count(code) > 0)
	//		{
	//			// The code is in the dictionary, get the corresponding string
	//			decoded_str += dictionary[code];
	//		}
	//		else
	//		{
	//			// The code is not in the dictionary, use the previous code and add a character
	//			decoded_str += dictionary[previous_code] + dictionary[previous_code][0];
	//		}

	//		// Add the previous code and the first character of the current code to the dictionary
	//		dictionary[dictionary.size()] = dictionary[previous_code] + dictionary[code][0];

	//		// Update the previous code
	//		previous_code = code;

	//		// Read the next code from the stream
	//		stream >> code;
	//	}

	//	// Return the decoded string
	//	return decoded_str;
	//}