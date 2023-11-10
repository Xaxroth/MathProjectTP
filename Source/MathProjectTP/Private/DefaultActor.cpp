#include "DefaultActor.h"

ADefaultActor::ADefaultActor()
{
	PrimaryActorTick.bCanEverTick = true;

	DetectionRadius = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionRadius"));
	DetectionRadius->SetSphereRadius(DetectionRadiusSize);
}

void ADefaultActor::BeginPlay()
{
	Super::BeginPlay();

	DetectionRadius->SetSphereRadius(DetectionRadiusSize);

	FString DebugMessage = TEXT("DefaultActor in Scene");
	float TimeToDisplay = 5.0f;
	FColor DebugTextColor = FColor::Green;

	GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, DebugTextColor, DebugMessage);
}

void ADefaultActor::Interpolate(float DeltaTime, bool ExitAtFullCharge)
{
	float InterpolationFactor = 0.5f + 0.5f * FMath::Sin(ChargeSpeed * GetWorld()->GetTimeSeconds());

	float InterpolatedFloat = FMath::Lerp(0.0f, ChargeRange, InterpolationFactor);

	FString DebugMessage = FString::Printf(TEXT("Interpolated Float Value: %.2f"), InterpolatedFloat);
	float TimeToDisplay = 5.0f;
	FColor DebugTextColor = FColor::Green;

	GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, DebugTextColor, DebugMessage);

	FVector NewScale = FVector::OneVector * InterpolatedFloat;
	SetActorScale3D(NewScale);

	if (InterpolatedFloat == ChargeRange && ExitAtFullCharge) {
		MovementState = MovementState::Default;
	}
}

void ADefaultActor::GetOtherActors() {
	TArray<AActor*> OverlappingActors;
	DetectionRadius->GetOverlappingActors(OverlappingActors);

	for (AActor* OverlappingActor : OverlappingActors)
	{
		ADefaultActor* DefaultActor = Cast<ADefaultActor>(OverlappingActor);
		if (DefaultActor)
		{
			if (DefaultActor->ActorType == ActorType::Projectile && ActorType == ActorType::Enemy) 
			{
				// AIEEEEE
			}
			FString DebugMessage = FString::Printf(TEXT("Found Actor"));
			float TimeToDisplay = 5.0f;
			FColor DebugTextColor = FColor::Green;
			GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, DebugTextColor, DebugMessage);
			// Here I can compare default actors to each other. For example, if a projectile actor collides with an enemy
		}

	}
}

void ADefaultActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GetOtherActors();

	switch (MovementState) {
	case MovementState::Interpolate:
		Interpolate(DeltaTime, true);
		break;
	case MovementState::Default:
		break;
	}

}

