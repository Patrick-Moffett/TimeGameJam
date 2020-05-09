// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveBlockage.h"
#include "Components/AudioComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

#define OUT

// Sets default values for this component's properties
UMoveBlockage::UMoveBlockage()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UMoveBlockage::BeginPlay()
{
	Super::BeginPlay();

	InitialLocation = GetOwner()->GetActorLocation();
	CurrentLocation = InitialLocation;

	CheckForPressurePlate();
	FindAudioComponent();
}

// Called every frame
void UMoveBlockage::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TotalMassOfActors() >= MassToOpenDoor)
	{
		OpenBlockage(DeltaTime);
		BlockageLastMoved = GetWorld()->GetTimeSeconds();
	}
	else
	{
		if (GetWorld()->GetTimeSeconds() - BlockageLastMoved > BlockageCloseDelay)
		{
			CloseBlockage(DeltaTime);
		}
	}
}

void UMoveBlockage::OpenBlockage(float DeltaTime)
{
	CurrentLocation = GetOwner()->GetActorLocation();
	FVector BlockageMovement = { 0.f, 0.f, 0.f };
	BlockageMovement = FMath::VInterpTo(CurrentLocation,(InitialLocation + TargetMovement), DeltaTime, MoveSpeed);
	GetOwner()->SetActorLocation(BlockageMovement);

	if (!AudioComponent) { return; }
	if (!HasOpenSoundPlayed)
	{
		AudioComponent->Play();
		HasOpenSoundPlayed = true;
		HasCloseSoundPlayed = false;
	}
}

void UMoveBlockage::CloseBlockage(float DeltaTime)
{
	CurrentLocation = GetOwner()->GetActorLocation();
	FVector BlockageMovement = { 0.f, 0.f, 0.f };
	BlockageMovement = FMath::VInterpTo(CurrentLocation, InitialLocation, DeltaTime, MoveSpeed);
	GetOwner()->SetActorLocation(BlockageMovement);

	if (!AudioComponent) { return; }
	if (!HasCloseSoundPlayed)
	{
		AudioComponent->Play();
		HasCloseSoundPlayed = true;
		HasOpenSoundPlayed = false;
	}
}

float UMoveBlockage::TotalMassOfActors() const
{
	float TotalMass = 0.f;
	TArray<AActor*> OverlappingActors;
	if (!PressurePlate) { return TotalMass; }
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}

	return TotalMass;
}

void UMoveBlockage::FindAudioComponent()
{
	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();

	if (!AudioComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Missing Audio Component!"), *GetOwner()->GetName());
	}
}

void UMoveBlockage::CheckForPressurePlate() const
{
	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has the open door componet on it, but no pressureplate set!"), *GetOwner()->GetName());
	}
}