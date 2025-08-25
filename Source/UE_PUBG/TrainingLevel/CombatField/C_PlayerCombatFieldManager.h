// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_PlayerCombatFieldManager.generated.h"


enum class EAttachmentNames : uint8;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UE_PUBG_API UC_PlayerCombatFieldManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UC_PlayerCombatFieldManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	void SetOwnerCombatFieldManager(class AC_CombatFieldManager* InCombatFieldManager) { OwnerCombatFieldManager = InCombatFieldManager; }
	void SetCombatFieldEnemy(class AC_Enemy* InEnemy) { CombatFieldEnemy = InEnemy; }
	void SetPlayerCombatFieldWidget(class UC_PlayerCombatFieldWidget* InPlayerCombatFieldWidget) { PlayerCombatFieldWidget = InPlayerCombatFieldWidget; }

private: // Combat Start 처리 관련 (OpeningGate Interaction)
	
	UFUNCTION()
	void OnGateOpeningBoxBeginOverlap
	(
		UPrimitiveComponent*	OverlappedComp,
		AActor*					OtherActor,
		UPrimitiveComponent*	OtherComp,
		int32					OtherBodyIndex,
		bool					bFromSweep,
		const FHitResult&		SweepResult
	);

	UFUNCTION()
	void OnGateOpeningBoxEndOverlap
	(
		UPrimitiveComponent* OverlappedComponent,
		AActor*				 OtherActor,
		UPrimitiveComponent* OtherComp,
		int32				 OtherBodyIndex
	);

	/// <summary>
	/// Start Gate OpeningBox Overlap되었을 때, F Key Start 처리
	/// </summary>
	/// <returns> : 제대로 Start 처리가 되었다면 return true </returns>
	bool OnStartGateFKeyInteraction();

private:
	
	/// <summary>
	/// Combat용 Item으로 Player inven 및 부착물, 무기 초기화
	/// </summary>
	void InitPlayerCombatItems();

private:

	AC_CombatFieldManager*		OwnerCombatFieldManager{};
	AC_Enemy*					CombatFieldEnemy{};
	
	UC_PlayerCombatFieldWidget* PlayerCombatFieldWidget{};

protected:

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly)
	class AC_TutorialGate* CombatFieldStartGate{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EAttachmentNames, TSubclassOf<class AC_AttachableItem>> GunAttachableClasses{};

};
