#pragma once

#include "CoreMinimal.h"

class SOLARSYSTEM2_API FPerlinNoise
{
public:
	FPerlinNoise();
	FPerlinNoise(int32 Seed);

	float Noise2D(float x, float y) const;

	float FractalNoise2D(float X, float Y, int32 Octaves = 4, float Persistence = 0.5f, float Lacunarity = 2.0f) const;

private:
	TArray<int32> PermutationTable;

	void InitializePermutationTable(int32 Seed);

	// t = time ?
	static float Fade(float t);

	static float Lerp(float t, float A, float B);

	static float Grad(int32 Hash, float X, float Y);
};