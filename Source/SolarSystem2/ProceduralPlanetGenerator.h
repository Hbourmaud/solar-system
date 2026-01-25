#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
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

	UFUNCTION(BlueprintCallable, Category = "Planet Generation")
	void GeneratePlanet();

private:
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FLinearColor> VertexColors;
	TArray<FProcMeshTangent> Tangents;

	void GenerateIcosahedron();

	void SubdivideMesh(int32 SubdivisionLevel);

	void NormalizeVertices();

	void CalculateNormals();

	void CalculateUVs();

	int32 GetMiddlePoint(int32 PointA, int32 PointB, TMap<int64, int32>& MiddlePointCache);
};