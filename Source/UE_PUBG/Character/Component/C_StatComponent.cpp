// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_StatComponent.h"

#include "C_InvenComponent.h"
#include "C_SwimmingComponent.h"
#include "Character/C_BasicCharacter.h"
#include "Character/C_Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/C_HUDWidget.h"
#include "HUD/C_OxygenWidget.h"
#include "InvenUserInterface/C_ItemBarWidget.h"
#include "Item/Equipment/C_EquipableItem.h"
#include "HUD/C_BloodScreenWidget.h"

#include "Utility/C_Util.h"	

const float UC_StatComponent::MAX_HP		= 100.f;
const float UC_StatComponent::MAX_BOOSTING	= 100.f;
const float UC_StatComponent::HEAL_UP_LIMIT = 75.f; // 구급상자, 붕대로 채울 수 있는 총 힐량 limit
const float UC_StatComponent::MAX_OXYGEN_HP = 100.f; // 숨 HP Max

const float UC_StatComponent::BOOST_ONE_BLOCK_EFFECT_TIME	= 8.f;
const float UC_StatComponent::BOOST_ONE_BLOCK_AMOUNT		= 2.631f; // 한 블록 당 줄어드는 Boost 량

// 20 40 30 10
const TArray<float> UC_StatComponent::EACH_BOOST_PHASE_BORDER = { 20.f, 60.f, 90.f, 100.f };

const TArray<FBoostingEffectFactor> UC_StatComponent::BOOSTING_EFFECT_FACTORS =
{
	{1.f, 1.f},		// 1페이즈 8초당 체력 회복량 & 이동 속도 증가(factor)
	{2.f, 1.01f},	// 2페이즈
	{3.f, 1.025f},	// 3페이즈
	{4.f, 1.0625f}	// 4페이즈
};

const float UC_StatComponent::OXYGEN_EXHAUSTED_DAMAGE_PER_SEC = 20.f;

const TMap<FName, EDamagingPartType> UC_StatComponent::DAMAGINGPARTS_MAP =
{
	{"Neck",		EDamagingPartType::HEAD},

	{"Hips",		EDamagingPartType::HIPS},

	{"LeftUpLeg",	EDamagingPartType::LEFT_LEG},
	{"LeftFoot",	EDamagingPartType::LEFT_FOOT},
	{"RightUpLeg",	EDamagingPartType::RIGHT_LEG},
	{"RightFoot",	EDamagingPartType::RIGHT_FOOT},

	{"Spine",		EDamagingPartType::LOWER_STOMACH},
	{"Spine1",		EDamagingPartType::UPPER_STOMACH},
	{"Spine2",		EDamagingPartType::SHOULDER},

	{"LeftArm",		EDamagingPartType::LEFT_ARM},
	{"LeftHand",	EDamagingPartType::LEFT_HAND},

	{"RightArm",	EDamagingPartType::RIGHT_ARM},
	{"RightHand",	EDamagingPartType::RIGHT_HAND}
};

UC_StatComponent::UC_StatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UC_StatComponent::BeginPlay()
{
	Super::BeginPlay();
	if (HittingBlood)
	{
		HittingBlood->AddToViewport();
		HittingBlood->SetVisibility(ESlateVisibility::Collapsed);
	}//TODO : 처음에 시작할 때 무슨 이유인지 알파값이 존재하는 상태로 시작하는 것 같음.
}


void UC_StatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateBoostEffect(DeltaTime);
	//UC_Util::Print("CurHP : " + FString::SanitizeFloat(CurHP));
	HandleOxygenExhausted(DeltaTime);

	HandleFallingDamage();
}

void UC_StatComponent::SetCurHP(const float& InCurHP)
{
	CurHP = InCurHP;

	if (OwnerHUDWidget) OwnerHUDWidget->OnUpdateHP(CurHP);
	if (AC_Enemy* Enemy = Cast<AC_Enemy>(OwnerCharacter)) Enemy->GetHPBar()->SetPercent(CurHP / MAX_HP);
}

void UC_StatComponent::SetCurBoosting(const float& InCurBoosting)
{
	CurBoosting = InCurBoosting;
	if (OwnerHUDWidget) OwnerHUDWidget->OnUpdateBoosting(CurBoosting);
	if (AC_Enemy* Enemy = Cast<AC_Enemy>(OwnerCharacter)) Enemy->GetBoostBar()->SetPercent(CurBoosting / MAX_BOOSTING);
}

bool UC_StatComponent::TakeDamage(const float& DamageAmount, const FKillFeedDescriptor& KillFeedDescriptor)
{
	if (CurHP <= 0.f) return false;
	if (DamageAmount < 0.f) return false;

	CurHP -= DamageAmount;


	if (CurHP < 0.f) CurHP = 0.f;

	if (OwnerHUDWidget) 
	{ 
		OwnerHUDWidget->OnUpdateHP(CurHP); 
		if (HittingBlood)
		{ 
			HittingBlood->ShowHitEffect(); 
			// UC_Util::Print("HittingBlood!");
		}
		else
		{
			// UC_Util::Print("HitingBlood is Nullptr!");
		}
	}

	
	if (AC_Enemy* OwnerEnemy = Cast<AC_Enemy>(OwnerCharacter))
	{
		OwnerEnemy->GetHPBar()->SetPercent(CurHP / MAX_HP);// TODO : 이 라인 지우기
		OwnerEnemy->OnTakeDamage(KillFeedDescriptor.DamageCauser);
	}

	// 사망
	if (CurHP <= 0.f)
	{
		if (Cast<AC_Player>(OwnerCharacter)) return true; // 잠깐 테스트 위해 Player만 Dead처리 꺼둠 ((현재 Enemy만 처리))
		
		// 사망 처리 
		OwnerCharacter->CharacterDead(KillFeedDescriptor);
	}
	
	return true;
}

float UC_StatComponent::TakeDamage(float DamageAmount, EDamagingPartType DamagingPartType, FKillFeedDescriptor& KillFeedDescriptor, const bool& bVestTakeDamage)
{
	//FString Str = "Character Damaged on certain damaging part! Damaged Amount : " + FString::SanitizeFloat(DamageAmount);
	//UC_Util::Print(Str, FColor::Cyan, 3.f);

	// Armor 확인해서 Armor 부분이라면 Damage 감소 적용

	// FString str = "Damage caused by" + DamageCauser->GetName();
	// UC_Util::Print(str, FColor::MakeRandomColor(), 10.f);

	switch (DamagingPartType)
	{
	case EDamagingPartType::HEAD:
		KillFeedDescriptor.bDamagedByHeadShot = true;
		if (AC_EquipableItem* Helmet = OwnerCharacter->GetInvenComponent()->GetEquipmentItems()[EEquipSlot::HELMET])
		{
			DamageAmount *= Helmet->GetDamageReduceFactor();
			Helmet->TakeDamage(DamageAmount);
		}
		break;
	case EDamagingPartType::SHOULDER:		case EDamagingPartType::UPPER_STOMACH:
	case EDamagingPartType::LOWER_STOMACH:	case EDamagingPartType::HIPS:
	
		if (!bVestTakeDamage) break;
	
		if (AC_EquipableItem* Vest = OwnerCharacter->GetInvenComponent()->GetEquipmentItems()[EEquipSlot::VEST])
		{
			DamageAmount *= Vest->GetDamageReduceFactor();
			Vest->TakeDamage(DamageAmount);
		}
		break;
	}

	TakeDamage(DamageAmount, KillFeedDescriptor);
	return DamageAmount;
}

float UC_StatComponent::TakeDamage(float DamageAmount, FName DamagingPhysicsAssetBoneName, FKillFeedDescriptor& KillFeedDescriptor, const bool& bVestTakeDamage)
{
	if (!DAMAGINGPARTS_MAP.Contains(DamagingPhysicsAssetBoneName))
	{
		// UC_Util::Print("From UC_StatComponent::TakeDamage : No Such PhysicsAsset Bone Name exists!");
		return 0.f;
	}

	return TakeDamage(DamageAmount, DAMAGINGPARTS_MAP[DamagingPhysicsAssetBoneName], KillFeedDescriptor, bVestTakeDamage);
}

bool UC_StatComponent::ApplyHeal(const float& HealAmount)
{
	if (CurHP >= MAX_HP)  return false; // 이미 체력이 모두 찼을 때
	if (HealAmount < 0.f) return false;

	CurHP = FMath::Min(CurHP + HealAmount, MAX_HP);
	
	if (OwnerHUDWidget) OwnerHUDWidget->OnUpdateHP(CurHP);
	if (AC_Enemy* Enemy = Cast<AC_Enemy>(OwnerCharacter)) Enemy->GetHPBar()->SetPercent(CurHP / MAX_HP);
	return true;
}

bool UC_StatComponent::AddBoost(const float& BoostAmount)
{
	if (CurBoosting >= MAX_BOOSTING)	return false;
	if (BoostAmount < 0.f)				return false;

	CurBoosting = FMath::Min(CurBoosting + BoostAmount, MAX_BOOSTING);

	if (OwnerHUDWidget) OwnerHUDWidget->OnUpdateBoosting(CurBoosting);
	if (AC_Enemy* Enemy = Cast<AC_Enemy>(OwnerCharacter)) Enemy->GetBoostBar()->SetPercent(CurBoosting / MAX_BOOSTING);
	return true;
}

void UC_StatComponent::AddOxygen(const float& OxygenAmount)
{
	CurOxygen += OxygenAmount;
	CurOxygen  = FMath::Clamp(CurOxygen, 0.f, MAX_OXYGEN_HP);

	if (OwnerHUDWidget) OwnerHUDWidget->GetOxygenWidget()->SetOxygenPercent(CurOxygen * 0.01f);
}

void UC_StatComponent::UpdateBoostEffect(const float& DeltaTime)
{
	// Boost 게이지가 없거나 이미 사망 처리되었을 때
	if (CurBoosting <= 0.f || CurHP <= 0.f)
	{
		BoostTimer = 0.f;
		return;
	}

	BoostTimer += DeltaTime;

	if (BoostTimer < BOOST_ONE_BLOCK_EFFECT_TIME) return;

	BoostTimer -= BOOST_ONE_BLOCK_EFFECT_TIME;
	
	FBoostingEffectFactor BoostingEffectFactor = GetBoostingEffectFactorByCurBoostingAmount();

	OwnerCharacter->SetBoostingSpeedFactor(BoostingEffectFactor.MoveSpeedIncreaseFactor);
	ApplyHeal(BoostingEffectFactor.OneBlockHPGainedAmount);
	
	CurBoosting -= BOOST_ONE_BLOCK_AMOUNT;
	if (CurBoosting <= 0.5f) CurBoosting = 0.f;
	
	if (OwnerHUDWidget) OwnerHUDWidget->OnUpdateBoosting(CurBoosting);
	if (AC_Enemy* Enemy = Cast<AC_Enemy>(OwnerCharacter)) Enemy->GetBoostBar()->SetPercent(CurBoosting / MAX_BOOSTING);	
}

void UC_StatComponent::HandleOxygenExhausted(const float& DeltaTime)
{
	if (CurOxygen > 0.f)
	{
		OxygenExhaustedTimer = 0.f;
		return;
	}
	if (OxygenExhaustedTimer >= 1.f)
	{
		OxygenExhaustedTimer -= 1.f;

		FKillFeedDescriptor DrownKillFeedDescriptor =
		{
			EDamageType::Drown,
			nullptr,
			OwnerCharacter,
			nullptr,
			false,
			0
		};
		
		TakeDamage(OXYGEN_EXHAUSTED_DAMAGE_PER_SEC, DrownKillFeedDescriptor);
		return;
	}
	OxygenExhaustedTimer += DeltaTime;
}

FBoostingEffectFactor UC_StatComponent::GetBoostingEffectFactorByCurBoostingAmount() const
{
	for (int i = 0; i < EACH_BOOST_PHASE_BORDER.Num(); i++)
		if (CurBoosting <= EACH_BOOST_PHASE_BORDER[i]) return BOOSTING_EFFECT_FACTORS[i];

	return FBoostingEffectFactor();
}

void UC_StatComponent::HandleFallingDamage()
{
	// TODO : 차에서 떨어졌을 때에는 아마 차에서 내리는 함수에서 따로 처리를 해줘야 함
	
	// SkyDiving 중이라면 Damage 처리 x
	if (OwnerCharacter->GetMainState() == EMainState::SKYDIVING) return;
	
	if (OwnerCharacter->GetCharacterMovement()->IsFalling()) // 떨어지고 있는 중
	{
		if (!bFallingFlag) // 떨어지기 시작한 시점
		{
			bFallingFlag = true;
			FallingStartedHeight = OwnerCharacter->GetActorLocation().Z;
		}
		return;
	}

	// 떨어지지 않고 있는 중

	if (FallDamageAmount > 0.f) // 이전 Tick에서 FallDamageAmount가 setting 되었을 때(Deferred Damage taking 처리)
	{
		if (OwnerCharacter->GetSwimmingComponent()->IsSwimming())
		{
			// 물 속으로 들어간 처리라면 Damage 처리하지 않기
			FallDamageAmount = 0.f;
			return;
		}

		FKillFeedDescriptor FallKillFeedDescriptor =
		{
			EDamageType::Fall,
			nullptr,
			OwnerCharacter,
			nullptr,
			false,
			0
		};
		
		TakeDamage(FallDamageAmount, FallKillFeedDescriptor); // 낙하 데미지 적용
		FallDamageAmount = 0.f;
	}
	

	if (bFallingFlag) // 착지한 시점, damage 계산하기
	{
		FColor Color = FColor::MakeRandomColor();
		bFallingFlag = false;
		
		float FallenHeight = FallingStartedHeight - OwnerCharacter->GetActorLocation().Z;

		// UC_Util::Print("Fallen Height : " + FString::SanitizeFloat(FallenHeight * 0.01f) + "M", Color, 10.f);
		
		// 15m 이상
		if (FallenHeight >= 1500.f)
		{
			FallDamageAmount = 100.f;
			// UC_Util::Print("Damage Amount : " + FString::SanitizeFloat(FallDamageAmount), Color, 10.f);
			return;
		}

		// 5m ~ 15m 사이 -> 비율에 따른 FallDamageAmount 설정
		FallDamageAmount = FMath::GetMappedRangeValueClamped(FVector2D(500.f, 1500.f), FVector2D(0.f, 100.f), FallenHeight);
		// UC_Util::Print("Damage Amount : " + FString::SanitizeFloat(FallDamageAmount), Color, 10.f);

		// Deferred 방식으로 Damage를 Apply할지 결정 -> 수영 상태를 확인해야해서
	}
}
