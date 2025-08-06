// Fill out your copyright notice in the Description page of Project Settings.


#include "C_CombatFieldManager.h"

#include "Character/C_Player.h"
#include "Components/CapsuleComponent.h"
#include "Singleton/C_GameSceneManager.h"
#include "Utility/C_Util.h"


AC_CombatFieldManager::AC_CombatFieldManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_CombatFieldManager::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < 2; ++i)
	{
		FString VersusPlayerCapsuleComponentName = "VersusPlayerSpawnTransform" + FString::FromInt(i);
		UCapsuleComponent* VersusPlayerSpawnCapsule = Cast<UCapsuleComponent>(GetDefaultSubobjectByName(FName(*VersusPlayerCapsuleComponentName)));

		if (!VersusPlayerSpawnCapsule) UC_Util::Print("From AC_CombatFieldManager::BeginPlay : Invalid VersusPlayerSpawnCapsule!", FColor::Red, 10.f);
		else PlayerVsEnemySpawnTransform.Add(VersusPlayerSpawnCapsule);

		FString VersusEnemyCapsuleComponentName = "VersusEnemySpawnTransform" + FString::FromInt(i);
		UCapsuleComponent* VersusEnemySpawnCapsule = Cast<UCapsuleComponent>(GetDefaultSubobjectByName(FName(*VersusEnemyCapsuleComponentName)));

		if (!VersusEnemySpawnCapsule) UC_Util::Print("From AC_CombatFieldManager::BeginPlay : Invalid VersusEnemySpawnCapsule!", FColor::Red, 10.f);
		else EnemyVsEnemySpawnTransform.Add(VersusEnemySpawnCapsule);
	}
	
	SpawnParameters.Owner = this;

	// For testing
	GAMESCENE_MANAGER->GetPlayer()->SetActorTransform(PlayerVsEnemySpawnTransform[0]->GetComponentTransform());

	// Enemy vs Enemy 용 Enemies Spawn 처리
	// Spawn 처리 X -> Level에 있는 캐릭터 계속 재활용 해야 할듯
	/*for (const UCapsuleComponent* CapsuleComponent : EnemyVsEnemySpawnTransform)
	{
		AC_Enemy* Enemy = GetWorld()->SpawnActor<AC_Enemy>(EnemyClass, SpawnParameters);
		Enemy->SetActorTransform(CapsuleComponent->GetComponentTransform());
		VersusAIEnemies.Add(Enemy);
	}

	// 서로 TargetCharacter로 잡도록 setting
	VersusAIEnemies[0]->GetController<AC_EnemyAIController>()->GetBehaviorComponent()->SetTargetCharacter(VersusAIEnemies[1]);
	VersusAIEnemies[1]->GetController<AC_EnemyAIController>()->GetBehaviorComponent()->SetTargetCharacter(VersusAIEnemies[0]);*/
}

void AC_CombatFieldManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

