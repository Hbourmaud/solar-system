#include "CelestialBody.h"

const float G = 0.0000000000674f;

ACelestialBody::ACelestialBody()
{
    PrimaryActorTick.bCanEverTick = true;

    ProceduralMesh = CreateDefaultSubobject<UProceduralPlanetGenerator>(TEXT("ProceduralMesh"));
    RootComponent = ProceduralMesh;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(RootComponent);

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

    if (UseProcedural && ProceduralMesh) {
		RegeneratePlanet();
		MeshComponent->SetVisibility(false);

        if (PlanetMaterial) {
            ProceduralMesh->SetMaterial(0, PlanetMaterial);
            UE_LOG(LogTemp, Log, TEXT("Applied custom material to %s"), *BodyName);
        } else {
            UMaterial* DefaultMaterial = LoadObject<UMaterial>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial"));
            if (DefaultMaterial) {
                UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(DefaultMaterial, this);
                if (DynamicMaterial) {
                    DynamicMaterial->SetVectorParameterValue(FName("Color"), OrbitColor);
                    ProceduralMesh->SetMaterial(0, DynamicMaterial);
                }
            }
        }

    } else if (MeshComponent) {
        ProceduralMesh->SetVisibility(false);
    }
}

void ACelestialBody::RegeneratePlanet()
{
    if (ProceduralMesh) {
        ProceduralMesh->Radius = Radius * VisualScale;
        ProceduralMesh->Subdivisions = PlanetSubdivisions;
        ProceduralMesh->GeneratePlanet();

        UE_LOG(LogTemp, Log, TEXT("Regenerated procedural planet: %s with Radius=%.2f, Subdivisions=%d"),
            *BodyName, Radius, PlanetSubdivisions);
    }
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