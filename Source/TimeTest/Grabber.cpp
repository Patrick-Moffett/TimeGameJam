// Copyright Mcbright

#include "Grabber.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	SetupInputComponent();
	FindPhysicsHandle();
}

void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
}
void UGrabber::FindPhysicsHandle()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!PhysicsHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("No Physics Handle Component Found On %s! Add A Physics Handle Component."), *GetOwner()->GetName());
	}
}

// input function
void UGrabber::Grab()
{
	FHitResult HitResult = GetFirstPhysicsBodyInReach();

	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
	AActor* ActorHit = HitResult.GetActor();

	if (ActorHit)
	{
		if (!PhysicsHandle) { return; }
		PhysicsHandle->GrabComponentAtLocationWithRotation
		(
			ComponentToGrab,
			NAME_None,
			GetPlayersReach(),
			FRotator (0.f, 0.f, 0.f)
		);
		ActorHit->FindComponentByClass<UStaticMeshComponent>()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	}
}

// input function
void UGrabber::Release()
{
	if (!PhysicsHandle) { return; }
	auto grabbedComponent = PhysicsHandle->GetGrabbedComponent();
	if (grabbedComponent)
	{
		PhysicsHandle->GetGrabbedComponent()->GetOwner()->FindComponentByClass<UStaticMeshComponent>()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
		PhysicsHandle->ReleaseComponent();
	}
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PhysicsHandle) { return; }
	if (PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocation(GetPlayersReach());
	}
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
	FHitResult Hit;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	GetWorld()->LineTraceSingleByObjectType
	(
		OUT Hit,
		GetPlayersWorldPosition(),
		GetPlayersReach(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);

	return Hit;
}

FVector UGrabber::GetPlayersReach() const
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	// reach is the desired length of the ray cast
	return PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
}

FVector UGrabber::GetPlayersWorldPosition() const
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	return PlayerViewPointLocation;
}

