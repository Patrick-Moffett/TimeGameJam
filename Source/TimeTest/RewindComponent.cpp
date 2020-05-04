// Fill out your copyright notice in the Description page of Project Settings.


#include "RewindComponent.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveFloat.h" 
#include "TimerManager.h"



// Sets default values for this component's properties
URewindComponent::URewindComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


void URewindComponent::SetMeshReference(UStaticMeshComponent* StaticMeshComponent)
{
	//Setup staticMesh reference and bool indicating if it usually simulates physics
	ActorComponentMesh = StaticMeshComponent;
	if (ActorComponentMesh->IsSimulatingPhysics())
	{
		bActorSimulatePhysics = true;
	}
	else
	{
		bActorSimulatePhysics = false;
	}
}

// Called when the game starts
void URewindComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ActorComponentMesh)
	{
		//Set Initial State based on initial position
		InitialState = FRewindStateInfoStruct(ActorComponentMesh->GetComponentLocation(),
			ActorComponentMesh->GetComponentRotation(),
			ActorComponentMesh->GetComponentVelocity(),
			ActorComponentMesh->GetPhysicsAngularVelocityInDegrees());

		//add initial state to Rewind States
		RewindStates.Add(InitialState);

		//Setup Timer To Record States
		GetWorld()->GetTimerManager().SetTimer(
			RecordStateTimerHandle,
			this,
			&URewindComponent::RecordRewindState,
			DeltaRecordTime,
			true);

		//Setup Timeline Component
		RewindTimeline.SetTimelineLength(1.0f);
		TimelineUpdateFunction.BindUFunction(this, FName("RewindTimelineUpdate"));
		TimelineEndedFunction.BindUFunction(this, FName("RewindTimelineFinished"));
		RewindTimeline.AddInterpFloat(Curve,TimelineUpdateFunction);
		RewindTimeline.SetTimelineFinishedFunc(TimelineEndedFunction);
		//RewindTimeline.
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NO MESH ASSIGNED TO REWIND COMPONENT"))
	}

}

void URewindComponent::RecordRewindState()
{
	if (bShouldRecordState)
	{
		RewindStates.Add(FRewindStateInfoStruct(ActorComponentMesh->GetComponentLocation(),
		ActorComponentMesh->GetComponentRotation(),
		ActorComponentMesh->GetComponentVelocity(),
		ActorComponentMesh->GetPhysicsAngularVelocityInDegrees()));
	}
}

void URewindComponent::Rewind()
{

	if (RewindTimeline.IsPlaying()){return;}

	bShouldRecordState = false;

	bShouldRewindStop = false;

	GetOwner()->SetActorEnableCollision(false);

	if (bActorSimulatePhysics)
	{
		ActorComponentMesh->SetSimulatePhysics(false);
	}

	float playbackRate = 1.0f / ((RewindStates.Num() - 1) * DeltaRecordTime);
	
	if (bIsRewindSpeedBoosted){playbackRate = playbackRate * 5;}

	RewindTimeline.SetPlayRate(playbackRate);

	RewindTimeline.PlayFromStart();
}

void URewindComponent::StopRewind()
{
}

void URewindComponent::RewindTimelineUpdate(float value)
{
	float currentPosition= (1.0f - value)*(RewindStates.Num()-1);
	if (currentPosition + 1 == RewindStates.Num())
	{
		return;
	}
	UE_LOG(LogTemp,Warning,TEXT("Current Position: %f RewindStates: %i Fractional: %f"), currentPosition, RewindStates.Num(), FMath::Fractional(currentPosition))

	int lowerBound = FMath::TruncToInt(currentPosition);
	
	auto location = FMath::Lerp(RewindStates[lowerBound].Location, RewindStates[lowerBound+1].Location, FMath::Fractional(currentPosition));
	auto rotation = FMath::Lerp(RewindStates[lowerBound].Rotation, RewindStates[lowerBound + 1].Rotation, FMath::Fractional(currentPosition));
	UE_LOG(LogTemp,Warning,TEXT("\nLocation1: %s \nLocation2: %s \nnew Location: %s"), *RewindStates[lowerBound + 1].Location.ToString(), *RewindStates[lowerBound].Location.ToString(),*location.ToString())
	ActorComponentMesh->SetWorldLocationAndRotation(location,rotation);
}

void URewindComponent::RewindTimelineFinished()
{
}


// Called every frame
void URewindComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	RewindTimeline.TickTimeline(DeltaTime);
	//UE_LOG(LogTemp, Warning, TEXT("DeltaTime: %f"),DeltaTime)
}

