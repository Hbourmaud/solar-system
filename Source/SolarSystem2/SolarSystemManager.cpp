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
			UE_LOG(LogTemp, Warning, TEXT("Found Body: %s | Pos: %s | Mass: %.2e | Velocity: %s | VelMag: %.4f"),
				*Body->BodyName,
				*Body->GetActorLocation().ToString(),
				Body->Mass,
				*Body->CurrentVelocity.ToString(),
				Body->CurrentVelocity.Size());
		}
	}

	for (ACelestialBody* Body : CelestialBodies) {
		if (Body && Body->CurrentVelocity.SizeSquared() > 0.01f) {
			ACelestialBody* Sun = nullptr;
			float LargestMass = 0.0f;
			for (ACelestialBody* OtherBody : CelestialBodies) {
				if (OtherBody && OtherBody != Body && OtherBody->CurrentVelocity.SizeSquared() < 0.1f && OtherBody->Mass > LargestMass) {
					Sun = OtherBody;
					LargestMass = OtherBody->Mass;
				}
			}

			if (Sun) {
				float Distance = FVector::Dist(Body->GetActorLocation(), Sun->GetActorLocation());
				float CurrentSpeed = Body->CurrentVelocity.Size();
				float ExpectedSpeed = FMath::Sqrt(G * Sun->Mass / Distance);
				float SpeedRatio = CurrentSpeed / ExpectedSpeed;

				UE_LOG(LogTemp, Error, TEXT("ORBIT CHECK - %s: Distance=%.2f, CurrentSpeed=%.4f, ExpectedSpeed=%.4f, Ratio=%.4f, SunMass=%.2e"),
					*Body->BodyName, Distance, CurrentSpeed, ExpectedSpeed, SpeedRatio, Sun->Mass);
			}
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
		if (!Body || Body->CurrentVelocity.SizeSquared() < 0.01f) {
			continue;
		}

		FVector OriginalPosition = Body->GetActorLocation();
		FVector OriginalVelocity = Body->CurrentVelocity;

		ACelestialBody* CentralBody = nullptr;
		float LargestMass = 0.0f;

		for (ACelestialBody* OtherBody : CelestialBodies) {
			if (OtherBody && OtherBody != Body && OtherBody->Mass > LargestMass && OtherBody->CurrentVelocity.SizeSquared() < 0.1f) {
				CentralBody = OtherBody;
				LargestMass = OtherBody->Mass;
			}
		}

		if (!CentralBody) {
			UE_LOG(LogTemp, Warning, TEXT("No central body found for %s, skipping orbit"), *Body->BodyName);
			continue;
		}

		UE_LOG(LogTemp, Log, TEXT("Simulating orbit for %s around %s"), *Body->BodyName, *CentralBody->BodyName);

		float Distance = FVector::Dist(OriginalPosition, CentralBody->GetActorLocation());
		float Speed = Body->CurrentVelocity.Size();

		if (Distance < 1.0f || Speed < 0.1f) {
			UE_LOG(LogTemp, Warning, TEXT("Invalid orbit parameters for %s: Dist=%.2f, Speed=%.2f"), *Body->BodyName, Distance, Speed);
			continue;
		}

		float EstimatePeriod = (2.0f * PI * Distance) / Speed;

		int32 DynamicSteps = FMath::Clamp(FMath::CeilToInt(EstimatePeriod / 10.0f), 200, 2000);
		float SimulationTime = EstimatePeriod * 1.2f;
		float DynamicTimeStep = SimulationTime / DynamicSteps;

		DynamicTimeStep = FMath::Clamp(DynamicTimeStep, 0.1f, 50.0f);

		UE_LOG(LogTemp, Log, TEXT("Orbit calc for %s: Dist=%.2f, Speed=%.2f, Period=%.2f, Steps=%d, TimeStep=%.2f"),
			*Body->BodyName, Distance, Speed, EstimatePeriod, DynamicSteps, DynamicTimeStep);

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
		bool bOrbitUnstable = false;

		for (int32 step = 0; step < DynamicSteps; ++step) {
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
					float Dist = Direction.Size();

					if (Dist > 1.0f) {
						FVector Fdir = Direction.GetSafeNormal();
						float AccelerationMagnitude = G * TempMass[j] / (Dist * Dist);
						FVector Acceleration = Fdir * AccelerationMagnitude;

						TotalAcceleration += Acceleration;
					}
				}

				Accelerations[i] = TotalAcceleration;
			}

			for (int32 i = 0; i < CelestialBodies.Num(); ++i) {
				if (!CelestialBodies[i]) {
					continue;
				}

				TempPositions[i] += TempVelocities[i] * DynamicTimeStep + 0.5f * Accelerations[i] * DynamicTimeStep * DynamicTimeStep;
			}

			TArray<FVector> NewAccelerations;
			NewAccelerations.SetNum(TempPositions.Num());

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
					float Dist = Direction.Size();

					if (Dist > 1.0f) {
						FVector Fdir = Direction.GetSafeNormal();
						float AccelerationMagnitude = G * TempMass[j] / (Dist * Dist);
						FVector Acceleration = Fdir * AccelerationMagnitude;

						TotalAcceleration += Acceleration;
					}
				}

				NewAccelerations[i] = TotalAcceleration;
			}

			for (int32 i = 0; i < CelestialBodies.Num(); ++i) {
				if (!CelestialBodies[i]) {
					continue;
				}

				TempVelocities[i] += 0.5f * (Accelerations[i] + NewAccelerations[i]) * DynamicTimeStep;
			}

			if (TempPositions[BodyIndex].Size() > Distance * 1000.0f) {
				UE_LOG(LogTemp, Error, TEXT("Orbit simulation unstable for %s at step %d - position exploded"), *Body->BodyName, step);
				bOrbitUnstable = true;
				break;
			}

			OrbitPoints.Add(TempPositions[BodyIndex]);
		}

		if (bOrbitUnstable) {
			continue;
		}

		float TotalDistance = 0.0f;
		for (int32 k = 0; k < OrbitPoints.Num() - 1; ++k) {
			TotalDistance += FVector::Dist(OrbitPoints[k], OrbitPoints[k + 1]);
		}

		float ExpectedCircumference = 2.0f * PI * Distance;
		float DistanceRatio = TotalDistance / ExpectedCircumference;

		UE_LOG(LogTemp, Warning, TEXT("Orbit for %s: Points=%d, TotalDist=%.2f, ExpectedCirc=%.2f, Ratio=%.2f, Start=%s, End=%s"),
			*Body->BodyName,
			OrbitPoints.Num(),
			TotalDistance,
			ExpectedCircumference,
			DistanceRatio,
			*OrbitPoints[0].ToString(),
			*OrbitPoints.Last().ToString());

		if (drawOrbits) {
			FColor OrbitColor = Body->OrbitColor.ToFColor(true);

			for (int32 k = 0; k < OrbitPoints.Num() - 1; ++k) {
				DrawDebugLine(
					GetWorld(),
					OrbitPoints[k],
					OrbitPoints[k + 1],
					OrbitColor,
					false,
					0.016f,
					0,
					5.0f
				);
			}

			DrawDebugSphere(GetWorld(), OrbitPoints[0], 15.0f, 8, FColor::Green, false, 0.016f);
			DrawDebugSphere(GetWorld(), OrbitPoints.Last(), 15.0f, 8, FColor::Red, false, 0.016f);
			DrawDebugSphere(GetWorld(), CentralBody->GetActorLocation(), 20.0f, 12, FColor::Yellow, false, 0.016f);
		}

		Body->SetActorLocation(OriginalPosition);
		Body->CurrentVelocity = OriginalVelocity;
	}
}