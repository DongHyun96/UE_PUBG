// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemBullet/C_Item_Bullet.h"

AC_Item_Bullet::AC_Item_Bullet()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AC_Item_Bullet::BeginPlay()
{
	Super::BeginPlay();
}

void AC_Item_Bullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
