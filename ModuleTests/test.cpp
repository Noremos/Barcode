#include "pch.h"


TEST(TittReaderOpen, InvalidPath)
{
	TiffReader reader;
	bool opened = reader.open("");
	ASSERT_TRUE(!opened);
}