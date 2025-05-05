// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "C_GameInstance.generated.h"

/**
 * 게임 실행 -> 종료 까지 지속적으로 살아있는 Singleton 클래스 (GameManager)
 */
UCLASS()
class UE_PUBG_API UC_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	/// <summary>
	/// <para> Override the Init function from UGameInstance </para>
	/// <para> Called When the GameInstance is initialized </para>
	/// </summary>
	virtual void Init() override;

	/// <summary>
	/// <para> UFUNCTION() required for binding to the PreLoadMap delegate </para>
	/// <para> Called before a new map starts loading </para>
	/// </summary>
	/// <param name="InMapName"> : The name/path of the map that's about to be loaded </param>
	UFUNCTION()
	virtual void BeginLoadingScreen(const FString& InMapName);

	/// <summary>
	/// <para> UFUNCTION() required for binding to the PostLoadMapWithWorld delegate </para>
	/// <para> Called after a map has finished loading </para>
	/// </summary>
 	/// <param name="InLoadedWorld"> : Pointer to the newly loaded world/level </param>
	UFUNCTION()
	virtual void EndLoadingScreen(UWorld* InLoadedWorld);
	
};
