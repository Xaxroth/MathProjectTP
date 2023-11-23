// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DefaultActor.h"
#include "GameFramework/GameModeBase.h"
#include "MathProjectTPGameMode.generated.h"

UCLASS(minimalapi)
class AMathProjectTPGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMathProjectTPGameMode();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameMode")
	TArray<ADefaultActor*> DefaultActorsArray;
};



