#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CelestialBody.h"
#include "SolarSystemManager.generated.h"

UCLASS()
class SOLARSYSTEM2_API ASolarySystemManager : public AActor
{
	GENERATED_BODY()

public:
	ASolarySystemManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Solar system")
	TArray<ACelestialBody*> CelestialBodies;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Solar system")
	float TimeScale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool drawOrbits = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	int32 OrbitSimulationSteps = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	float OrbitSimulationTimeStep = 0.1f;

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	const float G = 0.0000000000674f;

	FVector CalculateGravitationalForce(ACelestialBody* BodyA, ACelestialBody* OtherBody);

	void UpdateGravitationalForces(float DeltaTime);

	void SimulateOrbits();
};