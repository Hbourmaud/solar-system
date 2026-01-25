#include "ProceduralPlanetGenerator.h"

UProceduralPlanetGenerator::UProceduralPlanetGenerator(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UProceduralPlanetGenerator::GeneratePlanet()
{
	NoiseGenerator = MakeUnique<FPerlinNoise>(NoiseSeed);

	Vertices.Empty();
	Triangles.Empty();
	Normals.Empty();
	UVs.Empty();
	VertexColors.Empty();
	Tangents.Empty();

	GenerateIcosahedron();
	SubdivideMesh(Subdivisions);
	NormalizeVertices();

	if (ApplyNoise) {
		ApplyNoiseToVertices();
	}

	CalculateNormals();
	CalculateUVs();

	CreateMeshSection_LinearColor(
		0,
		Vertices,
		Triangles,
		Normals,
		UVs,
		VertexColors,
		Tangents,
		true
	);

	SetVisibility(true);
	SetHiddenInGame(false);

	UE_LOG(LogTemp, Log, TEXT("Generated planet with %d vertices and %d triangles (Noise: %s)"),
		Vertices.Num(), Triangles.Num() / 3, ApplyNoise ? TEXT("ON") : TEXT("OFF"));
}

void UProceduralPlanetGenerator::ApplyNoiseToVertices()
{
	if (!NoiseGenerator.IsValid()) {
		return;
	}

	for (int32 i = 0; i < Vertices.Num(); i++) {
		FVector Point = Vertices[i].GetSafeNormal();

		float NoiseValue = NoiseGenerator->FractalNoise2D(
			Point.X * NoiseScale,
			Point.Y * NoiseScale,
			NoiseOctaves,
			NoisePersistence,
			NoiseLacunarity
		);

		float HeightOffset = (NoiseValue + 1.0f) * 0.5f;
		float FinalRadius = Radius * (1.0f + HeightOffset * NoiseHeightMultiplier);

		Vertices[i] = Point * FinalRadius;
	}

	UE_LOG(LogTemp, Log, TEXT("Applied noise to %d vertices (Scale=%.2f, Height=%.2f, Octaves=%d)"),
		Vertices.Num(), NoiseScale, NoiseHeightMultiplier, NoiseOctaves);
}

void UProceduralPlanetGenerator::GenerateIcosahedron()
{
	const float Phi = (1.0f + FMath::Sqrt(5.0f)) / 2.0f;
	const float Scale = 1.0f / FMath::Sqrt(Phi * Phi + 1.0f);

	Vertices.Add(FVector(-1, Phi, 0) * Scale);
	Vertices.Add(FVector(1, Phi, 0) * Scale);
	Vertices.Add(FVector(-1, -Phi, 0) * Scale);
	Vertices.Add(FVector(1, -Phi, 0) * Scale);

	Vertices.Add(FVector(0, -1, Phi) * Scale);
	Vertices.Add(FVector(0, 1, Phi) * Scale);
	Vertices.Add(FVector(0, -1, -Phi) * Scale);
	Vertices.Add(FVector(0, 1, -Phi) * Scale);

	Vertices.Add(FVector(Phi, 0, -1) * Scale);
	Vertices.Add(FVector(Phi, 0, 1) * Scale);
	Vertices.Add(FVector(-Phi, 0, -1) * Scale);
	Vertices.Add(FVector(-Phi, 0, 1) * Scale);

	Triangles.Append({ 0, 11, 5 });
	Triangles.Append({ 0, 5, 1 });
	Triangles.Append({ 0, 1, 7 });
	Triangles.Append({ 0, 7, 10 });
	Triangles.Append({ 0, 10, 11 });

	Triangles.Append({ 1, 5, 9 });
	Triangles.Append({ 5, 11, 4 });
	Triangles.Append({ 11, 10, 2 });
	Triangles.Append({ 10, 7, 6 });
	Triangles.Append({ 7, 1, 8 });

	Triangles.Append({ 3, 9, 4 });
	Triangles.Append({ 3, 4, 2 });
	Triangles.Append({ 3, 2, 6 });
	Triangles.Append({ 3, 6, 8 });
	Triangles.Append({ 3, 8, 9 });

	Triangles.Append({ 4, 9, 5 });
	Triangles.Append({ 2, 4, 11 });
	Triangles.Append({ 6, 2, 10 });
	Triangles.Append({ 8, 6, 7 });
	Triangles.Append({ 9, 8, 1 });
}

int32 UProceduralPlanetGenerator::GetMiddlePoint(int32 PointA, int32 PointB, TMap<int64, int32>& MiddlePointCache)
{
	bool FirstIsSmaller = PointA < PointB;
	int64 SmallerIndex = FirstIsSmaller ? PointA : PointB;
	int64 GreaterIndex = FirstIsSmaller ? PointB : PointA;
	int64 Key = (SmallerIndex << 32) + GreaterIndex;

	if (MiddlePointCache.Contains(Key))	{
		return MiddlePointCache[Key];
	}

	FVector LocalPointA = Vertices[PointA];
	FVector LocalPointB = Vertices[PointB];
	FVector Middle = (LocalPointA + LocalPointB) / 2.0f;

	int32 Index = Vertices.Add(Middle);
	MiddlePointCache.Add(Key, Index);

	return Index;
}

void UProceduralPlanetGenerator::SubdivideMesh(int32 SubdivisionLevel)
{
	TMap<int64, int32> MiddlePointCache;

	for (int32 level = 0; level < SubdivisionLevel; level++) {
		TArray<int32> NewTriangles;
		MiddlePointCache.Empty();

		for (int32 i = 0; i < Triangles.Num(); i += 3) {
			int32 LocalPointA = Triangles[i];
			int32 LocalPointB = Triangles[i + 1];
			int32 LocalPointC = Triangles[i + 2];

			// TODO: rename
			int32 A = GetMiddlePoint(LocalPointA, LocalPointB, MiddlePointCache);
			int32 B = GetMiddlePoint(LocalPointB, LocalPointC, MiddlePointCache);
			int32 C = GetMiddlePoint(LocalPointC, LocalPointA, MiddlePointCache);

			NewTriangles.Append({ LocalPointA, A, C });
			NewTriangles.Append({ LocalPointB, B, A });
			NewTriangles.Append({ LocalPointC, C, B });
			NewTriangles.Append({ A, B, C });
		}

		Triangles = MoveTemp(NewTriangles);
	}
}

void UProceduralPlanetGenerator::NormalizeVertices()
{
	for (int32 i =0; i < Vertices.Num(); i++) {
		FVector Normalized = Vertices[i].GetSafeNormal();

		Vertices[i] = Normalized * Radius;
	}
}

void UProceduralPlanetGenerator::CalculateNormals()
{
	Normals.SetNum(Vertices.Num());

	if (SmoothShading) {
		for (int32 i = 0; i < Vertices.Num(); i++) {
			Normals[i] = Vertices[i].GetSafeNormal();
		}
	} else {
		TArray<int32> NormalCount;
		NormalCount.SetNumZeroed(Vertices.Num());

		for (int32 i = 0; i < Normals.Num(); i += 3) {
			Normals[i] = FVector::ZeroVector;
		}

		for (int32 i = 0; i < Triangles.Num(); i += 3) {
			// TODO: rename
			int32 I1 = Triangles[i];
			int32 I2 = Triangles[i + 1];
			int32 I3 = Triangles[i + 2];

			FVector V1 = Vertices[I1];
			FVector V2 = Vertices[I2];
			FVector V3 = Vertices[I3];

			FVector Edge1 = V2 - V1;
			FVector Edge2 = V3 - V1;
			FVector FaceNormal = FVector::CrossProduct(Edge1, Edge2).GetSafeNormal();

			Normals[I1] += FaceNormal;
			Normals[I2] += FaceNormal;
			Normals[I3] += FaceNormal;

			NormalCount[I1]++;
			NormalCount[I2]++;
			NormalCount[I3]++;
		}

		for (int32 i = 0; i < Normals.Num(); i++) {
			if (NormalCount[i] > 0) {
				Normals[i] = (Normals[i] / NormalCount[i]).GetSafeNormal();
			}
		}
	}
}

void UProceduralPlanetGenerator::CalculateUVs()
{
	UVs.SetNum(Vertices.Num());

	for (int32 i = 0; i < Vertices.Num(); i++) {
		FVector Normal = Vertices[i].GetSafeNormal();

		float U = 0.5f + (FMath::Atan2(Normal.Y, Normal.X) / (2.0f * PI));
		float V = 0.5f - (FMath::Asin(Normal.Z) / PI);
		UVs[i] = FVector2D(U, V);
	}
}