// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "I_WeaponButtonStrategy.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UI_WeaponButtonStrategy : public UInterface
{
	GENERATED_BODY()
};

/**
 * �� ��ư�� ���� Weapon ���� �˰��� interface
 */
class UE_PUBG_API II_WeaponButtonStrategy
{
	GENERATED_BODY()

public:

	/// <summary>
	/// B key mapped weapon strategy
	/// </summary>
	/// <param name="WeaponUser"> : Weapon Owner Character </param>
	/// <param name="Weapon">     : �ش� ������ ����� Weapon ��ü </param>
	/// <returns> �ش��ϴ� ���� ������ ���ų� ���� ���� ���� �� return false </returns>
	virtual bool UseBKeyStrategy(class AC_BasicCharacter* WeaponUser, class AC_Weapon* Weapon) = 0;

	/// <summary>
	/// R key mapped weapon strategy
	/// </summary>
	/// <param name="WeaponUser"> : Weapon Owner Character </param>
	/// <param name="Weapon">     : �ش� ������ ����� Weapon ��ü </param>
	/// <returns> �ش��ϴ� ���� ������ ���ų� ���� ���� ���� �� return false </returns>
	virtual bool UseRKeyStrategy(class AC_BasicCharacter* WeaponUser, class AC_Weapon* Weapon) = 0;

	/// <summary>
	/// MLB Click Started mapped weapon strategy
	/// </summary>
	/// <param name="WeaponUser"> : Weapon Owner Character </param>
	/// <param name="Weapon">     : �ش� ������ ����� Weapon ��ü </param>
	/// <returns> �ش��ϴ� ���� ������ ���ų� ���� ���� ���� �� return false </returns>
	virtual bool UseMlb_StartedStrategy(class AC_BasicCharacter* WeaponUser, class AC_Weapon* Weapon) = 0;

	/// <summary>
	/// MLB Click OnGoing mapped weapon strategy
	/// </summary>
	/// <param name="WeaponUser"> : Weapon Owner Character </param>
	/// <param name="Weapon">     : �ش� ������ ����� Weapon ��ü </param>
	/// <returns> �ش��ϴ� ���� ������ ���ų� ���� ���� ���� �� return false </returns>
	virtual bool UseMlb_OnGoingStrategy(class AC_BasicCharacter* WeaponUser, class AC_Weapon* Weapon) = 0;

	/// <summary>
	/// MLB Click Completed mapped weapon strategy
	/// </summary>
	/// <param name="WeaponUser"> : Weapon Owner Character </param>
	/// <param name="Weapon">     : �ش� ������ ����� Weapon ��ü </param>
	/// <returns> �ش��ϴ� ���� ������ ���ų� ���� ���� ���� �� return false </returns>
	virtual bool UseMlb_CompletedStrategy(class AC_BasicCharacter* WeaponUser, class AC_Weapon* Weapon) = 0;

	/// <summary>
	/// MRB Click Started mapped weapon strategy
	/// </summary>
	/// <param name="WeaponUser"> : Weapon Owner Character </param>
	/// <param name="Weapon">     : �ش� ������ ����� Weapon ��ü </param>
	/// <returns> �ش��ϴ� ���� ������ ���ų� ���� ���� ���� �� return false </returns>
	virtual bool UseMrb_StartedStrategy(class AC_BasicCharacter* WeaponUser, class AC_Weapon* Weapon) = 0;

	/// <summary>
	/// MRB Click OnGoing mapped weapon strategy
	/// </summary>
	/// <param name="WeaponUser"> : Weapon Owner Character </param>
	/// <param name="Weapon">     : �ش� ������ ����� Weapon ��ü </param>
	/// <returns> �ش��ϴ� ���� ������ ���ų� ���� ���� ���� �� return false </returns>
	virtual bool UseMrb_OnGoingStrategy(class AC_BasicCharacter* WeaponUser, class AC_Weapon* Weapon) = 0;

	/// <summary>
	/// MRB Click Completed mapped weapon strategy
	/// </summary>
	/// <param name="WeaponUser"> : Weapon Owner Character </param>
	/// <param name="Weapon">     : �ش� ������ ����� Weapon ��ü </param>
	/// <returns> �ش��ϴ� ���� ������ ���ų� ���� ���� ���� �� return false </returns>
	virtual bool UseMrb_CompletedStrategy(class AC_BasicCharacter* WeaponUser, class AC_Weapon* Weapon) = 0;

};
