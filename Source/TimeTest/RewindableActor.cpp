// Fill out your copyright notice in the Description page of Project Settings.

#include "Particles\ParticleSystemComponent.h"
#include "RewindableActor.h"


// Sets default values
ARewindableActor::ARewindableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//create static Meash
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("StaticMeshComponent"));
	SetRootComponent(StaticMeshComponent);

	//create rewind component and set mesh
	RewindComponent = CreateDefaultSubobject<URewindComponent>(FName("RewindComponent"));

	FrozenParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(FName("FrozenParticleSystem"));
	FrozenParticleSystem->SetupAttachment(StaticMeshComponent);
	FrozenParticleSystem->bAutoActivate = false;
}

// Called when the game starts or when spawned
void ARewindableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARewindableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

