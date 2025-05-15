// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "C_StaticMeshToHISMConverter.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_StaticMeshToHISMConverter : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void ConvertToHISM();

protected:
	TMap<UStaticMesh*, class UHierarchicalInstancedStaticMeshComponent*> MeshToHISMMap{};
};
