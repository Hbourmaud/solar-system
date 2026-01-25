#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralPlanetGenerator.h"
#include "CelestialBody.generated.h"

UCLASS()
class SOLARSYSTEM2_API ACelestialBody : public AActor
{
    GENERATED_BODY()

public:
    ACelestialBody();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Celestial Body")
    FString BodyName = "Unnamed";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Celestial Body")
    float Mass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Celestial Body")
    float Radius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Celestial Body")
    float SurfaceGravity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Celestial Body")
    FVector InitialVelocity;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Celestial Body")
    FVector CurrentVelocity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    UStaticMeshComponent* MeshComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    UProceduralPlanetGenerator* ProceduralMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    FLinearColor OrbitColor = FLinearColor::White;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    UMaterialInterface* PlanetMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    float VisualScale = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Generation")
    int32 PlanetSubdivisions = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Generation")
    bool UseProcedural = true;

    UFUNCTION(BlueprintCallable, Category = "Celestial Body")
    void RegeneratePlanet();

    UFUNCTION(BlueprintCallable, Category = "Celestial Body")
    void CalculateMassFromGravity();

    void ApplyGravitationalForce(const FVector& Force, float DeltaTime);

    void UpdatePosition(float DeltaTime);

    virtual void Tick(float DeltaTime) override;

protected:
    virtual void BeginPlay() override;

private:
    FVector AccumulatedAcceleration;
};