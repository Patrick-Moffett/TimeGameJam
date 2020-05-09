// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TriggerVolume.h"
#include "MoveBlockage.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TIMETEST_API UMoveBlockage : public UActorComponent
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Sets default values for this component's properties
	UMoveBlockage();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void OpenBlockage(float DeltaTime);
	void CloseBlockage(float DeltaTime);
	float TotalMassOfActors() const;
	void FindAudioComponent();
	void CheckForPressurePlate() const;

private:

	// move blockage
	FVector InitialLocation;
	FVector CurrentLocation;

	UPROPERTY(EditAnywhere)
		FVector TargetMovement = { 0.f, 0.f, 0.f };

	UPROPERTY(EditAnywhere)
		float MoveSpeed = 0.4f;

	UPROPERTY(EditAnywhere)
		float BlockageCloseDelay = 0.3f;

	float BlockageLastMoved = 0.f;

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
