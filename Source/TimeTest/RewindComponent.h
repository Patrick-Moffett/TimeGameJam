// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "GameFramework\Actor.h"
#include "Components/TimelineComponent.h"
#include "RewindComponent.generated.h"

USTRUCT()
struct FRewindStateInfoStruct
{
	GENERATED_BODY()
	
	FRewindStateInfoStruct(){}
	FRewindStateInfoStruct(FVector Location, FRotator Rotation, FVector Velocity, FVector AngularVelocity)
	{
		this->Location = Location;
		this->Rotation = Rotation;
		this->Velocity = Velocity;
		this->AngularVelocity = AngularVelocity;
	}

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FRotator Rotation;

	UPROPERTY()
	FVector Velocity;

	UPROPERTY()
	FVector AngularVelocity;

};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TIMETEST_API URewindComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URewindComponent();

	UFUNCTION(BlueprintCallable)
	void SetMeshReference(UStaticMeshComponent* StaticMeshComponent);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void RecordRewindState();

	UFUNCTION(BlueprintCallable)
	void Rewind();

	UFUNCTION(BlueprintCallable)
	void StopRewind();

	UFUNCTION()
	void RewindTimelineUpdate(float value);

	UFUNCTION()
	void RewindTimelineFinished();

	FTimeline RewindTimeline;

	UPROPERTY(EditDefaultsOnly)
	class UCurveFloat* Curve;

	UPROPERTY()
	FOnTimelineFloat TimelineUpdateFunction;
	UPROPERTY()
	FOnTimelineEvent TimelineEndedFunction;

	UPROPERTY()
	TArray<FRewindStateInfoStruct> RewindStates;

	UPROPERTY()
	FRewindStateInfoStruct InitialState;

	UPROPERTY()
	UStaticMeshComponent* ActorComponentMesh;

	UPROPERTY()
	FTimerHandle RecordStateTimerHandle;


	UPROPERTY()
	bool bShouldRecordState = true;

	UPROPERTY()
	bool bShouldRewindStop = false;

	UPROPERTY()
	bool bIsRewindSpeedBoosted = false;

	UPROPERTY()
	bool bIsActorFrozen = false;

	UPROPERTY()
	bool bActorSimulatePhysics;

	UPROPERTY(Category = "Setup", EditDefaultsOnly)
	float DeltaRecordTime = .3f;

	UPROPERTY(Category = "Setup", EditDefaultsOnly)
	float RewindBoostMultiplier =  5.0f;





public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
