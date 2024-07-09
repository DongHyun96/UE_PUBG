// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_Item.generated.h"
/// <summary>
/// �Ҹ�����ۺз��� ���� ���� Ŭ����.
/// ���濡 ���� �������� ���� Ŭ����.
/// ��)ź, ������, ����, ����ź��.
/// ���濡 ���� ����ǰ ��� �ۼ��ؾ���.
/// </summary>
UCLASS(abstract)
class UE_PUBG_API AC_Item : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AC_Item();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	uint8 GetVolume() { return Volume; }

	virtual void Interaction(class AC_BasicCharacter* character) PURE_VIRTUAL(AC_Item::Interaction;);

private:
	uint8 Volume;
};
