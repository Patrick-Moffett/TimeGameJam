// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RewindComponent.h"
#include "RewindableActor.generated.h"

UCLASS()
class TIMETEST_API ARewindableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARewindableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(VisibleDefaultsOnly)
	class UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleDefaultsOnly)
	class URewindComponent* RewindComponent;

	UPROPERTY(VisibleDefaultsOnly)
	UParticleSystemComponent* FrozenParticleSystem;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
