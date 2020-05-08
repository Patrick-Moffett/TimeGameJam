// Copyright Mcbright

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TriggerVolume.h"
#include "OpenDoor.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TIMETEST_API UOpenDoor : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UOpenDoor();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void OpenDoor(float DeltaTime);
	void CloseDoor(float DeltaTime);
	float TotalMassOfActors() const;
	void FindAudioComponent();
	void CheckForPressurePlate() const;

private:

	float InitialYaw;
	float CurrentYaw;

	UPROPERTY(EditAnywhere)
		float OpenAngle = 90.f;

	UPROPERTY(EditAnywhere)
		float DoorOpenSpeed = 1.5f;

	UPROPERTY(EditAnywhere)
		float DoorCloseSpeed = 2.5f;

	UPROPERTY(EditAnywhere)
		float DoorCloseDelay = 0.3f;

	float DoorLastOpened = 0.f;

	UPROPERTY(EditAnywhere)
		ATriggerVolume* PressurePlate = nullptr;

	UPROPERTY(EditAnywhere)
		float MassToOpenDoor = 50.f;

	UPROPERTY()
		UAudioComponent* AudioComponent = nullptr;

	// tracks wether sound has been played
	bool HasOpenSoundPlayed = false;
	bool HasCloseSoundPlayed = true;
};
