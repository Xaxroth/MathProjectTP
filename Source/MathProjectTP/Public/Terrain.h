// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Terrain.generated.h"

class UProceduralMeshComponent;
class UMaterialInterface;

UCLASS()
class MATHPROJECTTP_API ATerrain : public AActor
{
	GENERATED_BODY()
	
public:	
	ATerrain();

	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0))
	int XSize = 0;
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0))
	int YSize = 0;
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0.01))
	int Scale = 0;
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0.01))
	float NoiseScale = 0.1f;

	float RandomGeneratedNumber = 0.1f;
	float MinValue = 0.1f;
	float MaxValue = 0.5f;


	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	UProceduralMeshComponent* ProceduralMesh;

	UPROPERTY(EditAnywhere)
	TArray<FVector> Vertices;

	UPROPERTY(EditAnywhere)
	float PerlinNoiseMultiplier = 1.0f;

	float PerlinNoiseValue = 0.1;

	UPROPERTY(EditAnywhere)
	TArray<int> Triangles;

	void CreateVertices();

	void CreateTriangles();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:

	TArray<FVector2D> UV0;
};
