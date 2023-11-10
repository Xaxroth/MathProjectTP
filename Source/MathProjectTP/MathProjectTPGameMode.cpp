// Copyright Epic Games, Inc. All Rights Reserved.

#include "MathProjectTPGameMode.h"
#include "MathProjectTPCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMathProjectTPGameMode::AMathProjectTPGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
