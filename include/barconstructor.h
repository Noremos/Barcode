#pragma once
#include <vector>
#include <barcontainer.h>
namespace bc
{

	enum class ComponentType { Component, Hole, FullPrepair, PrepairComp };
	enum class ProcType { f0t255, f255t0, experement };
	enum class ColorType { gray, rgb, native };

	enum class ReturnType { betty, barcode2d, barcode3d };


	struct barstruct
	{
		ComponentType comtype;
		ProcType proctype;
		ColorType coltype;
		barstruct()
		{
			comtype = ComponentType::Hole;
			proctype = ProcType::f0t255;
			coltype = ColorType::gray;
		}
		barstruct(ProcType pt, ColorType colT, ComponentType comT)
		{
			this->comtype = comT;
			this->proctype = pt;
			this->coltype = colT;
		}
	};

	template<class T>
	class BarConstructor
	{
		CachedValue<T> settStep;
	public:
		T foneStart;
		T foneEnd;

#ifdef USE_OPENCV
		bool visualize = false;
#endif // USE_OPENCV

		bool createGraph = false;
		bool createBinayMasks = false;
		ReturnType returnType;
		CachedValue<T> maxTypeValue;
		std::vector<barstruct> structure;

		void checkCorrect()
		{
			if (createGraph && !createBinayMasks)
				throw std::exception();

			if (returnType == ReturnType::betty && T != uchar)
				throw std::exception();

			if (structure.size()==0)
				throw std::exception();

			getStep();
		}

		T getStep()
		{
			return settStep.getOrDefault(1);
		}
		void setStep(T val)
		{
			settStep.set(val);
		}
		void setFoneRange(T st, T ed)
		{
			foneStart = st;
			foneEnd = ed;
		}
	};
}
