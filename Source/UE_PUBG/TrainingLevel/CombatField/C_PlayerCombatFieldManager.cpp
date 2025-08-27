// Fill out your copyright notice in the Description page of Project Settings.


#include "C_PlayerCombatFieldManager.h"

#include "C_CombatFieldManager.h"
#include "C_PlayerCombatFieldWidget.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "Character/C_Enemy.h"
#include "Character/C_Player.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InputComponent.h"
#include "Character/Component/C_InvenComponent.h"
#include "Character/Component/C_InvenSystem.h"
#include "Character/Component/C_PlayerController.h"
#include "Character/Component/EnemyComponent/C_DefaultItemSpawnerComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ShapeComponent.h"
#include "Door/C_TutorialGate.h"
#include "HUD/C_HUDWidget.h"
#include "HUD/C_InformWidget.h"
#include "InvenUI/BasicItemSlot/WeaponSlot/GunSlot/C_GunSlotWidget.h"
#include "InvenUI/BasicItemSlot/WeaponSlot/GunSlot/C_MainGunSlotWidget.h"
#include "InvenUI/BasicItemSlot/WeaponSlot/GunSlot/C_SubGunSlotWidget.h"
#include "InvenUI/Panel/ItemPanel/EquipmentPanel/C_EquipmentPanelWidget.h"
#include "Item/Attachment/C_AttachableItem.h"
#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Singleton/C_GameSceneManager.h"
#include "Utility/C_Util.h"


UC_PlayerCombatFieldManager::UC_PlayerCombatFieldManager()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UC_PlayerCombatFieldManager::BeginPlay()
{
	Super::BeginPlay();

	/* PlayerVsEnemySpawnTransform 저장 */
	
	const UCapsuleComponent* VersusPlayerSpawnCapsuleComponent =
		Cast<UCapsuleComponent>(OwnerCombatFieldManager->GetDefaultSubobjectByName(TEXT("VersusPlayerSpawnTransform")));
	
	if (!VersusPlayerSpawnCapsuleComponent) UC_Util::Print("From AC_PlayerCombatFieldManager::BeginPlay : VersusPlayerSpawnCapsuleComponent missing!", FColor::Red, 10.f);
	else SpawnTransforms.Add(VersusPlayerSpawnCapsuleComponent->GetComponentTransform());

	if (CombatFieldEnemy)
	{
		SpawnTransforms.Add(CombatFieldEnemy->GetActorTransform());
		if (CombatFieldEnemy->GetBehaviorType() != EEnemyBehaviorType::CombatTest)
			UC_Util::Print("From AC_CombatFieldManager::BeginPlay : VersusPlayerEnemy Behavior Type is not COMBAT", FColor::Red, 10.f);
	}

	if (!CombatFieldStartGate)
	{
		UC_Util::Print("From UC_PlayerCombatFieldManager::BeginPlay : CombatFieldGate nullptr!", FColor::Red, 10.f);
		return;
	}

	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
	{
		CombatFieldStartGate->GetGateOpeningBoxComponent()->OnComponentBeginOverlap.AddDynamic(this, &UC_PlayerCombatFieldManager::OnGateOpeningBoxBeginOverlap);
		CombatFieldStartGate->GetGateOpeningBoxComponent()->OnComponentEndOverlap.AddDynamic(this, &UC_PlayerCombatFieldManager::OnGateOpeningBoxEndOverlap);
		
		CombatFieldStartGate->ToggleOpeningBoxTriggerEnabled(true);
	});

	if (IsValid(LookAction))
	{
		AC_PlayerController* PC = Cast<AC_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PC->InputComponent))
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &UC_PlayerCombatFieldManager::OnLookInput);
	}
	else UC_Util::Print("From AC_CombatFieldManager::BeginPlay : Invalid LookAction!", FColor::Red, 10.f);

	// Round 숫자로 Index를 맞출 예정이라 3판 2선이어도 4개를 집어넣음
	RoundResults.Init(EPlayerCombatRoundResult::NotPlayed, 4);
}

void UC_PlayerCombatFieldManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (CombatFieldState)
	{
	case EPlayerCombatFieldState::Idle:
		return;
	case EPlayerCombatFieldState::WaitingRoundStart:
	{
		CombatFieldTimer -= DeltaTime;

		// 게임 시작까지 남은 시간 UI로 띄우기
		UC_InformWidget* InstructionWidget = GAMESCENE_MANAGER->GetPlayer()->GetHUDWidget()->GetInformWidget(); 
		const int TimeLeft = static_cast<int>(CombatFieldTimer) + 1; 
		InstructionWidget->SetGameStartTimerText(TimeLeft);

		if (CombatFieldTimer > 0.f) return;

		// Round 시작 처리
		SetPlayerCombatFieldState(EPlayerCombatFieldState::PlayingRound);
		return;
	}
	case EPlayerCombatFieldState::PlayingRound:
	{
		CombatFieldTimer -= DeltaTime;

		// 남은 Round Time UI로 띄우기
		PlayerCombatFieldWidget->SetTopRoundTimerText(CombatFieldTimer);

		if (CombatFieldTimer > 0.f) return;

		// Round 끝 처리
		SetPlayerCombatFieldState(EPlayerCombatFieldState::RoundEnd);
		return;
	}
	case EPlayerCombatFieldState::RoundEnd:
	{
		// 남은 Round Time 0으로 초기화
		PlayerCombatFieldWidget->SetTopRoundTimerTextToZero();
		
		// Round 결과 처리
		
		
		
		return;
	}
	case EPlayerCombatFieldState::MatchingEnd:
		return;
	}
}

void UC_PlayerCombatFieldManager::SetCurrentRoundResult(EPlayerCombatRoundResult RoundResult)
{
	if (CurrentRound > RoundResults.Num())
	{
		UC_Util::Print("From UC_PlayerCombatFieldManager::SetCurrentRoundResult : CurrentRound exceeds total round count", FColor::Red, 10.f);		
		return;
	}
	
	RoundResults[CurrentRound] = RoundResult;
}

void UC_PlayerCombatFieldManager::SetPlayerCombatFieldState(EPlayerCombatFieldState FieldState)
{
	CombatFieldState = FieldState;

	AC_Player* Player = GAMESCENE_MANAGER->GetPlayer();

	switch (FieldState)
	{
	case EPlayerCombatFieldState::Idle:
	{
		// TODO : FKey Interaction으로 Toggle된 Start Box 활성화
		// 상단 나침반 활성화 etc...

		
		return;
	}
	case EPlayerCombatFieldState::WaitingRoundStart:
	{
		// Match Round에 맞추어 기본 설정들 처리
		OwnerCombatFieldManager->InitRoundForCombatCharacter(Player, SpawnTransforms[0]);
		OwnerCombatFieldManager->InitRoundForCombatCharacter(CombatFieldEnemy, SpawnTransforms[1]);
	
		Player->GetInvenSystem()->GetInvenUI()->UpdateWidget();

		// Round Start Timer 초기화 및 초 세기 (UI 띄우기)
		CombatFieldTimer = 5.f;

		UC_InformWidget* InformWidget = Player->GetHUDWidget()->GetInformWidget();
		InformWidget->ToggleGameStartTimerVisibility(true);
		InformWidget->SetGameStartTimerText(static_cast<int>(CombatFieldTimer) + 1);

		GAMESCENE_MANAGER->SetCurrentHUDMode(EHUDMode::IDLE);

		// Player Main IMC Round Start 이전까지 막고, 대체 IMC로 대체
		AC_PlayerController* PlayerController = Player->GetController<AC_PlayerController>();
		UInputMappingContext* PlayerMainIMC = Player->GetInputComponent()->MappingContext;
		PlayerController->RemoveIMCFromSubsystem(PlayerMainIMC);
		PlayerController->AddIMCToSubsystem(IMC_WaitRound, 0);
		
		return;
	}
	case EPlayerCombatFieldState::PlayingRound:
	{
		AC_PlayerController* PlayerController = Player->GetController<AC_PlayerController>();
		UInputMappingContext* PlayerMainIMC = Player->GetInputComponent()->MappingContext;
		
		PlayerController->RemoveIMCFromSubsystem(IMC_WaitRound);
		PlayerController->AddIMCToSubsystem(PlayerMainIMC, 0);
		
		UC_InformWidget* InformWidget = Player->GetHUDWidget()->GetInformWidget();
		InformWidget->ToggleGameStartTimerVisibility(false);

		PlayerCombatFieldWidget->ExecuteRoundStart(CurrentRound);
		
		// Round Start Timer 세팅
		CombatFieldTimer = 60.f;
		
		return;
	}
	case EPlayerCombatFieldState::RoundEnd:
		return;
	case EPlayerCombatFieldState::MatchingEnd:
		return;
	}
}

void UC_PlayerCombatFieldManager::OnGateOpeningBoxBeginOverlap
(
	UPrimitiveComponent*	OverlappedComp,
	AActor*					OtherActor,
	UPrimitiveComponent*	OtherComp,
	int32					OtherBodyIndex,
	bool					bFromSweep,
	const FHitResult&		SweepResult
)
{
	AC_Player* Player = Cast<AC_Player>(OtherActor);
	if (!Player) return;

	PlayerCombatFieldWidget->ToggleStartFKeyInstruction(true);
	GAMESCENE_MANAGER->GetPlayer()->GetInputComponent()->CombatControlFKeyInteractionDelegate.BindUObject(this, &UC_PlayerCombatFieldManager::OnStartGateFKeyInteraction);	
}

void UC_PlayerCombatFieldManager::OnGateOpeningBoxEndOverlap
(
	UPrimitiveComponent* OverlappedComponent,
	AActor*				 OtherActor,
	UPrimitiveComponent* OtherComp,
	int32				 OtherBodyIndex
)
{
	AC_Player* Player = Cast<AC_Player>(OtherActor);
	if (!Player) return;

	GAMESCENE_MANAGER->GetPlayer()->GetInputComponent()->CombatControlFKeyInteractionDelegate.Unbind();
	PlayerCombatFieldWidget->ToggleStartFKeyInstruction(false);
}

bool UC_PlayerCombatFieldManager::OnStartGateFKeyInteraction()
{
	// TODO : Match 끝난 뒤, 다시 활성화 시켜주기 (Match 끝나면 CombatField에서 나가되, OpeningGateBox 바깥에 Player 두기)
	CombatFieldStartGate->ToggleOpeningBoxTriggerEnabled(false);

	UC_Util::Print("START PLAYER COMBAT FIELD", FColor::MakeRandomColor(), 10.f);

	/* Init Matching */
		
	// Player Inven check -> Combat에 필요한 아이템으로 초기화

	AC_Player* Player = GAMESCENE_MANAGER->GetPlayer();

	// Combat에 필요한 아이템으로 Player 아이템 초기화
	Player->GetInvenComponent()->ClearInventory();
	Player->GetEquippedComponent()->ClearEquippedWeapons();
	CombatFieldEnemy->GetItemSpawnerHelper()->SpawnDefaultWeaponsAndItemsForCombatFieldCharacter(Player);

	// Player 총기 부착물 추가 부착 처리
	FActorSpawnParameters SpawnParam{};
	SpawnParam.Owner = Player;
	
	// Spawn AR Attachables
	AC_AttachableItem* VertGrip 	= GetWorld()->SpawnActor<AC_AttachableItem>(GunAttachmentClasses[EAttachmentNames::VERTGRIP],	 SpawnParam);
	AC_AttachableItem* Compensator	= GetWorld()->SpawnActor<AC_AttachableItem>(GunAttachmentClasses[EAttachmentNames::COMPENSATOR], SpawnParam);
	AC_AttachableItem* RedDot		= GetWorld()->SpawnActor<AC_AttachableItem>(GunAttachmentClasses[EAttachmentNames::REDDOT],		 SpawnParam);
	
	// Spawn SR Attachable
	AC_AttachableItem* Scope = GetWorld()->SpawnActor<AC_AttachableItem>(GunAttachmentClasses[EAttachmentNames::SCOPE4], SpawnParam);
	
	UC_GunSlotWidget*	 MainGunSlotWidget	= Player->GetInvenSystem()->GetInvenUI()->GetEquipmentPanel()->GetMainGunSlot();
	UC_SubGunSlotWidget* SubGunSlotWidget	= Player->GetInvenSystem()->GetInvenUI()->GetEquipmentPanel()->GetSubGunSlot();
	AC_Weapon* 	 		 MainGun			= Player->GetEquippedComponent()->GetWeapons()[EWeaponSlot::MAIN_GUN];
	AC_Weapon* 	 		 SubGun				= Player->GetEquippedComponent()->GetWeapons()[EWeaponSlot::SUB_GUN];

	if (MainGun)
	{
		MainGunSlotWidget->SetAttachmentSlotOnDrop(MainGun, VertGrip);
		MainGunSlotWidget->SetAttachmentSlotOnDrop(MainGun, Compensator);
		MainGunSlotWidget->SetAttachmentSlotOnDrop(MainGun, RedDot);
	}
	else UC_Util::Print("From UC_PlayerCombatFieldManager::OnStartGateFKeyInteraction : Main Gun spawned failed!", FColor::Red, 10.f);

	if (SubGun) SubGunSlotWidget->SetAttachmentSlotOnDrop(SubGun, Scope);
	else UC_Util::Print("From UC_PlayerCombatFieldManager::OnStartGateFKeyInteraction : Sub Gun spawned failed!", FColor::Red, 10.f);

	// 상단 나침반 가리기 및 TopRoundTimerPanel 보이게끔 처리
	PlayerCombatFieldWidget->ToggleTopRoundTimerPanelVisibility(true);
	Player->GetHUDWidget()->ToggleCompassBarVisibility(false);
	// GAMESCENE_MANAGER->SetCurrentHUDMode(EHUDMode::IDLE);
	
	SetPlayerCombatFieldState(EPlayerCombatFieldState::WaitingRoundStart);
	return true;
}

void UC_PlayerCombatFieldManager::OnLookInput(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	GAMESCENE_MANAGER->GetPlayer()->AddControllerYawInput(LookAxisVector.X);
	GAMESCENE_MANAGER->GetPlayer()->AddControllerPitchInput(LookAxisVector.Y);
}
