// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Equipment/C_Helmet.h"


bool AC_Helmet::TakeDamage(float DamageAmount)
{
	// Level에 따른 Helmet에 적용되는 Damage량 조절
	float DamageReduceRate =	(ItemLevel == EEquipableItemLevel::LV1) ? 1.25f  :
								(ItemLevel == EEquipableItemLevel::LV2) ? 0.666f : 0.434f;
	DamageAmount *= DamageReduceRate;
	
	CurDurability -= DamageAmount;
	if (CurDurability < 0.f)
	{
		CurDurability = 0.f;
		// TODO : 뚝배기의 경우 내구도 0이 되었을 때 없애야 함
	}

	return true;
}

float AC_Helmet::GetDamageReduceFactor() const
{
	return	(ItemLevel == EEquipableItemLevel::LV1) ? 0.7f :
			(ItemLevel == EEquipableItemLevel::LV2) ? 0.6f : 0.45f;
}
