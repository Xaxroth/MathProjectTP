#include "DefaultActor.h"

ADefaultActor::ADefaultActor()
{
	PrimaryActorTick.bCanEverTick = true;

	DetectionRadius = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionRadius"));
	DetectionRadius->SetSphereRadius(DetectionRadiusSize);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
	RootComponent = StaticMeshComponent;
}

void ADefaultActor::BeginPlay()
{
	Super::BeginPlay();
	StaticMeshComponent->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
	DetectionRadius->SetSphereRadius(DetectionRadiusSize);
}

void ADefaultActor::Interpolate(float DeltaTime, bool ExitAtFullCharge)
{
	float InterpolationFactor = 0.5f + 0.5f * FMath::Sin(ChargeSpeed * GetWorld()->GetTimeSeconds());

	float InterpolatedFloat = FMath::Lerp(0.0f, ChargeRange, InterpolationFactor);

	FVector NewScale = FVector::OneVector * InterpolatedFloat;
	SetActorScale3D(NewScale);

	if (InterpolatedFloat == ChargeRange && ExitAtFullCharge) {
		MovementState = MovementState::Default;
	}
}

void ADefaultActor::AxisAlignedBoundingBox() 
{
	AABBMin = GetActorLocation() - AABBHalfExtents;
	AABBMax = GetActorLocation() + AABBHalfExtents;

	DrawDebugBox(GetWorld(), GetActorLocation(), AABBHalfExtents, FColor::Green, false, 0, 0, 5);
}

bool ADefaultActor::CheckAABBCollision(const ADefaultActor* OtherActor) const
{
	FVector OtherAABBMin = OtherActor->GetActorLocation() - OtherActor->AABBHalfExtents;
	FVector OtherAABBMax = OtherActor->GetActorLocation() + OtherActor->AABBHalfExtents;

	FVector AABBMinTemp = GetActorLocation() - AABBHalfExtents;
	FVector AABBMaxTemp = GetActorLocation() + AABBHalfExtents;

	// compare the vectors to each other / check if the actors in question are colliding with each other.
	// this might get kind of heavy with a lot of actors
	bool OverlapsX = AABBMin.X <= OtherAABBMax.X && AABBMax.X >= OtherAABBMin.X;
	bool OverlapsY = AABBMin.Y <= OtherAABBMax.Y && AABBMax.Y >= OtherAABBMin.Y;
	bool OverlapsZ = AABBMin.Z <= OtherAABBMax.Z && AABBMax.Z >= OtherAABBMin.Z;

	return OverlapsX && OverlapsY && OverlapsZ;
}

void ADefaultActor::HandleAABBCollision(ADefaultActor* OtherActor)
{
		FString DebugMessage = FString::Printf(TEXT("Another Actor Found"));
		float TimeToDisplay = 5.0f;
		FColor DebugTextColor = FColor::Green;

		GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, DebugTextColor, DebugMessage);

	if (CheckAABBCollision(OtherActor))
	{
		CheckDirection(OtherActor);
	}
}

void ADefaultActor::GroundCheck()
{
	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0.f, 0.f, 100.f);
	FHitResult HitResult;

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1, 0, 1);

	if (!Grounded) 
	{
		if (!Falling) 
		{
			LastKnownPosition = GetActorLocation();
			Falling = true;
		}

		bool Landed = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);

		if (Landed) 
		{
			NewPosition = GetActorLocation();

			float TotalLandingValue = LastKnownPosition.Z - NewPosition.Z;
			FString DebugMessage = FString::Printf(TEXT("Total Landing Value: %.2f"), TotalLandingValue);
			float TimeToDisplay = 5.0f;
			FColor DebugTextColor = FColor::Green;

			GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, DebugTextColor, DebugMessage);

			if (TotalLandingValue > LandingThreshhold) 
			{
				// Take Damage
				Falling = false;
			}
		}
	}
	else 
	{
		Falling = false;
	}

	Grounded = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);
}

// REPLACED BY AABB
//void ADefaultActor::GetOtherActors() {
//	TArray<AActor*> OverlappingActors;
//
//	for (AActor* OverlappingActor : OverlappingActors)
//	{
//		ADefaultActor* DefaultActor = Cast<ADefaultActor>(OverlappingActor);
//		if (DefaultActor)
//		{
//			if (DefaultActor->ActorType == ActorType::Projectile && ActorType == ActorType::Enemy) 
//			{
//				// Enemy takes damage from projectile, call on relevant takedamage function here
//			}
//			// Here I can compare default actors to each other. For example, if a projectile actor collides with an enemy
//		}
//	}
//}

void ADefaultActor::CheckDirection(ADefaultActor* OtherActor)
{
	FVector DirectionToOtherActor = (OtherActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	FVector ForwardVector = GetActorForwardVector();

	FVector CrossProduct = FVector::CrossProduct(DirectionToOtherActor, ForwardVector);
	// Check and compare both vector angles
	float DotProduct = FVector::DotProduct(DirectionToOtherActor, ForwardVector);

	// Check the sign of the dot product to determine relative direction
	if (DotProduct > 0)
	{
		// In front
		FString DebugMessage = FString::Printf(TEXT("Actor In Front"));
		float TimeToDisplay = 5.0f;
		FColor DebugTextColor = FColor::Yellow;
		GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, DebugTextColor, DebugMessage);
	}
	else if (DotProduct < 0)
	{
		// Behind
		FString DebugMessage = FString::Printf(TEXT("Actor Behind"));
		float TimeToDisplay = 5.0f;
		FColor DebugTextColor = FColor::Yellow;
		GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, DebugTextColor, DebugMessage);
	}

	if (CrossProduct.Z > 0) 
	{
		// Right
		FString DebugMessage = FString::Printf(TEXT("Actor To The Right"));
		float TimeToDisplay = 5.0f;
		FColor DebugTextColor = FColor::Yellow;
		GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, DebugTextColor, DebugMessage);
	}
	else if (CrossProduct.Z < 0) 
	{
		// Left
		FString DebugMessage = FString::Printf(TEXT("Actor To The Left"));
		float TimeToDisplay = 5.0f;
		FColor DebugTextColor = FColor::Yellow;
		GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, DebugTextColor, DebugMessage);
	}
}

void ADefaultActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	StaticMeshComponent->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f)); // If I don't do this the scale is always 0 in play mode for some reason.
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, ADefaultActor::StaticClass());

	for (AActor* OverlappingActor : OverlappingActors)
	{
				FString DebugMessage = FString::Printf(TEXT("Actor Behind"));
				float TimeToDisplay = 5.0f;
				FColor DebugTextColor = FColor::Yellow;
				GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, DebugTextColor, DebugMessage);
		ADefaultActor* OtherActor = Cast<ADefaultActor>(OverlappingActor);
		if (OtherActor)
		{
			// Check for AABB collision
			if (CheckAABBCollision(OtherActor))
			{
				// Collision occurred, perform actions
				HandleAABBCollision(OtherActor);
			}
		}
	}

	AxisAlignedBoundingBox();

	GroundCheck();

	switch (MovementState) {
	case MovementState::Interpolate:
		Interpolate(DeltaTime, true);
		break;
	case MovementState::Default:
		break;
	}

}

