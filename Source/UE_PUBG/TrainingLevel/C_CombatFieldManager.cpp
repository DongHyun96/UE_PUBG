// Fill out your copyright notice in the Description page of Project Settings.


#include "C_CombatFieldManager.h"

#include "AI/C_BehaviorComponent.h"
#include "AI/C_EnemyAIController.h"
#include "AI/Task/CombatTask/C_BTTaskSwapWeapon.h"
#include "Character/C_Enemy.h"
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

	/* for (int i = 0; i < 2; ++i)
	{
		EnemyVsEnemySpawnTransform.Add(VersusAIEnemies[i]->GetActorTransform());
		PlayerVsEnemySpawnTransform.Add(VersusPlayerEnemies[i]->GetActorTransform());
	} */

	/* PlayerVsEnemySpawnTransform 저장 */
	const UCapsuleComponent* VersusPlayerSpawnCapsuleComponent = Cast<UCapsuleComponent>(GetDefaultSubobjectByName(TEXT("VersusPlayerSpawnTransform")));
	if (!VersusPlayerSpawnCapsuleComponent) UC_Util::Print("From AC_CombatFieldManager::BeginPlay : VersusPlayerSpawnCapsuleComponent missing!", FColor::Red, 10.f);
	else PlayerVsEnemySpawnTransform.Add(VersusPlayerSpawnCapsuleComponent->GetComponentTransform());

	PlayerVsEnemySpawnTransform.Add(VersusPlayerEnemy->GetActorTransform());
	if (VersusPlayerEnemy->GetBehaviorType() != EEnemyBehaviorType::CombatTest)
		UC_Util::Print("From AC_CombatFieldManager::BeginPlay : VersusPlayerEnemy Behavior Type is not COMBAT", FColor::Red, 10.f);

	/* EnemyVsEnemySpawnTransform 저장 */
	for (const AC_Enemy* Enemy : VersusAIEnemies)
	{
		if (Enemy->GetBehaviorType() != EEnemyBehaviorType::CombatTest)
			UC_Util::Print("From AC_CombatFieldManager::BeginPlay : VersusEnemy Behavior Type is not COMBAT", FColor::Red, 10.f);
			
		EnemyVsEnemySpawnTransform.Add(Enemy->GetActorTransform());
	}

	// 서로 TargetCharacter로 잡도록 setting
	VersusAIEnemies[0]->GetController<AC_EnemyAIController>()->GetBehaviorComponent()->SetTargetCharacter(VersusAIEnemies[1]);
	VersusAIEnemies[1]->GetController<AC_EnemyAIController>()->GetBehaviorComponent()->SetTargetCharacter(VersusAIEnemies[0]);

	// VersusPlayerEnemy의 TargetCharacter 세팅은 P vs E 라운드가 시작되기 직전에 잡아주기
}

void AC_CombatFieldManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

