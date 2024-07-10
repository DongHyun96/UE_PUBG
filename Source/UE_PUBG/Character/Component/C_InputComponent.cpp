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

		EnhancedInputComponent->BindAction(CrawlAction, ETriggerEvent::Started, Player, &AC_Player::Crawl);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, Player, &AC_Player::Crouch);
		EnhancedInputComponent->BindAction(HoldDirectionAction, ETriggerEvent::Triggered, Player, &AC_Player::HoldDirection);
		EnhancedInputComponent->BindAction(HoldDirectionAction, ETriggerEvent::Completed, Player, &AC_Player::ReleaseDirection);

		EnhancedInputComponent->BindAction(Num1Action, ETriggerEvent::Started, Player, &AC_Player::OnNum1);
		EnhancedInputComponent->BindAction(Num2Action, ETriggerEvent::Started, Player, &AC_Player::OnNum2);
		EnhancedInputComponent->BindAction(Num4Action, ETriggerEvent::Started, Player, &AC_Player::OnNum4);
		EnhancedInputComponent->BindAction(Num5Action, ETriggerEvent::Started, Player, &AC_Player::OnNum5);

		EnhancedInputComponent->BindAction(XAction, ETriggerEvent::Started,     Player, &AC_Player::OnXKey);
																			    
		EnhancedInputComponent->BindAction(BAction, ETriggerEvent::Started,		Player, &AC_Player::OnBKey);
		EnhancedInputComponent->BindAction(RAction, ETriggerEvent::Started,     Player, &AC_Player::OnRKey);

		EnhancedInputComponent->BindAction(MLBAction, ETriggerEvent::Started,   Player, &AC_Player::OnMLBStarted);
		EnhancedInputComponent->BindAction(MLBAction, ETriggerEvent::Ongoing,   Player, &AC_Player::OnMLBOnGoing);
		EnhancedInputComponent->BindAction(MLBAction, ETriggerEvent::Completed, Player, &AC_Player::OnMLBCompleted);

		EnhancedInputComponent->BindAction(MRBAction, ETriggerEvent::Started,   Player, &AC_Player::OnMRBStarted);
		EnhancedInputComponent->BindAction(MRBAction, ETriggerEvent::Ongoing,   Player, &AC_Player::OnMRBOnGoing);
		EnhancedInputComponent->BindAction(MRBAction, ETriggerEvent::Completed, Player, &AC_Player::OnMRBCompleted);
	}
}



