#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "PerlinNoise.h"
#include "ProceduralPlanetGenerator.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SOLARSYSTEM2_API UProceduralPlanetGenerator : public UProceduralMeshComponent
{
	GENERATED_BODY()

public:
	UProceduralPlanetGenerator(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet Generation")
	float Radius = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet Generation", meta = (ClampMin = "0", ClampMax = "5"))
	int32 Subdivisions = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet Generation")
	bool SmoothShading = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Noise")
	bool ApplyNoise = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Noise", meta = (ToolTip = "Frequency / zoom of noise pattern"))
	float NoiseScale = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Noise", meta = (ToolTip = "Tall/Deep terrain"))
	float NoiseHeightMultiplier = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Noise", meta = (ToolTip = "Details terrain"))
	int32 NoiseOctaves = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Noise", meta = (ToolTip = "Roughness terrain"))
	float NoisePersistence = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Noise", meta = (ToolTip = "Space between details"))
	float NoiseLacunarity = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Noise")
	int32 NoiseSeed = 91;

	UFUNCTION(BlueprintCallable, Category = "Planet Generation")
	void GeneratePlanet();

private:
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FLinearColor> VertexColors;
	TArray<FProcMeshTangent> Tangents;

	TUniquePtr<FPerlinNoise> NoiseGenerator;

	void GenerateIcosahedron();
	void SubdivideMesh(int32 SubdivisionLevel);

	void NormalizeVertices();
	void CalculateNormals();
	void CalculateUVs();

	void ApplyNoiseToVertices();

	int32 GetMiddlePoint(int32 PointA, int32 PointB, TMap<int64, int32>& MiddlePointCache);
};