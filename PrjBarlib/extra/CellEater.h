#pragma once

#include <functional>
#include <memory>
#include <unordered_set>

import BarcodeModule;

namespace bc
{
	struct Region
	{
		// point prev;
		float score = -1;
		bc::barline* line;
		point prev;

		void init(Region& prevReg, point prevP, float scr)
		{
			score = scr;
			line = prevReg.line;
			prev = prevP;
		}


		Region(bc::barline* st = nullptr, float scr = -1) :
			score(scr), line(st)
		{ }

		bool hasPrev()
		{
			return prev.x != -1 && prev.y != -1;
		}

		bool inited()
		{
			return score >= 0;
		}
	};

	class Eater
	{
		std::vector<poidex> sorted;
		// barmapHash<poidex, Region> grows;
		// barmapHash<poidex, Region> newGrows;
		std::vector<bc::barline*> lines;

		// std::unique_ptr<poidex> sorted;

		// std::vector<Region> closed;
		// std::unique_ptr<Region[]> field;
		std::vector<Region> field;
		DatagridProvider* input;

		// Seeding
		// void seedPoint(point p)
		// {
		// 	Barscalar value = input->get(p.x, p.y);
		// 	// result.push_back(new Region(value));
		// 	grows[p.getLiner(nput->wid())] = Region(value);
		// 	field[p.getLiner(input->wid())] = Region(value);
		// }


		// Growing
		constexpr bool IS_OUT_OF_REG(int x, int y) const
		{
			return x < 0 || y < 0 || x >= input->wid() || y >= input->hei();
		}

		void growSeed(point p, Region& r)
		{
			const char poss[9][2] = { { -1,0 },{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 },{ -1,0 } };

			bool added = false;
			int minInd = -1;
			Barscalar pointValue = input->get(p.x, p.y);
			// Barscalar minDiff = pointValue;
			for (int i = 0; i < 8; ++i)
			{
				const point IcurPoint(p + poss[i]);

				if (IS_OUT_OF_REG(IcurPoint.x, IcurPoint.y))
					continue;

				const poidex Icurpoindex = IcurPoint.getLiner(input->wid());

				Barscalar offValue = input->get(IcurPoint.x, IcurPoint.y);
				float diff = offValue.absDiff(pointValue).getAvgFloat();

				auto& existsR = field[Icurpoindex];
				if (existsR.inited()) //  Если
				{
					if (existsR.score > r.score + diff)
					{
						field[Icurpoindex].init(r, p, r.score + diff);
					}
				}
				else
				{
					field[Icurpoindex].init(r, p, r.score + diff);
				}

				// if (diff < minDiff)
				// {
				// 	minInd = 0;
				// 	minDiff = diff;
				// }
				// else if (offValue == pointValue)
				// {
				// newGrows[IcurPoint.getLiner(input->wid())] = Region(p, r.start, r.score + diff.getAvgFloat());
				// field[IcurPoint.getLiner(input->wid())] = Region(p, r.start, r.score + diff);
				// added = true;
				// }
			}

			// if (minInd != -1)
			// {
			// 	newGrows[(p + poss[minInd]).getLiner(input->wid())] = Region(p, r.start, r.score + minDiff);
			// 	field[IcurPoint.getLiner(input->wid())] = Region(p, r.start, r.score);
			// }
			// else if (!added)
			// {
			// 	closed.push_back(r);
			// }
		}

		// void growField()
		// {
		// 	for (auto &g : grows)
		// 	{
		// 		growSeed(g.first, g.second);
		// 	}

		// 	grows = std::move(newGrows);
		// }


		void sort()
		{
			uint hist[256];//256
			uint offs[256];//256
			std::fill_n(hist, 256, 0);
			std::fill_n(offs, 256, 0);

			for (int j = 0; j < input->hei(); ++j)//hei
			{
				for (int i = 0; i < input->wid(); ++i)//wid
				{
					auto p = (int)input->get(i, j);
					++hist[p];//можно vector, но хз
				}
			}

			for (size_t i = 1; i < 256; ++i)
			{
				hist[i] += hist[i - 1];
				offs[i] = hist[i - 1];
			}
			size_t totalSize = input->length();
			sorted.resize(totalSize);
			for (size_t i = 0; i < totalSize; i++)
			{
				uchar p = input->getLiner(i).getAvgUchar();
				sorted[offs[p]++] = i;
			}
		}

		void growSortedSeeds()
		{
			for (auto& pindex : sorted)
			{
				point p = barvalue::getStatPoint(pindex, input->wid());
				auto& existsR = field[pindex];
				if (!existsR.inited())
				{
					Barscalar val = input->getLiner(pindex);
					field[pindex] = Region(nullptr, 0);
				}

				Region& r = field[pindex];
				growSeed(p, r);
			}
		}

		void harvest()
		{
			std::unordered_set<poidex> exs;
			poidex totalSize = input->length();
			for (poidex i = 0; i < totalSize; i++)
			{
				exs.insert(i);
			}

			for (size_t i = 0; i < field.size(); i++)
			{
				if (field[i].hasPrev())
				{
					auto p = field[i].prev;
					exs.erase(barvalue::getStatInd(p.x, p.y, input->wid()));
				}
			}

			for (auto &e : exs)
			{
				int curPoin = e;
				Barscalar val = input->getLiner(curPoin);
				lines.push_back(new bc::barline(val, val, input->wid()));
				auto& matr = lines.back()->matr;
				Region* r;

				while (true)
				{
					point p = barvalue::getStatPoint(curPoin, input->wid());
					val = input->getLiner(curPoin);
					matr.push_back(bc::barvalue(p, val));

				 	r = &field[curPoin];
					if (!r->hasPrev())
						break;

					curPoin = barvalue::getStatInd(r->prev.x, r->prev.y, input->wid());
				}
			}

		}

		Baritem* proc(DatagridProvider* inImg)
		{
			this->input = inImg;
			sort();
			field.resize(input->length());
			growSortedSeeds();
			harvest();
			Baritem* item = new Baritem(input->wid(), input->getType());
			for (auto* line : lines)
			{
				if (line->matr.size() > 0)
					item->barlines.push_back(line);
				else
					delete line;
			}
			lines.clear();

			return item;
		}

	public:
		static Baritem* createBarcode(DatagridProvider* input)
		{
			Eater e;
			return e.proc(input);
		}
	};
}
