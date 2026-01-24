#include "SolarSystemManager.h"
#include "DrawDebugHelpers.h"
#include <Kismet/GameplayStatics.h>

ASolarySystemManager::ASolarySystemManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASolarySystemManager::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundBodies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACelestialBody::StaticClass(), FoundBodies);


	for (AActor* Actor : FoundBodies) {
		if (ACelestialBody* Body = Cast<ACelestialBody>(Actor)) {
			CelestialBodies.Add(Body);
			UE_LOG(LogTemp, Warning, TEXT("Found Body: %s | Pos: %s | Mass: %.2e | Velocity: %s"),
				*Body->GetName(),
				*Body->GetActorLocation().ToString(),
				Body->Mass,
				*Body->CurrentVelocity.ToString());
		}
	}
}

void ASolarySystemManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float ScaledDeltaTime = DeltaTime * TimeScale;

	UpdateGravitationalForces(ScaledDeltaTime);

	for (ACelestialBody* Body : CelestialBodies) {
		if (Body) {
			Body->UpdatePosition(ScaledDeltaTime);
		}
	}

	if (drawOrbits) {
		SimulateOrbits();
	}
}

FVector ASolarySystemManager::CalculateGravitationalForce(ACelestialBody* Body, ACelestialBody* OtherBody)
{
	FVector Direction = OtherBody->GetActorLocation() - Body->GetActorLocation();
	float Distance = Direction.Size();

	if (Distance < 1.0f) {
		return FVector::ZeroVector;
	}

	FVector Fdir = Direction.GetSafeNormal();

	float ForceMagnitude = G * (Body->Mass * OtherBody->Mass) / (Distance * Distance);

	return Fdir * ForceMagnitude;
}

void ASolarySystemManager::UpdateGravitationalForces(float DeltaTime)
{
	for (int32 i = 0; i < CelestialBodies.Num(); ++i){
		for (int32 j = i + 1; j < CelestialBodies.Num(); j++) {
			ACelestialBody* Body = CelestialBodies[i];
			ACelestialBody* OtherBody = CelestialBodies[j];

			if (Body && OtherBody) {
				FVector Force = CalculateGravitationalForce(Body, OtherBody);

				Body->ApplyGravitationalForce(Force, DeltaTime);
				OtherBody->ApplyGravitationalForce(-Force, DeltaTime);
			}
		}
	}
}

void ASolarySystemManager::SimulateOrbits()
{
	for (ACelestialBody* Body : CelestialBodies) {
		if (!Body || Body->CurrentVelocity.SizeSquared() < 0.1f) {
			continue;
		}

		FVector OriginalPosition = Body->GetActorLocation();
		FVector OriginalVelocity = Body->CurrentVelocity;

		TArray<FVector> TempPositions;
		TArray<FVector> TempVelocities;
		TArray<float> TempMass;

		for (ACelestialBody* OtherBody : CelestialBodies) {
			if (OtherBody) {
				TempPositions.Add(OtherBody->GetActorLocation());
				TempVelocities.Add(OtherBody->CurrentVelocity);
				TempMass.Add(OtherBody->Mass);
			}
		}

		TArray<FVector> OrbitPoints;
		OrbitPoints.Add(OriginalPosition);

		int32 BodyIndex = CelestialBodies.IndexOfByKey(Body);

		for (int32 step = 0; step < OrbitSimulationSteps; ++step) {
			TArray<FVector> Accelerations;
			Accelerations.SetNum(TempPositions.Num());

			for (int32 i = 0; i < TempPositions.Num(); ++i) {
				if (!CelestialBodies[i]) {
					continue;
				}

				FVector TotalAcceleration = FVector::ZeroVector;

				for (int32 j = 0; j < CelestialBodies.Num(); ++j) {
					if (i == j || !CelestialBodies[j]) {
						continue;
					}

					FVector Direction = TempPositions[j] - TempPositions[i];
					float Distance = Direction.Size();

					if (Distance > 1.0f) {
						FVector Fdir = Direction.GetSafeNormal();
						float ForceMagnitude = G * (TempMass[i] * TempMass[j]) / (Distance * Distance);
						FVector Force = Fdir * ForceMagnitude;

						TotalAcceleration += Force / TempMass[i];
					}
				}

				Accelerations[i] = TotalAcceleration;
			}

			for (int32 i = 0; i < CelestialBodies.Num(); ++i) {
				if (!CelestialBodies[i]) {
					continue;
				}

				TempVelocities[i] += Accelerations[i] * OrbitSimulationTimeStep;
				TempPositions[i] += TempVelocities[i] * OrbitSimulationTimeStep;
			}

			OrbitPoints.Add(TempPositions[BodyIndex]);
		}

		float TotalDistance = 0.0f;
		for (int32 k = 0; k < OrbitPoints.Num() - 1; ++k) {
			TotalDistance += FVector::Dist(OrbitPoints[k], OrbitPoints[k + 1]);
		}

		UE_LOG(LogTemp, Warning, TEXT("Orbit for %s: Points=%d, TotalDist=%.2f, AvgDist=%.2f, Start=%s, End=%s"),
			*Body->GetName(),
			OrbitPoints.Num(),
			TotalDistance,
			TotalDistance / (OrbitPoints.Num() - 1),
			*OrbitPoints[0].ToString(),
			*OrbitPoints.Last().ToString());

		if (drawOrbits) {
			FColor OrbitColor = FColor::Green;

			for (int32 k = 0; k < OrbitPoints.Num() - 1; ++k) {
				DrawDebugLine(
					GetWorld(),
					OrbitPoints[k],
					OrbitPoints[k + 1],
					OrbitColor,
					false,
					0.016f,
					0,
					10.0f
				);
			}

			DrawDebugSphere(GetWorld(), OrbitPoints[0], 50.0f, 12, FColor::Blue, false, 0.016f);
			DrawDebugSphere(GetWorld(), OrbitPoints.Last(), 50.0f, 12, FColor::Red, false, 0.016f);
		}

		Body->SetActorLocation(OriginalPosition);
		Body->CurrentVelocity = OriginalVelocity;
	}
}