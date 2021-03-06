// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealCourse.h"
#include "Grabber.h"
#define OUT //Just to know when a param its out in the func call.


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
	
	LookForPhysicsHandle();
	SetupInputComponent();
	
}

void UGrabber::LookForPhysicsHandle()
{
	PhysicsHandle = Player->FindComponentByClass<UPhysicsHandleComponent>();
	if (!PhysicsHandle) 
	{
		UE_LOG(LogTemp, Error, TEXT("No physics handle component in actor %s, please add one."), *Player->GetName())
	}
}

void UGrabber::SetupInputComponent()
{
	InputComponent = Player->FindComponentByClass<UInputComponent>();
	if (!InputComponent)
		UE_LOG(LogTemp, Error, TEXT("No input component in actor %s."), *Player->GetName())
	else
	{
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
		

}

void UGrabber::Grab()
{
	if (!PhysicsHandle) { return; } 

	FHitResult HitResult = GetPhysicsBodyInReach();
	if (HitResult.GetActor()) //If there is an Actor in the HitResult, then attach to Physics Handle  
	{
		UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent(); //Gets to mesh in our case
		PhysicsHandle->GrabComponent(ComponentToGrab, NAME_None, ComponentToGrab->GetOwner()->GetActorLocation(), true);
	}
}

void UGrabber::Release()
{
	if (!PhysicsHandle) { return; }
	PhysicsHandle->ReleaseComponent();
}

const FHitResult UGrabber::GetPhysicsBodyInReach()
{

	FHitResult Hit;
	FCollisionObjectQueryParams ObjectTypes(ECollisionChannel::ECC_PhysicsBody); //Saying we want to target objects in the PhysicsBody channel
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, Player);	

	GetWorld()->LineTraceSingleByObjectType(OUT Hit, Player->GetActorLocation(), GetRaycastEndPoint(), ObjectTypes, TraceParameters);

	return Hit;
}

FVector UGrabber::GetRaycastEndPoint()
{
	FVector ViewPointLocation;
	FRotator ViewPointRotator;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(ViewPointLocation, ViewPointRotator);// I pass two empty variables to this function and it will modify them on the inside with the PlayerViewPoint values.

	FVector RaycastVectorEnd = ViewPointLocation + ViewPointRotator.Vector()*Reach; //Basic vector addition.
	return RaycastVectorEnd;
}
// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	if (!PhysicsHandle) { return; }

	if (PhysicsHandle->GrabbedComponent) //Check if we have a grabbed component
		PhysicsHandle->SetTargetLocation(GetRaycastEndPoint());
	
}
