// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_PlayerInputManagerComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputSubsystems.h"
UC_PlayerInputManagerComponent::UC_PlayerInputManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	// BP로 만든 기본 IMC를 로드
	// 에디터에서 할당해 주는게 안정적일 지도?
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> DefaultIMCObj(TEXT("/Game/Project_PUBG/Common/Character/Input/IMC_Player.IMC_Player"));
	if (DefaultIMCObj.Succeeded())
	{
		DefaultPlayerInputMappingContext = DefaultIMCObj.Object;
	}
}

void UC_PlayerInputManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UC_PlayerInputManagerComponent::InitializeInput(APlayerController* Controller)
{
	CachedController = Controller;

	// IMC 생성
	UserMappingContext = NewObject<UInputMappingContext>();
	ApplyDefaultMappings();
	LoadUserKeySettings();
	EnsureNewActionsArePatched();

	// 등록
	//auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Controller->GetLocalPlayer());
	//Subsystem->AddMappingContext(UserMappingContext, 0);
}

void UC_PlayerInputManagerComponent::LoadUserKeySettings()
{
}

void UC_PlayerInputManagerComponent::SaveUserKeySettings()
{
}

void UC_PlayerInputManagerComponent::ResetToDefaultKeySettings()
{
}

void UC_PlayerInputManagerComponent::RebindKey(UInputAction* Action, FKey NewKey, bool bIsPrimary)
{
}

void UC_PlayerInputManagerComponent::ApplyDefaultMappings()
{
	if (!DefaultPlayerInputMappingContext || !UserMappingContext) return;


	for (const auto& Mapping : DefaultPlayerInputMappingContext->GetMappings())
	{
		UserMappingContext->MapKey(Mapping.Action, Mapping.Key);
	}
}

void UC_PlayerInputManagerComponent::EnsureNewActionsArePatched()
{
}


