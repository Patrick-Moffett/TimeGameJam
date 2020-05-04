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
			ActorComponentMesh->GetComponentRotation().Quaternion(),
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

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NO MESH ASSIGNED TO REWIND COMPONENT"))
	}

}
/*
Records the current state of the object into RewindStates
*/
void URewindComponent::RecordRewindState()
{
		RewindStates.Add(FRewindStateInfoStruct(ActorComponentMesh->GetComponentLocation(),
		ActorComponentMesh->GetComponentRotation().Quaternion(),
		ActorComponentMesh->GetComponentVelocity(),
		ActorComponentMesh->GetPhysicsAngularVelocityInDegrees()));
}
/*
Does all the Initializing of values in order to rewind the object, and then starts RewindTimeline
-Also turns of physics and collision on the object for the duration of the rewind.
*/
void URewindComponent::Rewind()
{
	//dont rewind if currently rewinding
	if (RewindTimeline.IsPlaying()){return;}

	//initialize to false before starting
	bShouldRewindStop = false;

	//disable collision
	GetOwner()->SetActorEnableCollision(false);

	//if actor simulates physics, disable them while rewinding
	if (bActorSimulatePhysics)
	{
		ActorComponentMesh->SetSimulatePhysics(false);
	}
	
	//find the playbackrate so that it plays back at 1x speed
	float playbackRate = RewindTimeline.GetTimelineLength() / ((RewindStates.Num() - 1) * DeltaRecordTime);
	
	//Boost the speed if true
	if (bIsRewindSpeedBoosted){playbackRate = playbackRate * RewindBoostMultiplier;}
	//set playback speed
	RewindTimeline.SetPlayRate(playbackRate);

	//pause the update function
	GetWorld()->GetTimerManager().PauseTimer(RecordStateTimerHandle);

	RewindTimeline.PlayFromStart();
}
/*
Sets bShouldRewindStop to true, which will be handles by the RewindTimelineUpdate, in order to stop itself.
*/
void URewindComponent::StopRewind()
{
	bShouldRewindStop = true;
}

void URewindComponent::BoostRewind()
{
	//return if already true
	if (bIsRewindSpeedBoosted)
	{
		return;
	}
	if (RewindTimeline.IsPlaying())
	{
		RewindTimeline.SetPlayRate(RewindTimeline.GetPlayRate() * RewindBoostMultiplier);
	}
	bIsRewindSpeedBoosted = true;
}

void URewindComponent::UnBoostRewind()
{
	//return if already false
	if (!bIsRewindSpeedBoosted)
	{
		return;
	}
	if (RewindTimeline.IsPlaying())
	{
		RewindTimeline.SetPlayRate(RewindTimeline.GetPlayRate()/ RewindBoostMultiplier);
	}
	bIsRewindSpeedBoosted = false;
}
/*
Toggle Rewind Speed
*/
void URewindComponent::ToggleBoostRewindSpeed()
{
	if (bIsRewindSpeedBoosted)
	{
		UnBoostRewind();
	}
	else
	{
		BoostRewind();
	}
}

void URewindComponent::FreezeTime()
{
	//return if already frozen
	if (bIsActorFrozen)
	{
		return;
	}
	
	//stop recording states while frozen
	//pause the update function
	GetWorld()->GetTimerManager().PauseTimer(RecordStateTimerHandle);

	//Record State before disabling physics
	RecordRewindState();

	//stop simulating physics
	if (bActorSimulatePhysics)
	{
		ActorComponentMesh->SetSimulatePhysics(false);
	}

	//set bool to true
	bIsActorFrozen = true;
}

void URewindComponent::UnFreezeTime()
{
	//return if already not frozen
	if (!bIsActorFrozen)
	{
		return;
	}
	
	//unpause the update function
	GetWorld()->GetTimerManager().UnPauseTimer(RecordStateTimerHandle);

	//turn on physics
	if (bActorSimulatePhysics)
	{
		ActorComponentMesh->SetSimulatePhysics(true);
		ActorComponentMesh->SetPhysicsLinearVelocity(RewindStates.Last().Velocity);
		ActorComponentMesh->SetPhysicsAngularVelocity(RewindStates.Last().AngularVelocity);
	}
	//set bool to false
	bIsActorFrozen = false;
}

void URewindComponent::ToggleFreezeTime()
{
	if (bIsActorFrozen)
	{
		UnFreezeTime();
	}
	else
	{
		FreezeTime();
	}
}

/**
Each Update changes the location and rotation of the Static Mesh Component to move backwards in time based on the states contained in RewindStates
*/
void URewindComponent::RewindTimelineUpdate(float value)
{
	//find current position
	float currentPosition = (1.0f - value) * (RewindStates.Num() - 1);

	//don't run the very first update
	//TODO: FIND MORE EFFICIENT WAY TO DO THIS
	if (currentPosition + 1 == RewindStates.Num())
	{
		return;
	}
	//Check if Timeline needs to stop early, wait till close to a stored Frame
	if (bShouldRewindStop && FMath::Fractional(currentPosition)<DeltaRecordTime)
	{
		RewindTimeline.Stop();
		RewindTimelineInterrupted(currentPosition);
		return;
	}
	//get the lowerBound Frame
	int lowerBound = FMath::TruncToInt(currentPosition);
	
	//lerp and slerp between two frames to get the current position and rotation
	auto location = FMath::Lerp(RewindStates[lowerBound].Location, RewindStates[lowerBound+1].Location, FMath::Fractional(currentPosition));
	auto rotation = FQuat::Slerp(RewindStates[lowerBound].Rotation, RewindStates[lowerBound + 1].Rotation, FMath::Fractional(currentPosition));
	
	//set the new location and rotation
	ActorComponentMesh->SetWorldLocationAndRotation(location,rotation);
}

/*
completely clears rewind states and restarts mesh
*/
void URewindComponent::RewindTimelineFinished()
{
	//remove previous states
	RewindStates.Empty();
	RewindStates.Add(InitialState);

	RewindTimelineRestartMesh();
}

/*
clears RewindStates that were passed and restarts mesh
*/
void URewindComponent::RewindTimelineInterrupted(float interruptedFrame)
{
	if (interruptedFrame < 0.0f)
	{
		RewindTimelineFinished();
		return;
	}
	//remove previous states
	int Frame = FMath::TruncToInt(interruptedFrame);
	RewindStates.SetNum(Frame + 1);

	RewindTimelineRestartMesh();

}
/*
Sets location and turns collision on, restarts physics if they were enabled, and restores velocity and angular velocity(Rotation)
*/
void URewindComponent::RewindTimelineRestartMesh()
{
	//SetLocationAndRotation
	ActorComponentMesh->SetWorldLocationAndRotation(RewindStates.Last().Location, RewindStates.Last().Rotation);

	//turn on collision
	GetOwner()->SetActorEnableCollision(true);

	//turn on physics
	if (!bIsActorFrozen)
	{
		if (bActorSimulatePhysics)
		{
			ActorComponentMesh->SetSimulatePhysics(true);
			ActorComponentMesh->SetPhysicsLinearVelocity(RewindStates.Last().Velocity);
			ActorComponentMesh->SetPhysicsAngularVelocity(RewindStates.Last().AngularVelocity);
		}
		//unpause timer function to record new states
		GetWorld()->GetTimerManager().UnPauseTimer(RecordStateTimerHandle);
	}
}


// Called every frame
void URewindComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//tick Rewindtimeline if playing
	RewindTimeline.TickTimeline(DeltaTime);
}

