//module;
//#include "../include/barline.h"
//#include "StateBinFile.h"
//
//export module Bartypes;
//
//export
//{
//	BarType;
//	Barscalar;
//	//using ::uchar;
//	//using ::uint;
//}
//
//export namespace StateBinFile
//{
//	BinState;
//	BinStateReader;
//	BinStateWriter;
//}
//
//export namespace bc
//{
//	const unsigned int BAR_MAX_WID = 65535;
//
//	poidex;
//
//	point;
//	pointHash;
//	pmap;
//
//	CachedValue;
//	BarConstructor;
//	DatagridProvider;
//	BarRect;
//	barline;
//	using bc::barlinevector;
//	using bc::BarRoot;
//
//	using bc::barvector;
//	using bc::barcounter;
//	CachedValue;
//
//	barstruct;
//
//	CompireStrategy;
//	ComponentType;
//	ProcType; 
//	ColorType;
//	ReturnType;
//	ReturnType;
//	AttachMode;
//	ProcessStrategy;
//
//	BarRect;
//	bc::BarRect getBarRect(const bc::barvector& matrix)
//	{
//		int l, r, t, d;
//		r = l = matrix[0].getX();
//		t = d = matrix[0].getY();
//		for (size_t j = 0; j < matrix.size(); ++j)
//		{
//			if (l > matrix[j].getX())
//				l = matrix[j].getX();
//			if (r < matrix[j].getX())
//				r = matrix[j].getX();
//
//			if (t > matrix[j].getY())
//				t = matrix[j].getY();
//			if (d < matrix[j].getY())
//				d = matrix[j].getY();
//		}
//		return bc::BarRect(l, t, r - l + 1, d - t + 1);
//	}
//}
