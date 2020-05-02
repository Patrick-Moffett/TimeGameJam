// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "TimeTestGameMode.h"
#include "TimeTestHUD.h"
#include "TimeTestCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATimeTestGameMode::ATimeTestGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ATimeTestHUD::StaticClass();
}
