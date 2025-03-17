// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/SkyDivingComponent/C_EnemySkyDivingComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Utility/C_Util.h"

UC_EnemySkyDivingComponent::UC_EnemySkyDivingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UC_EnemySkyDivingComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UC_EnemySkyDivingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// UC_Util::Print(OwnerCharacter->GetVelocity());
}

void UC_EnemySkyDivingComponent::SetStateToSkyDivingState()
{
	// skyDiving EndPoint까지의 Velocity 잡아주기 & Anim NextSpeed 지정해주기
	UCharacterMovementComponent* OwnerMovement = OwnerCharacter->GetCharacterMovement();
	OwnerMovement->SetMovementMode(MOVE_Falling); // z velocity 바로 0되는 것 방지하기 위함
	OwnerMovement->AirControl = 1.f;
	OwnerMovement->BrakingDecelerationFalling = 0.f;
	
	FVector Direction = SkyDivingStateDestination - OwnerCharacter->GetActorLocation();
	Direction.Normalize();
	
	OwnerMovement->Velocity = Direction * 4500.f;

	// BS 속도 setting
	OwnerCharacter->SetNextSpeed(400.f);
}

void UC_EnemySkyDivingComponent::SetStateToParachutingState()
{
	UC_Util::Print("Set to Parachute State!", FColor::MakeRandomColor(), 10.f);
	// Target Landing Location 까지의 Velocity 잡아주기
	FVector Direction = ParachutingStateDestination - OwnerCharacter->GetActorLocation();
	Direction.Normalize();

	OwnerCharacter->GetCharacterMovement()->Velocity = Direction * 700.f;

	// BS 속도 Setting
	OwnerCharacter->SetNextSpeed(500.f);
}

void UC_EnemySkyDivingComponent::SetStateToLandingState()
{
	UC_Util::Print("Set to Landing State!", FColor::MakeRandomColor(), 10.f);
}
