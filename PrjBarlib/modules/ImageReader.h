#include "ReaderTypes.h"


class ImageReader
{
public:
	virtual rowptr getRowData(int ri) = 0;
	virtual bool open(const char* path) = 0;
	virtual void close() = 0;
	virtual ImageType getType() = 0;
	virtual int getSamples() = 0;
	virtual int widght() = 0;
	virtual int height() = 0;
	RowptrCache cachedRows;
	bool ready = false;
	bool isTile = true;
	//	float max, min;

	virtual float getNullValue() = 0;
	cachedRow getRow(int i, bool)
	{
		RowPtrHolder* nulld = nullptr;
		RowPtrHolder* data = cachedRows.getData(i, nulld);
		if (data == nullptr)
		{
			rowptr row = getRowData(i);
			data = new RowPtrHolder(row);
			cachedRows.storeData(i, data);
		}

		return data;
	}


	virtual DataRect getRect(int stX, int stRow, int wid, int hei) = 0;

	virtual ~ImageReader()
	{
		cachedRows.clear();
	}
};
