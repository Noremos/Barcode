#include "pch.h"


#define TIFF_FILE_PATH "../ModuleTests/files/tiff/sample_640×426.tiff"
#define PNG_FILE_PATH "../ModuleTests/files/png/sample_640×426.png"

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

//class TiffReaderOpen : public ::testing::Test
//{
//protected:
//	void SetUp()
//	{
//	}
//};

TEST(TiffReaderOpen, EmptyPath)
{
	TiffReader reader;
	bool isOpened = reader.open("");
	ASSERT_FALSE(isOpened);
}

TEST(TiffReaderOpen, FolderPath)
{
	TiffReader reader;
	bool isOpened = reader.open("../ModuleTests/files");
	ASSERT_FALSE(isOpened);
}

TEST(TiffReaderOpen, NoTiffFilePath)
{
	TiffReader reader;
	bool isOpened = reader.open("../ModuleTests/files/noTiffFile.txt");
	ASSERT_FALSE(isOpened);
}

TEST(TiffReaderOpen, CorrectPath)
{
	TiffReader reader;
	bool isOpened = reader.open("../ModuleTests/files/tiff/sample_640×426.tiff");
	ASSERT_TRUE(isOpened);
}

TEST(TiffReaderTags, TagComparison) // не ясно, как посмотреть эталонные теги
{
	TiffReader reader;
	reader.open(TIFF_FILE_PATH);
	reader.setCurrentSubImage(1);
	// TiffTags& actualTags = reader.getTags();
	// TiffTags expectedTags = 
}

TEST(TiffReaderArea, RegionComparison)
{
	TiffReader reader;
	reader.open(TIFF_FILE_PATH);
	reader.setCurrentSubImage(1);
	DataRect rect = reader.getRect(0, 0, reader.width(), reader.height());
	int width, height, chls;
	unsigned char* image_data = stbi_load(PNG_FILE_PATH, &width, &height, &chls, 0);
	for (uint i = 0; i < reader.width(); i++)
	{
		for (uint j = 0; j < reader.height(); j++)
		{
			bool isEqual = false;
			if (rect.get(i, j) == image_data[i, j])
			{
				isEqual = true;
			}
			ASSERT_TRUE(isEqual);
		}
	}
}

TEST(TiffReaderArea, RowComparison)
{
	TiffReader reader;
	reader.open(TIFF_FILE_PATH);
	reader.setCurrentSubImage(1);
	cachedRow row = reader.getRow(1, false);
	int width, height, chls;
	unsigned char* image_data = stbi_load(PNG_FILE_PATH, &width, &height, &chls, 0);
	for (uint i = 0; i < reader.width(); i++)
	{
		bool isEqual = false;
		if (row[i] == image_data[i, 0]) // не ясно, как получить байтовое значение row
		{
			isEqual = true;
		}
		ASSERT_TRUE(isEqual);
	}
}