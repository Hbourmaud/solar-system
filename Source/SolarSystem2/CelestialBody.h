#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
    FLinearColor OrbitColor = FLinearColor::White;

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