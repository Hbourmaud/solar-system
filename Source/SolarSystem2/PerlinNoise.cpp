#include "PerlinNoise.h"

FPerlinNoise::FPerlinNoise()
{
	InitializePermutationTable(91);
}

FPerlinNoise::FPerlinNoise(int32 Seed)
{
	InitializePermutationTable(Seed);
}

void FPerlinNoise::InitializePermutationTable(int32 Seed)
{
	PermutationTable.SetNum(512);

	// TODO: rename
	TArray<int32> P;
	P.SetNum(256);

	for (int32 i = 0; i < 256; ++i)	{
		P[i] = i;
	}

	// TODO : see this 
	FRandomStream RandomStream(Seed);
	for (int32 i = 255; i > 0; i--) {
		int32 SwapIndex = RandomStream.RandRange(0, i);

		int32 Temp = P[i];
		P[i] = P[SwapIndex];
		P[SwapIndex] = Temp;
	}

	for (int32 i = 0; i < 256; i++)	{
		PermutationTable[i] = P[i];
		PermutationTable[256 + i] = P[i];
	}
}

// TODO : explain this
float FPerlinNoise::Fade(float t)
{
	return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

// TODO : explain this
float FPerlinNoise::Lerp(float t, float A, float B)
{
	return A + t * (B - A);
}

// TODO : explain this
float FPerlinNoise::Grad(int32 Hash, float X, float Y)
{
	int32 H = Hash & 7;
	float U = H < 4 ? X : Y;
	float V = H < 4 ? Y : X;

	return ((H & 1) ? -U : U) + ((H & 2) ? -2.0f * V : 2.0f * V);
}

float FPerlinNoise::Noise2D(float X, float Y) const
{
	int32 Xi = FMath::FloorToInt(X) & 255;
	int32 Yi = FMath::FloorToInt(Y) & 255;

	float Xf = X - FMath::FloorToFloat(X);
	float Yf = Y - FMath::FloorToFloat(Y);

	float U = Fade(Xf);
	float V = Fade(Yf);

	int32 AA = PermutationTable[PermutationTable[Xi] + Yi];
	int32 AB = PermutationTable[PermutationTable[Xi] + Yi + 1];
	int32 BA = PermutationTable[PermutationTable[Xi + 1] + Yi];
	int32 BB = PermutationTable[PermutationTable[Xi + 1] + Yi + 1];

	float X1 = Lerp(U, Grad(AA, Xf, Yf), Grad(BA, Xf - 1, Yf));
	float X2 = Lerp(U, Grad(AB, Xf, Yf - 1), Grad(BB, Xf - 1, Yf - 1));

	return Lerp(V, X1, X2);
}

float FPerlinNoise::FractalNoise2D(float X, float Y, int32 Octaves, float Persistence, float Lacunarity) const
{
	float Total = 0.0f;
	float Frequency = 1.0f;
	float Amplitude = 1.0f;
	float MaxValue = 0.0f;

	for (int32 i = 0; i < Octaves; i++) {
		Total += Noise2D(X * Frequency, Y * Frequency) * Amplitude;

		MaxValue += Amplitude;
		Amplitude *= Persistence;
		Frequency *= Lacunarity;
	}

	return Total / MaxValue;
}