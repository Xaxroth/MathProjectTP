#include "DefaultActor.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "MathProjectTP/MathProjectTPGameMode.h"

ADefaultActor::ADefaultActor()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
	RootComponent = StaticMeshComponent;
}

// Sets up the actors to recognize each other in the scene.

void ADefaultActor::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();

	if (World)
	{
		AMathProjectTPGameMode* GameMode = Cast<AMathProjectTPGameMode>(World->GetAuthGameMode());

		if (GameMode)
		{
			GameMode->DefaultActorsArray.Add(this);
		}
	}
}

// INTERPOLATION //
// Interpolates between the min and max value determined by ChargeRange. The float that is changed by this is later used to scale the actor in the scene.
// This results in an actor that rapidly grows and shrinks when the interpolation state is triggered.

void ADefaultActor::Interpolate(float DeltaTime, bool ExitAtFullCharge)
{
	float InterpolationFactor = 0.5f + 0.5f * FMath::Sin(ChargeSpeed * GetWorld()->GetTimeSeconds());

	float InterpolatedFloat = FMath::Lerp(0.25f, ChargeRange, InterpolationFactor);

	FVector NewScale = FVector::OneVector * InterpolatedFloat;
	SetActorScale3D(NewScale);

	if (InterpolatedFloat == ChargeRange && ExitAtFullCharge) {
		MovementState = ActionState::Default;
	}
}

void ADefaultActor::AxisAlignedBoundingBox() 
{
	AABBMin = GetActorLocation() - AABBHalfExtents;
	AABBMax = GetActorLocation() + AABBHalfExtents;

	DrawDebugBox(GetWorld(), GetActorLocation(), AABBHalfExtents, ActorColor, false, 0, 0, 5);
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
	if (CheckAABBCollision(OtherActor))
	{
		CheckDirection(OtherActor);
	}
}

// INTERSECTION//

// Checks the direction of the external actor. Once a collision happens, all actors will perform this check in relativity to their collided "partner".

void ADefaultActor::CheckDirection(ADefaultActor* OtherActor)
{
	FVector DirectionToOtherActor = (OtherActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	FVector ForwardVector = GetActorForwardVector().GetSafeNormal();

	FVector CrossProduct = FVector::CrossProduct(DirectionToOtherActor, ForwardVector);
	float DotProduct = FVector::DotProduct(DirectionToOtherActor, ForwardVector);

	// Check the dot product to determine relative direction
	if (DotProduct > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor In Front"));
		MovementState = ActionState::Interpolate;
	}
	else if (DotProduct < 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor Behind"));
		MovementState = ActionState::Default;
	}

	if (CrossProduct.Z > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor To The Right"));
		if (PowerLevel < PowerMaxLimit) 
		{
			PowerLevel++;
		}
	}
	else if (CrossProduct.Z < 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor To The Left"));
		if (PowerLevel > PowerMinLimit)
		{
			PowerLevel--;
		}
	}
}

// COLLISION //
// A ground check that utilizes a raycast (LineTrace) from the center point of the actor downwards. If the ray hits an object in the world, it is registered and the desired logic can be implemented.
// This will collide with everything, but ideally setting up some form of tag system like "Ground" or "Environment" would help avoid unwanted behavior.
// For example, assigning the "Environment" enum type to a default actor and only performing the TakeDamage() if the raycast hits a DefaultActor with the "Environment" enum type.

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

			if (TotalLandingValue > LandingThreshhold) 
			{
				// Take damage, change color, bounce?
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


void ADefaultActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	StaticMeshComponent->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f)); // If I don't do this the scale is always 0 in play mode for some reason.
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, ADefaultActor::StaticClass());

	AMathProjectTPGameMode* GameMode = GetWorld()->GetAuthGameMode<AMathProjectTPGameMode>();

	FString DebugMessage = FString::Printf(TEXT("PowerLevel: %.2f"), PowerLevel);
	float TimeToDisplay = 5.0f;
	FColor DebugTextColor = FColor::Green;

	GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, DebugTextColor, DebugMessage);

	if (PowerLevel > BossThreshhold) 
	{
		ActorType = ActorType::Boss;
		ActorColor = FColor::Red;
	}
	else if (PowerLevel > DangerousThreshhold) 
	{
		ActorType = ActorType::Strong;
		ActorColor = FColor::Orange;
	}
	else if (PowerLevel > AverageThreshhold)
	{
		ActorType = ActorType::Average;
		ActorColor = FColor::Yellow;
	}
	else if (PowerLevel < AverageThreshhold) 
	{
		ActorType = ActorType::Weak;
		ActorColor = FColor::Blue;
	}

	if (GameMode)
	{
		for (ADefaultActor* OtherActor : GameMode->DefaultActorsArray)
		{
			if (CheckAABBCollision(OtherActor))
			{
				HandleAABBCollision(OtherActor);
			}
		}
	}

	AxisAlignedBoundingBox();

	GroundCheck();

	switch (MovementState) {
	case ActionState::Interpolate:
		Interpolate(DeltaTime, true);
		break;
	case ActionState::Default:
		break;
	}

}

