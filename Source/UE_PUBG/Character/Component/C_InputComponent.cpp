// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_InputComponent.h"
#include "Components/ActorComponent.h"
#include "UE_PUBG/Character/C_Player.h"
#include "EnhancedInputComponent.h"


// Sets default values for this component's properties
UC_InputComponent::UC_InputComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UC_InputComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void UC_InputComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UC_InputComponent::BindAction(UInputComponent* PlayerInputComponent, AC_Player* Player)
{
	//Player->SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (IsValid(EnhancedInputComponent))
	{
		//EnhancedInputComponent->BindAction(MyInputComponent->JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, Player, &AC_Player::OnJump);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, Player, &AC_Player::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, Player, &AC_Player::MoveEnd);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, Player, &AC_Player::Look);

		EnhancedInputComponent->BindAction(CrawlAction, ETriggerEvent::Triggered, Player, &AC_Player::Crawl);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, Player, &AC_Player::Crouch);

	}
}



