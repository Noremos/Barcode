#pragma once
#include "presets.h"
#include "barstrucs.h"
#include "barImg.h"



const float octavesMin = 0.01f;
const float octavesMax = 5.f;

class NN
{
public:
	int width;
	int layers = 3;
	int reserved = 0;
	float** neurons;
	float** bias;
	float*** weights;
	float* octaves;
	int accure;

	Barscalar color;

	void inheritColor(const NN* fs, const NN* sc, bool mutate)
	{
		color = rand() % 2 == 0 ? fs->color : sc->color;
		if (mutate)
		{
			for (char i = 0; i < 3; ++i)
			{
				int ind = rand() % 3;
				if (rand() % 2 == 0)
					color.data.b3[ind] += rand() % 10;
				else
					color.data.b3[ind] -= rand() % 10;

				if (rand() % 2 == 0)
					break;
			}
		}
	}


	void clear()
	{
		for (size_t i = 0; i < layers; i++)
		{
			if (width != 0)
			{
				delete[] neurons[i];
				delete[] bias[i];
				for (size_t j = 0; j < width; j++)
				{
					delete[] weights[i][j];
				}
				delete[] weights[i];
			}
		}
		delete[] neurons;
		delete[] bias;
		//delete[] octaves;
		width = 0;

		neurons = nullptr;
		bias = nullptr;
		weights = nullptr;
		octaves = nullptr;
	}

	void createLayers(int wid, bool init = true)
	{
		clear();

		this->width = wid;
		width += reserved;

		neurons = new float* [layers];
		bias = new float* [layers - 1];
		weights = new float** [layers];
		for (size_t i = 0; i < layers; i++)
		{
			neurons[i] = new float[width];
			bias[i] = new float[width];
			weights[i] = new float* [width];
			for (size_t j = 0; j < width; j++)
			{
				weights[i][j] = new float[width];
			}
		}

		//octaves = new float[width - reserved];
		if (init)
			initRandom();
	}

	NN()
	{
		width = 0;
		neurons = nullptr;
		bias = nullptr;
		weights = nullptr;
		octaves = nullptr;
		color = 0;
		accure = 0;
	}

	~NN()
	{
		clear();
	}

	// init with random weights and biasis
	void initRandom()
	{
		for (int i = 0; i < layers - 1; i++)
		{
			for (int j = 0; j < width; j++)
			{
				bias[i][j] = MRNG::randf(-1.f, 1.f);
			}
		}
		for (int i = 0; i < layers; i++)
		{
			for (int j = 0; j < width; j++)
			{
				for (int k = 0; k < width; k++)
				{
					weights[i][j][k] = MRNG::randf(-1.f, 1.f);
				}
			}
		}
		//for (int i = 0; i < width - reserved; i++)
		//{
		//	float r = MRNG::randf() * MRNG::randf();
		//	r *= octavesMax - octavesMin;
		//	r += octavesMin;
		//	octaves[i] = r;
		//}
	}

	// init by copying
	NN(const NN& a)
	{
		copyFrom(a);
	}

	void copyFrom(const NN& a)
	{
		width = 0;
		neurons = nullptr;
		bias = nullptr;
		weights = nullptr;
		octaves = nullptr;

		createLayers(a.width);

		for (int i = 0; i < layers - 1; i++)
		{
			for (int j = 0; j < width; j++)
			{
				bias[i][j] = a.bias[i][j];
			}
		}
		for (int i = 0; i < layers; i++)
		{
			for (int j = 0; j < width; j++)
			{
				for (int k = 0; k < width; k++)
				{
					weights[i][j][k] = a.weights[i][j][k];
				}
			}
		}
		//for (int i = 0; i < width - reserved; i++)
		//{
		//	octaves[i] = a.octaves[i];
		//}
	}

	// init by crossover
	NN(const NN* a, const NN* b, float rate)
	{
		reinit(a, b, rate);
	}

	void reinit(const NN* a, const NN* b, float rate)
	{
		createLayers(a->width, false);

		for (int i = 0; i < layers - 1; i++)
		{
			for (int j = 0; j < width; j++)
			{
				if (MRNG::randf() < rate)
					bias[i][j] = a->bias[i][j];
				else
					bias[i][j] = b->bias[i][j];
			}
		}
		for (int i = 0; i < layers; i++)
		{
			for (int j = 0; j < width; j++)
			{
				for (int k = 0; k < width; k++)
				{
					if (MRNG::randf() < rate)
						weights[i][j][k] = a->weights[i][j][k];
					else
						weights[i][j][k] = b->weights[i][j][k];
				}
			}
		}
		//for (int i = 0; i < width - reserved; i++)
		//{
		//	if (MRNG::randf() < rate)
		//		octaves[i] = a->octaves[i];
		//	else
		//		octaves[i] = b->octaves[i];
		//}
	}

	void Mutate(float chance, float rate)
	{
		for (int i = 0; i < layers - 1; i++)
		{
			for (int j = 0; j < width; j++)
			{
				if (MRNG::randf() < chance * width)
					bias[i][j] += MRNG::randf(-rate, rate);
				if (bias[i][j] < -1.f)
					bias[i][j] = -1.f;
				else if (bias[i][j] > 1.f)
					bias[i][j] = 1.f;
			}
		}
		for (int i = 0; i < layers; i++)
		{
			for (int j = 0; j < width; j++)
			{
				for (int k = 0; k < width; k++)
				{
					if (MRNG::randf() < chance)
						weights[i][j][k] += MRNG::randf(-rate, rate);
					if (weights[i][j][k] < -1.f)
						weights[i][j][k] = -1.f;
					else if (weights[i][j][k] > 1.f)
						weights[i][j][k] = 1.f;
				}
			}
		}
		//for (int i = 0; i < width - reserved; i++)
		//{
		//	if (MRNG::randf() < chance * width)
		//	{
		//		octaves[i] += MRNG::randf(-rate, rate);
		//		if (octaves[i] < octavesMin)
		//			octaves[i] = octavesMin;
		//		else if (octaves[i] > octavesMax)
		//			octaves[i] = octavesMax;
		//	}
		//}
	}

	// propogation
	bool fit(const float* input)
	{
		for (int i = 0; i < width - reserved; i++)
		{
			neurons[0][i + reserved] = input[i];
		}

		for (int i = 1; i < layers - 1; i++)
		{
			for (int j = 0; j < width; j++) // cur, i' layer
			{
				neurons[i][j] = 0;
				for (int k = 0; k < width; k++)// prev, (i-1)' layer
				{
					neurons[i][j] += neurons[i - 1][k] * weights[i][j][k];
				}
				neurons[i][j] += bias[i - 1][j];
				if (neurons[i][j] < -1.f)
					neurons[i][j] = -1.f;
				else if (neurons[i][j] > 1.f)
					neurons[i][j] = 1.f;
			}
		}

		int lasI = layers - 1;
		float f = 0;
		for (int k = 0; k < width; k++) // prev lasI layer
		{
			f += neurons[lasI - 1][k] * weights[lasI][0][k];
		}
		f += bias[lasI - 1][0];
		if (f < -1.f)
			f = -1.f;
		else if (f > 1.f)
			f = 1.f;

		return f >= 0.5;
	}

};


class CellAutomat
{
	bc::barvector matr;
	Barscalar start; // Non-random
public:
	NN* brain;
	// ---------------

	// Показатели приспобленности
	float accure;
	// ---------------


	CellAutomat()
	{
		accure = 0;

		start = 0;
	}

	CellAutomat(bool onlyUchar)
	{
		accure = 0;
		if (onlyUchar)
			initRandomUchar();
		else
			initRandomVec3b();
	}

	CellAutomat(const bc::barvalue& satrt, const CellAutomat* base, bool onlyUchar)
	{
		accure = 0;
		if (base)
		{
			brain = base->brain;
		}
		else if (onlyUchar)
			initRandomUchar();
		else
			initRandomVec3b();

		//matr.push_back(satrt);
		start = satrt.value;
	}

	void initRandomUchar()
	{
		//brain->createLayers(9);
		initRandomColor();
	}

	void initRandomVec3b()
	{
		//brain.createLayers(9 * 3);
		initRandomColor();
	}

	void initRandomColor()
	{
		brain->color = Barscalar(rand() % 256, rand() % 256, rand() % 256);
	}

	bool coin()
	{
		return rand() % 2 == 0;
	}

	// Living

	bool tryAdd(const bc::barvalue& colval, const const std::array<Barscalar, 9>& zone)
	{
		if (brain->width == 9)
		{
			// uchar
			float vals[9];
			for (char i = 0; i < 9; i++)
			{
				vals[i] = static_cast<float>(zone[i].data.b1) / 255.f;
			}
			if (brain->fit(vals))
			{
				matr.push_back(colval);
				return true;
			}
		}
		else
		{
			float vals[9 * 3];
			int k = 0;
			for (char i = 0; i < 9; i++)
			{
				vals[k++] = static_cast<float>(zone[i].data.b3[0]) / 255.f;
				vals[k++] = static_cast<float>(zone[i].data.b3[1]) / 255.f;
				vals[k++] = static_cast<float>(zone[i].data.b3[2]) / 255.f;
			}
			if (brain->fit(vals))
			{
				//matr.push_back(colval);
				return true;
			}
		}

		return false;
	}


	void clear()
	{
		matr.clear();
	}

	// Inherit



	void mutate(float chance, float rate)
	{
		brain->Mutate(chance, rate);

		for (char i = 0; i < 3; i++)
		{
			int ind = rand() % 3;
			if (coin())
				brain->color.data.b3[ind] += rand() % (int)(40 * rate);
			else
				brain->color.data.b3[ind] -= rand() % (int)(40 * rate);

			if (coin())
				break;
		}

	}

	static void combineOne(const CellAutomat& fs, const CellAutomat& sc, CellAutomat& outChild)
	{
		int k = 0;
		delete outChild.brain;
		outChild.brain = new NN(fs.brain, sc.brain, 0.5f);
		outChild.brain->inheritColor(fs.brain, sc.brain, true);
	}
};
