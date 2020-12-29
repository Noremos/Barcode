#pragma once
#include <vector>
#include <barcontainer.h>
namespace bc
{

	enum class ComponentType { Component, Hole, FullPrepair, PrepairComp };
	enum class ProcType { f0t255, f255t0, experement };
	enum class ColorType { gray, rgb, native };


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

	class BarConstructor
	{
	private:
		union imgval
		{
			float f;
			int i;
			uchar b;
		};
		imgval foneStart;
		imgval foneEnd;
	public:
		std::vector<barstruct> structure;
		
		void setFoneRange(float st, float ed)
		{
			foneStart.f = st;
			foneEnd.f = ed;
		}
	};
}
