// Fill out your copyright notice in the Description page of Project Settings.


#include "C_JumpTrainingTable.h"

#include "C_JumpTrainingWidget.h"
#include "EnhancedInputComponent.h"
#include "AI/C_BehaviorComponent.h"
#include "AI/C_EnemyAIController.h"
#include "Character/C_Enemy.h"
#include "Character/C_Player.h"
#include "Character/Component/C_InputComponent.h"
#include "Character/Component/C_InvenComponent.h"
#include "Character/Component/C_PlayerController.h"
#include "Character/Component/SkyDivingComponent/C_PlayerSkyDivingComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "HUD/C_HUDWidget.h"
#include "HUD/C_InformWidget.h"
#include "Item/Equipment/C_EquipableItem.h"
#include "Kismet/GameplayStatics.h"
#include "Singleton/C_GameSceneManager.h"
#include "TrainingLevel/C_AISkyDiveTesterManager.h"
#include "TrainingLevel/C_TrainingGroundManager.h"
#include "Utility/C_Util.h"

AC_JumpTrainingTable::AC_JumpTrainingTable()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_JumpTrainingTable::BeginPlay()
{
	Super::BeginPlay();

	SkyDivingStartLocation = Cast<USceneComponent>(GetDefaultSubobjectByName(TEXT("SkyDivingStartLocation")));
	if (!SkyDivingStartLocation) UC_Util::Print("From AC_JumpTrainingTable::BeginPlay : Invalid SkyDivingStartLocation!", FColor::Red, 10.f);

	AC_PlayerController* PC = Cast<AC_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PC->InputComponent))
	{
		EnhancedInputComponent->BindAction(Num1Action, ETriggerEvent::Started, this, &AC_JumpTrainingTable::OnNum1Key);
		EnhancedInputComponent->BindAction(Num2Action, ETriggerEvent::Started, this, &AC_JumpTrainingTable::OnNum2Key);
		EnhancedInputComponent->BindAction(FKeyAction, ETriggerEvent::Started, this, &AC_JumpTrainingTable::OnFKey);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AC_JumpTrainingTable::OnLookInput);
	}
}

void AC_JumpTrainingTable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Lazy Initialization
	if (!JumpTrainingWidget)
	{
		AC_TrainingGroundManager* TrainingGroundManager = GAMESCENE_MANAGER->GetTrainingGroundManager();
		if (!TrainingGroundManager)
		{
			UC_Util::Print("From AC_JumpTrainingTable::Tick : TrainingGroundManager is not valid!", FColor::Red);
			return;
		}

		AC_AISkyDiveTesterManager* AISkyDiveTesterManager = TrainingGroundManager->GetAISkyDiveTesterManager();
		if (!AISkyDiveTesterManager)
		{
			UC_Util::Print("From AC_JumpTrainingTable::Tick : AISkyDiveTesterManager is not valid!", FColor::Red);
			return;
		}

		JumpTrainingWidget = AISkyDiveTesterManager->GetJumpTrainingWidget();
	}

	// Set InteractionInfoWidget RenderOpacity
	JumpTrainingWidget->SetRenderOpacity(1.f - TriangleWidget->GetRenderOpacity());
}

bool AC_JumpTrainingTable::OnNumber1KeyInteraction()
{
	if (!bIsFocused) return false;
	
	AC_Player* Player = GAMESCENE_MANAGER->GetPlayer();
	
	Player->SetActorLocation(SkyDivingStartLocation->GetComponentLocation());
	Player->SetMainState(EMainState::SKYDIVING);
	Player->GetPlayerSkyDivingComponent()->SetSkyDivingState(ESkyDivingState::SKYDIVING);

	AC_EquipableItem* BackPack = Player->GetInvenComponent()->GetEquipmentItems()[EEquipSlot::BACKPACK];
	if (!BackPack) return true;

	// 배낭을 장착하고 있다면, 배낭 잠시 숨기기
	BackPack->SetActorHiddenInGame(true);
	return true;
}

bool AC_JumpTrainingTable::OnNumber2KeyInteraction()
{
	if (!bIsFocused) return false;

	AC_TrainingGroundManager* TrainingGroundManager = GAMESCENE_MANAGER->GetTrainingGroundManager();
	if (!TrainingGroundManager) return false;
	
	AC_Enemy* SkyDivingTester = TrainingGroundManager->GetAISkyDiveTesterManager()->GetSkyDiveTester();
	if (!SkyDivingTester) return false;

	if (SkyDivingTester->GetMainState() == EMainState::SKYDIVING)
	{
		GAMESCENE_MANAGER->GetPlayer()->GetHUDWidget()->GetInformWidget()->AddPlayerWarningLog("AI SkyDiving test already in progress. Look to the skies!");
		return true;
	}

	SkyDivingTester->GetController<AC_EnemyAIController>()->GetBehaviorComponent()->SetServiceType(EServiceType::SKYDIVE);
	return true;
}

bool AC_JumpTrainingTable::OnFKeyInteraction()
{
	if (!bIsFocused) return false;
	if (GAMESCENE_MANAGER->GetPlayer()->GetMainState() == EMainState::SKYDIVING) return false;

	AC_TrainingGroundManager* TrainingGroundManager = GAMESCENE_MANAGER->GetTrainingGroundManager();
	if (!TrainingGroundManager) return false;

	AC_AISkyDiveTesterManager* SkyDiveTesterManager = TrainingGroundManager->GetAISkyDiveTesterManager();

	// Toggle Spectating Camera
	SkyDiveTesterManager->ToggleSpectatingSkyDiveTester();
	
	return true;
}

void AC_JumpTrainingTable::OnFKey()
{
	// 현재 SkyDiveTester를 관전중일 때 받을 Input
	UC_Util::Print("JumpTrainingTable::OnFKey");
	
	// Player inputComponent의 F Key와 중복된 input 처리로 인한 두번 처리 방지
	AC_AISkyDiveTesterManager* AISkyDiveTesterManager = GAMESCENE_MANAGER->GetTrainingGroundManager()->GetAISkyDiveTesterManager();
	if (!AISkyDiveTesterManager->GetIsSpectatingSkyDiveTester()) return;

	// Toggle Spectating Camera
	AISkyDiveTesterManager->ToggleSpectatingSkyDiveTester();
}

void AC_JumpTrainingTable::OnNum1Key()
{
	// 현재 SkyDiveTester를 관전중일 때 받을 Input
	UC_Util::Print("AC_JumpTrainingTable::OnNum1Key", FColor::MakeRandomColor());
	
	// Player inputComponent의 F Key와 중복된 input 처리로 인한 두번 처리 방지
	AC_AISkyDiveTesterManager* AISkyDiveTesterManager = GAMESCENE_MANAGER->GetTrainingGroundManager()->GetAISkyDiveTesterManager();
	if (!AISkyDiveTesterManager->GetIsSpectatingSkyDiveTester()) return;

	// 관전 처리 해제
	AISkyDiveTesterManager->ToggleSpectatingSkyDiveTester();

	// Player SkyDiveTesting 실행
	OnNumber1KeyInteraction();
}

void AC_JumpTrainingTable::OnNum2Key()
{
	// 현재 SkyDiveTester를 관전중일 때 받을 Input
	UC_Util::Print("AC_JumpTrainingTable::OnNum2Key", FColor::MakeRandomColor());
	
	// Player inputComponent의 F Key와 중복된 input 처리로 인한 두번 처리 방지
	AC_AISkyDiveTesterManager* AISkyDiveTesterManager = GAMESCENE_MANAGER->GetTrainingGroundManager()->GetAISkyDiveTesterManager();
	if (!AISkyDiveTesterManager->GetIsSpectatingSkyDiveTester()) return;

	OnNumber2KeyInteraction();
}

void AC_JumpTrainingTable::OnLookInput(const FInputActionValue& Value)
{
	// 현재 SkyDiveTester를 관전중일 때 받을 Input
	
	AC_AISkyDiveTesterManager* AISkyDiveTesterManager = GAMESCENE_MANAGER->GetTrainingGroundManager()->GetAISkyDiveTesterManager();
	if (!AISkyDiveTesterManager->GetIsSpectatingSkyDiveTester()) return;
	
	AC_Enemy* Tester = AISkyDiveTesterManager->GetSkyDiveTester();

	FVector2D LookAxisVector = Value.Get<FVector2D>();

	FRotator SpringArmRotation	 = Tester->GetSpringArmComponent()->GetRelativeRotation();
	SpringArmRotation.Yaw		-= LookAxisVector.X;
	SpringArmRotation.Pitch		 = FMath::Clamp(SpringArmRotation.Pitch - LookAxisVector.Y, -80.f, 80.f); // 상하 범위 제한

	Tester->GetSpringArmComponent()->SetRelativeRotation(SpringArmRotation);
}

void AC_JumpTrainingTable::OnBoxColliderBeginOverlap
(
	UPrimitiveComponent*	OverlappedComp,
	AActor*					OtherActor,
	UPrimitiveComponent*	OtherComp,
	int32					OtherBodyIndex,
	bool					bFromSweep,
	const FHitResult&		SweepResult
)
{
	Super::OnBoxColliderBeginOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	AC_Player* Player = Cast<AC_Player>(OtherActor);
	if (!Player) return;

	// Table interaction 처리 함수 구독
	Player->GetInputComponent()->JumpTrainingTableNum1KeyInteractionDelegate.BindUObject(this, &AC_JumpTrainingTable::OnNumber1KeyInteraction);
	Player->GetInputComponent()->JumpTrainingTableNum2KeyInteractionDelegate.BindUObject(this, &AC_JumpTrainingTable::OnNumber2KeyInteraction);
	Player->GetInputComponent()->JumpTrainingTableFKeyInteractionDelegate.BindUObject(this, &AC_JumpTrainingTable::OnFKeyInteraction);

	// Spectator 전용키 Input Mapping context 추가
	AC_PlayerController* PlayerController = Player->GetController<AC_PlayerController>();
	if (!PlayerController)
	{
		UC_Util::Print("From AC_JumpTrainingTable::OnBoxColliderBeginOverlap : Invalid PlayerController!", FColor::Red, 10.f);
		return;
	}

	PlayerController->AddIMCToSubsystem(IMC_Spectator, -1);
}

void AC_JumpTrainingTable::OnBoxColliderEndOverlap
(
	UPrimitiveComponent*	OverlappedComp,
	AActor*					OtherActor,
	UPrimitiveComponent*	OtherComp,
	int32					OtherBodyIndex
)
{
	Super::OnBoxColliderEndOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex);

	AC_Player* Player = Cast<AC_Player>(OtherActor);
	if (!Player) return;
	
	// Table interaction delegate 구독 해제
	Player->GetInputComponent()->JumpTrainingTableNum1KeyInteractionDelegate.Unbind();
	Player->GetInputComponent()->JumpTrainingTableNum2KeyInteractionDelegate.Unbind();
	Player->GetInputComponent()->JumpTrainingTableFKeyInteractionDelegate.Unbind();

	// Spectator 전용키 Input Mapping Context 제거
	AC_PlayerController* PlayerController = Player->GetController<AC_PlayerController>();
	if (!PlayerController)
	{
		UC_Util::Print("From AC_JumpTrainingTable::OnBoxColliderEndOverlap : Invalid PlayerController!", FColor::Red, 10.f);
		return;
	}

	PlayerController->RemoveIMCFromSubsystem(IMC_Spectator);
}
