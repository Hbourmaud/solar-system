#include "CelestialBody.h"

const float G = 0.0000000000674f;

ACelestialBody::ACelestialBody()
{
    PrimaryActorTick.bCanEverTick = true;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    RootComponent = MeshComponent;

    Mass = 1.0f;
    Radius = 100.0f;
    SurfaceGravity = 9.81f;
    CurrentVelocity = FVector::ZeroVector;
    AccumulatedAcceleration = FVector::ZeroVector;
}

void ACelestialBody::BeginPlay()
{
    Super::BeginPlay();

    if (Mass <= 1.0f) {
        CalculateMassFromGravity();
    }

    CurrentVelocity = InitialVelocity;
}

void ACelestialBody::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ACelestialBody::CalculateMassFromGravity()
{
    Mass = (SurfaceGravity * Radius * Radius) / G;
}

void ACelestialBody::ApplyGravitationalForce(const FVector& Force, float DeltaTime)
{
    FVector Acceleration = Force / Mass;
    AccumulatedAcceleration += Acceleration;
}

void ACelestialBody::UpdatePosition(float DeltaTime)
{
    CurrentVelocity += AccumulatedAcceleration * DeltaTime;

    FVector NewPosition = GetActorLocation() + (CurrentVelocity * DeltaTime);
    SetActorLocation(NewPosition);

    AccumulatedAcceleration = FVector::ZeroVector;
}