// Fill out your copyright notice in the Description page of Project Settings.


#include "C_CombatControlTable.h"

#include "EnhancedInputComponent.h"
#include "Character/C_Enemy.h"
#include "Character/C_Player.h"
#include "Character/Component/C_InputComponent.h"
#include "Character/Component/C_PlayerController.h"
#include "Components/CanvasPanel.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Singleton/C_GameSceneManager.h"
#include "TrainingLevel/C_TrainingGroundManager.h"
#include "TrainingLevel/CombatField/C_CombatFieldManager.h"
#include "TrainingLevel/CombatField/C_CombatFieldWidget.h"
#include "TrainingLevel/CombatField/C_EnemyCombatFieldManager.h"
#include "Utility/C_Util.h"


AC_CombatControlTable::AC_CombatControlTable()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_CombatControlTable::BeginPlay()
{
	Super::BeginPlay();

	AC_PlayerController* PC = Cast<AC_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PC->InputComponent))
	{
		EnhancedInputComponent->BindAction(ArrowLeftAction, ETriggerEvent::Started, this, &AC_CombatControlTable::OnArrowLeft);
		EnhancedInputComponent->BindAction(ArrowRightAction, ETriggerEvent::Started, this, &AC_CombatControlTable::OnArrowRight);
		EnhancedInputComponent->BindAction(FKeyAction, ETriggerEvent::Started, this, &AC_CombatControlTable::OnFKey);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AC_CombatControlTable::OnLookInput);
	}
}

void AC_CombatControlTable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Lazy initialization
	if (!CombatFieldWidget)
	{
		AC_TrainingGroundManager* TrainingGroundManager = GAMESCENE_MANAGER->GetTrainingGroundManager();
		if (!TrainingGroundManager)
		{
			UC_Util::Print("From AC_CombatControlTable::Tick : TrainingGroundManager is not valid!", FColor::Red, 10.f);
			return;
		}

		AC_CombatFieldManager* CombatFieldManager = TrainingGroundManager->GetCombatFieldManager();
		if (!CombatFieldManager)
		{
			UC_Util::Print("From AC_CombatControlTable::Tick : CombatFieldManager is not valid!", FColor::Red, 10.f);
			return;
		}
		
		CombatFieldWidget = GAMESCENE_MANAGER->GetTrainingGroundManager()->GetCombatFieldManager()->GetCombatFieldWidget();
		if (!CombatFieldWidget)
		{
			UC_Util::Print("From AC_CombatControlTable::Tick : CombatFieldWidget is not valid!", FColor::Red, 10.f);
			return;
		}
	}

	CombatFieldWidget->GetCombatSimulationPanel()->SetRenderOpacity(1.f - TriangleWidget->GetRenderOpacity());
}

bool AC_CombatControlTable::OnPlayerInputComponentFKeyDelegate()
{
	if (!bIsFocused) return false;
	AC_CombatFieldManager* CombatFieldManager = GAMESCENE_MANAGER->GetTrainingGroundManager()->GetCombatFieldManager();

	CombatFieldManager->GetEnemyCombatFieldManager()->GetIsPlaying() ?
		CombatFieldManager->StopEnemyVsEnemyRound() : CombatFieldManager->StartEnemyVsEnemyRound();

	return true;
}

void AC_CombatControlTable::OnFKey()
{
	ESpectatorType CurrentSpectatorType =	GAMESCENE_MANAGER->GetTrainingGroundManager()->
											GetCombatFieldManager()->GetEnemyCombatFieldManager()->GetCurrentSpectatorType();
	
	if (CurrentSpectatorType == ESpectatorType::Player) return;
	
	OnPlayerInputComponentFKeyDelegate();
}

void AC_CombatControlTable::OnArrowLeft()
{
	UC_Util::Print("ArrowLeft");
	GAMESCENE_MANAGER->GetTrainingGroundManager()->GetCombatFieldManager()->GetEnemyCombatFieldManager()->DecreaseSpectatorType();
}

void AC_CombatControlTable::OnArrowRight()
{
	UC_Util::Print("ArrowRight");
	GAMESCENE_MANAGER->GetTrainingGroundManager()->GetCombatFieldManager()->GetEnemyCombatFieldManager()->IncreaseSpectatorType();
}

void AC_CombatControlTable::OnLookInput(const FInputActionValue& Value)
{
	UC_EnemyCombatFieldManager* EnemyCombatFieldManager =	GAMESCENE_MANAGER->GetTrainingGroundManager()->
															GetCombatFieldManager()->GetEnemyCombatFieldManager();
	
	// Enemy 관전 카메라 처리에만 쓰일 예정
	const AC_Enemy* CurrentSpectatingEnemy = EnemyCombatFieldManager->GetCurrentSpectatingEnemy();
	if (!CurrentSpectatingEnemy) return;

	FVector2D LookAxisVector = Value.Get<FVector2D>();

	FRotator SpringArmRotation	 = CurrentSpectatingEnemy->GetSpringArmComponent()->GetRelativeRotation();
	SpringArmRotation.Yaw		-= LookAxisVector.X;
	SpringArmRotation.Pitch		 = FMath::Clamp(SpringArmRotation.Pitch - LookAxisVector.Y, -80.f, 80.f); // 상하 범위 제한

	CurrentSpectatingEnemy->GetSpringArmComponent()->SetRelativeRotation(SpringArmRotation);
}

void AC_CombatControlTable::OnBoxColliderBeginOverlap
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

	// 해당 BoxCollider에 잡혔을 떄에 Interaction Key Delegate 걸기 (RetVal 때문에 Dynamic Delegate가 아닌 단일 Delegate를 사용하기 때문에,
	// 해당 Interaction 영역에 들어왔을 때 구독 처리를 해 둠
	
	if (Player->GetInputComponent()->CombatControlFKeyInteractionDelegate.IsBound())
	{
		UC_Util::Print
		(
			"From AC_CombatControlTable::OnBoxColliderBeginOverlap : WARNING : FKeyInteractionDelegate already bound to other table object!",
			FColor::Red, 10.f
		);
	}
	
	Player->GetInputComponent()->CombatControlFKeyInteractionDelegate.BindUObject(this, &AC_CombatControlTable::OnPlayerInputComponentFKeyDelegate);

	// Spectator 전용키 Input Mapping Context 추가
	AC_PlayerController* PlayerController = Player->GetController<AC_PlayerController>();
	if (!PlayerController)
	{
		UC_Util::Print("From AC_CombatControlTable::OnBoxColliderBeginOverlap : Invalid PlayerController!", FColor::Red, 10.f);
		return;
	}

	PlayerController->AddIMCToSubsystem(IMC_CombatSpectator, -1);
}

void AC_CombatControlTable::OnBoxColliderEndOverlap
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

	// TODO : 맵으로 클릭해서 순간이동 처리 시, 잘못 처리될 수도 있을 것 같은 생각이 들긴 하네
	
	if (!Player->GetInputComponent()->CombatControlFKeyInteractionDelegate.IsBound())
	{
		UC_Util::Print
		(
			"From AC_CombatControlTable::OnBoxColliderEndOverlap : WARNING : FKeyInteractionDelegate not bound!",
			FColor::Red, 10.f
		);
		return;
	}

	Player->GetInputComponent()->CombatControlFKeyInteractionDelegate.Unbind();

	// Spectator 전용키 Input Mapping Context 제거
	AC_PlayerController* PlayerController = Player->GetController<AC_PlayerController>();
	if (!PlayerController)
	{
		UC_Util::Print("From AC_CombatControlTable::OnBoxColliderEndOverlap : Invalid PlayerController!", FColor::Red, 10.f);
		return;
	}

	PlayerController->RemoveIMCFromSubsystem(IMC_CombatSpectator);
}

