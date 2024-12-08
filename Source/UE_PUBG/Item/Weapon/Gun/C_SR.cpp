// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/Gun/C_SR.h"

AC_SR::AC_SR()
{
	PrimaryActorTick.bCanEverTick = true;
	//ItemType ����.
	ItemDatas.ItemType = EItemTypes::MAINGUN;
	CurGunType = EGunType::SR;
	EQUIPPED_SOCKET_NAME = FName("Sniper_Equip"); // ���Ⱑ �տ� ������ socket �̸�
	IronSightWindowLocation = FVector2D(0.5f, 0.5f);
}

void AC_SR::BeginPlay()
{
	Super::BeginPlay();
}

void AC_SR::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AC_SR::ExecuteReloadMontage()
{
}
