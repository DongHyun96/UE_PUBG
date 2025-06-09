// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "C_ANRifleDrawSheathSound.generated.h"

/**
 * 주의사항 : EquippedComponent의 CurWeapon이 바뀌는 AnimNotify 시점을 잘 고려하여 넣어주어야 함
 */
UCLASS()
class UE_PUBG_API UC_ANRifleDrawSheathSound : public UAnimNotify
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	
	/// <summary>
	/// Notify 내의 if문 중첩 정리 때문에 만든 함수 
	/// </summary>
	/// <param name="TargetSound"> : 재생하려는 SoundBase </param>
	/// <param name="TargetCharacter"> : Sound를 재생시키려는 Character 객체 </param>
	void PlaySound(USoundBase* TargetSound, class AC_BasicCharacter* TargetCharacter);

protected:

	// Draw sound notify인지 Sheath sound notify인지 구분 (false일 경우 Sheath Sound Notify)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="AnimNotify")
	bool bIsDrawSoundNotify{};

	// 2개로 초기화 (AR, SR Draw순으로 sound 초기화)
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Sound")
	TArray<USoundBase*> DrawSounds{};

	// 2개로 초기화 (AR, SR Sheath순으로 sound 초기화)
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Sound")
	TArray<USoundBase*> SheathSounds{};

	static const int ARIndex;
	static const int SRIndex;
	
};
