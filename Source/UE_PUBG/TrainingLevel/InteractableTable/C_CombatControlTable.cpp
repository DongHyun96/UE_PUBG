// Fill out your copyright notice in the Description page of Project Settings.


#include "C_CombatControlTable.h"

#include "Character/C_Player.h"
#include "Character/Component/C_InputComponent.h"
#include "Components/CanvasPanel.h"
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

bool AC_CombatControlTable::OnFKeyInteraction()
{
	if (!bIsFocused) return false;

	UC_Util::Print("Hi This is f Key interaction testing message!", FColor::MakeRandomColor());

	AC_CombatFieldManager* CombatFieldManager = GAMESCENE_MANAGER->GetTrainingGroundManager()->GetCombatFieldManager();
	
	if (CombatFieldManager->GetEnemyCombatFieldManager()->GetIsPlaying())
		CombatFieldManager->StopEnemyVsEnemyRound();
	else
		CombatFieldManager->StartEnemyVsEnemyRound();

	return true;
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
	
	Player->GetInputComponent()->CombatControlFKeyInteractionDelegate.BindUObject(this, &AC_CombatControlTable::OnFKeyInteraction);
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
}

