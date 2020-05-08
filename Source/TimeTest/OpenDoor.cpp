// Copyright Mcbright


#include "OpenDoor.h"
#include "Components/AudioComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

#define OUT

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	InitialYaw = GetOwner()->GetActorRotation().Yaw;
	CurrentYaw = InitialYaw;
	OpenAngle += InitialYaw;

	CheckForPressurePlate();
	FindAudioComponent();
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TotalMassOfActors() >= MassToOpenDoor)
	{
		OpenDoor(DeltaTime);
		DoorLastOpened = GetWorld()->GetTimeSeconds();
	}
	else
	{
		if (GetWorld()->GetTimeSeconds() - DoorLastOpened > DoorCloseDelay)
		{
			CloseDoor(DeltaTime);
		}
	}
}

void UOpenDoor::OpenDoor(float DeltaTime)
{
	CurrentYaw = GetOwner()->GetActorRotation().Yaw;
	FRotator DoorRotation(0.f, 0.f, 0.f);
	DoorRotation.Yaw = FMath::FInterpTo(CurrentYaw, OpenAngle, DeltaTime, DoorOpenSpeed);
	GetOwner()->SetActorRotation(DoorRotation);

	if (!AudioComponent) { return; }
	if (!HasOpenSoundPlayed)
	{
		AudioComponent->Play();
		HasOpenSoundPlayed = true;
		HasCloseSoundPlayed = false;
	}
}

void UOpenDoor::CloseDoor(float DeltaTime)
{
	CurrentYaw = GetOwner()->GetActorRotation().Yaw;
	FRotator DoorRotation(0.f, 0.f, 0.f);
	DoorRotation.Yaw = FMath::FInterpTo(CurrentYaw, InitialYaw, DeltaTime, DoorCloseSpeed);
	GetOwner()->SetActorRotation(DoorRotation);

	if (!AudioComponent) { return; }
	if (!HasCloseSoundPlayed)
	{
		AudioComponent->Play();
		HasCloseSoundPlayed = true;
		HasOpenSoundPlayed = false;
	}
}

float UOpenDoor::TotalMassOfActors() const
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

void UOpenDoor::FindAudioComponent()
{
	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();

	if (!AudioComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Missing Audio Component!"), *GetOwner()->GetName());
	}
}

void UOpenDoor::CheckForPressurePlate() const
{
	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has the open door componet on it, but no pressureplate set!"), *GetOwner()->GetName());
	}
}