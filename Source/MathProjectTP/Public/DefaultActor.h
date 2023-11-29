// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "DefaultActor.generated.h"

UENUM(BlueprintType)
enum class ActionState : uint8
{
	Interpolate,
	ChangeColor,
	Default
};

UENUM(BlueprintType)
enum class ActorType : uint8
{
	Weak,
	Average,
	Strong,
	Boss
};

UCLASS()
class MATHPROJECTTP_API ADefaultActor : public AActor
{
	GENERATED_BODY()
public:
	ADefaultActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sphere", meta = (AllowPrivateAccess = "true"))
	UMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enums")
	TEnumAsByte<ActionState> MovementState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enums")
	TEnumAsByte<ActorType> ActorType;

	UFUNCTION(BlueprintCallable, Category = "Interpolation")
	void Interpolate(float DeltaTime, bool ExitAtFullCharge);

	void AxisAlignedBoundingBox();

	void CheckDirection(ADefaultActor* OtherActor);

	bool CheckAABBCollision(const ADefaultActor* OtherActor) const;

	UPROPERTY(EditAnywhere)
	FVector AABBHalfExtents;

	void HandleAABBCollision(ADefaultActor* OtherActor);

	void GroundCheck();

	void GetOtherActors();

	bool Interpolating = true;

	bool Falling = false;

	bool Grounded = false;

	FVector LastKnownPosition;

	FVector NewPosition;

	FVector AABBMin;
	FVector AABBMax;

	FColor ActorColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ground Check")
	float LandingThreshhold = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ground Check")
	bool CheckForGround = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radius")
	float DetectionRadiusSize = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform")
	float InterpolationSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform")
	float ChargeSpeed;

	float PowerLevel = 0.0f;
	float PowerMinLimit = 0.0f;
	float PowerMaxLimit = 10000.0f;

	float WeakThreshhold = 1000.0f;
	float AverageThreshhold = 3000.0f;
	float DangerousThreshhold = 6000.0f;
	float BossThreshhold = 9000.0f;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform")
	float ChargeRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform")
	AActor* TargetActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform")
	FTransform StartTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform")
	FTransform TargetTransform;
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

};

