// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Component/C_EquippedComponent.h"
#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"

#include "Item/Weapon/C_Weapon.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Item/Weapon/Gun/C_AR.h"
#include "Item/Weapon/MeleeWeapon/C_MeleeWeapon.h"
#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"

#include "Character/Component/C_SwimmingComponent.h"
#include "Character/Component/C_InvenSystem.h"

#include "HUD/C_HUDWidget.h"
#include "HUD/C_AmmoWidget.h"
#include "InvenUI/BasicItemSlot/WeaponSlot/C_ThrowableWeaponSlotWidget.h"

#include "Utility/C_Util.h"

// Sets default values for this component's properties
UC_EquippedComponent::UC_EquippedComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;

    // ...
}


// Called when the game starts
void UC_EquippedComponent::BeginPlay()
{
    Super::BeginPlay();
}


// Called every frame
void UC_EquippedComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // ...
}

AC_Weapon* UC_EquippedComponent::SetSlotWeapon(EWeaponSlot InSlot, AC_Weapon* Weapon)
{
    AC_Weapon* PrevSlotWeapon = Weapons[InSlot];

    // 들어온 슬롯의 이전 무기가 존재할 때 이전 무기 해제
    if (PrevSlotWeapon)
    {
        Weapons[InSlot]->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);

        // 이전 무기 해제에 대한 PoseTransitionEnd 델리게이트 해제
        OwnerCharacter->Delegate_OnPoseTransitionFin.RemoveAll(PrevSlotWeapon);
    }

    Weapons[InSlot] = Weapon; // 새로 들어온 무기로 교체

    AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter);
    
    if (OwnerPlayer)
    {
        OwnerPlayer->GetInvenSystem()->InitializeList(); 
    }

    if (!Weapons[InSlot]) // Slot에 새로 지정한 무기가 nullptr -> early return
    {
        if (CurWeaponType == InSlot) // 현재 손에 들고 있는 무기를 Slot에서 강제로 뺀 상황
        {
            NextWeaponType  = EWeaponSlot::NONE;
            CurWeaponType   = EWeaponSlot::NONE;
            OwnerCharacter->SetHandState(EHandState::UNARMED);

            if (OwnerPlayer) OwnerPlayer->GetHUDWidget()->GetAmmoWidget()->SetVisibility(ESlateVisibility::Hidden);
        }

        return PrevSlotWeapon;
    }
    
    //아이템의 위치 변경,1108 상연
    Weapons[InSlot]->SetItemPlace(EItemPlace::SLOT);

    SetMainGunOrSubGun(InSlot);


    Weapons[InSlot]->SetOwnerCharacter(OwnerCharacter); // 새로운 OwnerCharacter 지정
    Weapons[InSlot]->SetActorHiddenInGame(true);
    //충돌체 켜주기, 1117 상연, 근접무기는 장착만 해도 보이는 상태.
    if (InSlot == EWeaponSlot::MELEE_WEAPON)
        Weapons[InSlot]->SetActorHiddenInGame(false);

    //꺼주면 제자리 파쿠르 방지.
    // Weapons[InSlot]->SetActorEnableCollision(false); 
 
    Weapons[InSlot]->SetRelativeTranformToInitial();
    Weapons[InSlot]->AttachToHolster(OwnerCharacter->GetMesh());

    // 새로 장착된 무기에 대한 PoseTransitionEnd 델리게이트 callback 걸기
    OwnerCharacter->Delegate_OnPoseTransitionFin.AddUObject(Weapons[InSlot], &AC_Weapon::OnOwnerCharacterPoseTransitionFin);
    
    return PrevSlotWeapon;
}

bool UC_EquippedComponent::SwapSlotsWhileGunHandState()
{
    AC_Gun* CurMainGun  = Cast<AC_Gun>(Weapons[EWeaponSlot::MAIN_GUN]);
    AC_Gun* CurSubGun   = Cast<AC_Gun>(Weapons[EWeaponSlot::SUB_GUN]);


    // 예외 상황이 아닌 상황
    if (!IsValid(CurMainGun) && !IsValid(CurSubGun))                                        return false;
    if (CurWeaponType != EWeaponSlot::MAIN_GUN && CurWeaponType != EWeaponSlot::SUB_GUN)    return false;
    
    Weapons[EWeaponSlot::SUB_GUN]   = CurMainGun;
    Weapons[EWeaponSlot::MAIN_GUN]  = CurSubGun;

    if (CurMainGun) CurMainGun->ChangeGunState(EGunState::SUB_GUN);
    if (CurSubGun)  CurSubGun->ChangeGunState(EGunState::MAIN_GUN);

    // CurMainGun과 CurSubGun 서로 Slot Swap
    // 현재 CurWeaponType -> 다른 종류의 GunType으로 Swap시키기
    // Gun 무기 Type 바꾸기
    if (CurWeaponType == EWeaponSlot::MAIN_GUN)
    {
        CurWeaponType = EWeaponSlot::SUB_GUN;

        // MainGun인 총기를 Holster에 집어넣기
        if (Weapons[EWeaponSlot::MAIN_GUN])
            Weapons[EWeaponSlot::MAIN_GUN]->AttachToHolster(OwnerCharacter->GetMesh());
    }
    else if (CurWeaponType == EWeaponSlot::SUB_GUN)
    {
        CurWeaponType = EWeaponSlot::MAIN_GUN;

        // SubGun인 총기를 Holster에 집어넣기
        if (Weapons[EWeaponSlot::SUB_GUN])
            Weapons[EWeaponSlot::SUB_GUN]->AttachToHolster(OwnerCharacter->GetMesh());
    }

    return true;
}

bool UC_EquippedComponent::ChangeCurWeapon(EWeaponSlot InChangeTo)
{
    if (OwnerCharacter->GetSwimmingComponent()->IsSwimming()) return false;

    if (IsValid(Weapons[CurWeaponType]))
    {
        // 현재 무기의 Sheath나 Draw animation montage가 이미 재생 중이라면 return
        if (OwnerCharacter->GetMesh()->GetAnimInstance()
            ->Montage_IsPlaying(Weapons[CurWeaponType]->GetCurDrawMontage().AnimMontage))
            return false;

        if (OwnerCharacter->GetMesh()->GetAnimInstance()
            ->Montage_IsPlaying(Weapons[CurWeaponType]->GetCurSheathMontage().AnimMontage))
            return false;
    }

    NextWeaponType = InChangeTo;

    if (CurWeaponType == NextWeaponType)
    {
        // 현재 투척류를 들고 있을 경우, 다른 종류의 투척류로 전환 시도
        if (AC_ThrowingWeapon* ThrowingWeapon = Cast<AC_ThrowingWeapon>(GetCurWeapon()))
        {
            // 전환 시도하려는 기존 투척류의 쿠킹이 이미 시작되었고, 아직 손에서 떠나지 않은 투척류라면 전환 시도 x
            if (ThrowingWeapon->GetIsCooked() && ThrowingWeapon->GetAttachParentActor()) return false;

            // 다음 종류의 투척류로 전환 시도
            EThrowableType ThrowableTypeTemp = ThrowingWeapon->GetThrowableType();
            while (++ThrowableTypeTemp != ThrowingWeapon->GetThrowableType())
            {
                FName               NextWeaponName      = AC_ThrowingWeapon::GetThrowableItemNameMap()[ThrowableTypeTemp];
                AC_Item*            NextThrowableItem   = OwnerCharacter->GetInvenComponent()->FindMyItemByName(NextWeaponName);
                AC_ThrowingWeapon*  NextThrowableWeapon = Cast<AC_ThrowingWeapon>(NextThrowableItem);

                if (!NextThrowableWeapon) continue; // NextThrowable이 Inven에 없는 경우

                NextThrowableWeapon->MoveToSlot(OwnerCharacter, NextThrowableWeapon->GetItemCurStack());
                if (AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter)) OwnerPlayer->GetInvenSystem()->GetInvenUI()->UpdateWidget();

                // 바로 다음 투척류 꺼내기
                OwnerCharacter->PlayAnimMontage(NextThrowableWeapon->GetCurDrawMontage());
                return true;
            }
        }
        return false; // 현재 무기와 다음 무기가 같을 때 무기를 굳이 다시 꺼내지 않음
    }

    // NextWeaponType이 None이 아니고, 바꾸려는 무기 슬롯에 무기가 없을 때
    if (NextWeaponType != EWeaponSlot::NONE && !IsValid(Weapons[NextWeaponType]))
    {
        if (NextWeaponType != EWeaponSlot::THROWABLE_WEAPON) return false;
        
        // Throwable의 경우, 가방 확인해서 있으면 장착해서 Throwable로 바꾸기

        // 새로운 투척류(가방에 있던)가 해당 슬롯에 장착되었는지 체크
        bool bThrowableSetToTargetSlot{};  
        
        for (int i = 0; i < static_cast<int>(EThrowableType::MAX); ++i)
        {
            EThrowableType      ThrowableType  = static_cast<EThrowableType>(i);
            FName               WeaponName     = AC_ThrowingWeapon::GetThrowableItemNameMap()[ThrowableType];
            AC_Item*            ThrowableItem  = OwnerCharacter->GetInvenComponent()->FindMyItemByName(WeaponName);
            AC_ThrowingWeapon*  ThrowingWeapon = Cast<AC_ThrowingWeapon>(ThrowableItem);

            if (!ThrowingWeapon) continue;

            ThrowingWeapon->MoveToSlot(OwnerCharacter, ThrowingWeapon->GetItemCurStack());
            if (AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter)) OwnerPlayer->GetInvenSystem()->GetInvenUI()->UpdateWidget();

            bThrowableSetToTargetSlot = true;
            break;
        }
        if (!bThrowableSetToTargetSlot) return false;
    }

    // 현재 무기를 착용중이지 않을 때 (UnArmed 상태), 또는 현재 슬롯에 장착된 무기가 없을 때 다음 무기 Draw만 재생
    if (CurWeaponType == EWeaponSlot::NONE || !IsValid(GetCurWeapon()))
    {
        // 만약 다음에 바꿀 무기가 None이거나 다음에 바꿀 무기 슬롯에 무기가 없을 때 
        if (NextWeaponType == EWeaponSlot::NONE || !IsValid(Weapons[NextWeaponType]))
        {
            CurWeaponType = EWeaponSlot::NONE;
            NextWeaponType = EWeaponSlot::NONE;
            OwnerCharacter->SetHandState(EHandState::UNARMED);
            return false;
        }

        // 다음 무기가 있을 때
        OwnerCharacter->PlayAnimMontage(Weapons[NextWeaponType]->GetCurDrawMontage());
        
        return true;
    }

    // 현재 무기를 착용중인 상황

    // 투척류 예외처리
    if (CurWeaponType == EWeaponSlot::THROWABLE_WEAPON)
    {
        AC_ThrowingWeapon* ThrowingWeapon = Cast<AC_ThrowingWeapon>(GetCurWeapon());
        if (IsValid(ThrowingWeapon))
        {
            // 이미 쿠킹이 시작되었고, 아직 손에서 떠나지 않은 투척류라면 땅에 떨굼
            if (ThrowingWeapon->GetIsCooked() && ThrowingWeapon->GetAttachParentActor())
                return ThrowingWeapon->ReleaseOnGround();
        }
    }
    //총을 들고 Aiming 중일 때 카메라 다시 원래대로 전환
    if (AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter))
    {
        if (CurWeaponType == EWeaponSlot::MAIN_GUN || CurWeaponType == EWeaponSlot::SUB_GUN)
        {
            AC_Gun* TempWeapon = Cast<AC_Gun>(Weapons[CurWeaponType]);
            if (IsValid(TempWeapon))
                TempWeapon->BackToMainCamera();
        }
    }

    OwnerCharacter->PlayAnimMontage(Weapons[CurWeaponType]->GetCurSheathMontage()); // 현 무기 집어넣는 동작에 Notify함수 걸어서 다음 무기로 전환
    return true;
}

bool UC_EquippedComponent::ToggleArmed()
{
    // Swimming 중이라면 return false
    if (OwnerCharacter->GetSwimmingComponent()->IsSwimming()) return false;

    // 현재 무기도 장착하지 않았고 이전에 들고 있었던 무기도 없을 때 (초기 상태)
    if (CurWeaponType == EWeaponSlot::NONE && PrevWeaponType == EWeaponSlot::NONE) return false;

    // 현재 들고 있는 무기가 있을 때
    if (CurWeaponType != EWeaponSlot::NONE && IsValid(GetCurWeapon()))
    {
        PrevWeaponType = CurWeaponType;


        return ChangeCurWeapon(EWeaponSlot::NONE);
    }

    // 현재 들고 있는 무기가 없을 때
    return ChangeCurWeapon(PrevWeaponType);
}

bool UC_EquippedComponent::TryAttachCurWeaponToHolsterWithoutSheathMotion()
{
    if (!GetCurWeapon()) return false;

    // 투척류 예외처리
    if (CurWeaponType == EWeaponSlot::THROWABLE_WEAPON)
    {
        AC_ThrowingWeapon* ThrowingWeapon = Cast<AC_ThrowingWeapon>(GetCurWeapon());
        if (IsValid(ThrowingWeapon))
        {
            // 이미 쿠킹이 시작되었고, 아직 손에서 떠나지 않은 투척류라면 땅에 떨굼
            if (ThrowingWeapon->GetIsCooked() && ThrowingWeapon->GetAttachParentActor())
                return ThrowingWeapon->ReleaseOnGround();
        }
    }

    // 현재 들고 있는 무기가 존재한다면 무기 잠깐 몸 쪽에 붙이기
    GetCurWeapon()->AttachToHolster(OwnerCharacter->GetMesh());
    OwnerCharacter->SetHandState(EHandState::UNARMED);

    // 총기류 예외처리
    if (AC_Gun* Gun = Cast<AC_Gun>(GetCurWeapon())) Gun->BackToMainCamera();

    return true;
}

bool UC_EquippedComponent::TryReAttachCurWeaponToHand()
{
    if (!GetCurWeapon()) return false;

    SetNextWeaponType(CurWeaponType);
    FPriorityAnimMontage DrawMontage = GetCurWeapon()->GetCurDrawMontage();
    OwnerCharacter->PlayAnimMontage(DrawMontage);

    return true;
}

void UC_EquippedComponent::OnSheathEnd()
{
    // Player HUD 업데이트
    if (AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter))
    {
        UC_AmmoWidget* AmmoWidget = OwnerPlayer->GetHUDWidget()->GetAmmoWidget();
        AmmoWidget->SetVisibility(ESlateVisibility::Hidden);
    }

    // 무기를 바꾸는 도중에 SlotWeapon 장착 해제 예외 처리
    if (!GetCurWeapon())
    {
        CurWeaponType = NextWeaponType;

        if (!IsValid(GetCurWeapon())) // 다음으로 바꿀 무기가 NONE일 때(UNARMED일 때)
        {
            OwnerCharacter->SetHandState(EHandState::UNARMED);
            CurWeaponType   = EWeaponSlot::NONE;
            NextWeaponType  = EWeaponSlot::NONE;
            return;
        }

        OwnerCharacter->PlayAnimMontage(GetCurWeapon()->GetCurDrawMontage());
        return;
    }

    // 현재 무기 무기집에 붙이기
    GetCurWeapon()->AttachToHolster(OwnerCharacter->GetMesh());

    // 총기류 예외처리
    if (CurWeaponType == EWeaponSlot::MAIN_GUN || CurWeaponType == EWeaponSlot::SUB_GUN)
    {
        AC_Gun* TempWeapon = Cast<AC_Gun>(GetCurWeapon());
        if (IsValid(TempWeapon))
            TempWeapon->SetIsAimPress(false);
    }

    CurWeaponType = NextWeaponType;

    if (!IsValid(GetCurWeapon()))
    {
        OwnerCharacter->SetHandState(EHandState::UNARMED);
        return;
    }

    OwnerCharacter->PlayAnimMontage(GetCurWeapon()->GetCurDrawMontage());
}

void UC_EquippedComponent::OnDrawStart()
{
    // 무기를 바꾸는 도중에 SlotWeapon 장착 해제 예외 처리 -> 바꿔들 무기가 사라졌을 때
    if (!Weapons[NextWeaponType])
    {
        //OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_Stop(0.2f, GetCurWeapon()->GetCurDrawMontage().AnimMontage);
        NextWeaponType  = EWeaponSlot::NONE;
        CurWeaponType   = EWeaponSlot::NONE;
        OwnerCharacter->SetHandState(EHandState::UNARMED);
        return;
    }

    Weapons[NextWeaponType]->AttachToHand(OwnerCharacter->GetMesh());
    //GetCurWeapon()->AttachToHand(OwnerCharacter->GetMesh());
}   

void UC_EquippedComponent::OnDrawEnd()
{
    if (NextWeaponType == EWeaponSlot::NONE) return;

    // 무기를 바꾸는 도중에 SlotWeapon 장착 해제 예외 처리 -> 바꿔들 무기가 사라졌을 때
    if (!Weapons[NextWeaponType])
    {
        //OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_Stop(0.2f, GetCurWeapon()->GetCurDrawMontage().AnimMontage);
        NextWeaponType  = EWeaponSlot::NONE;
        CurWeaponType   = EWeaponSlot::NONE;
        OwnerCharacter->SetHandState(EHandState::UNARMED);
        return;
    }

    UC_Util::Print("OnDrawEnd", FColor::Cyan, 5.f);

    Weapons[NextWeaponType]->AttachToHand(OwnerCharacter->GetMesh());
    CurWeaponType = NextWeaponType;
    NextWeaponType = EWeaponSlot::NONE;
}

void UC_EquippedComponent::OnReloadEnd()
{
    AC_Gun* CurGun = Cast<AC_Gun>(Weapons[CurWeaponType]);
    if (IsValid(CurGun))
    {
        CurGun->ReloadBullet();
        CurGun->SetMagazineVisibility(true);
    }
}

void UC_EquippedComponent::OnSniperReloadEnd()
{
    AC_Gun* CurGun = Cast<AC_Gun>(Weapons[CurWeaponType]);
    if (IsValid(CurGun))
    {
        CurGun->AttachToHand(OwnerCharacter->GetMesh());
        OwnerCharacter->SetIsReloadingBullet(false);

        UC_Util::Print("Attached To Hand???????????????????????????????????????????????????????");
    }
}

void UC_EquippedComponent::AddAttachedPartsActorsToIgnoreActors(FCollisionQueryParams& CollisionParams)
{
    for (auto& WeaponPair : Weapons)
    {
        AC_Weapon* Weapon = WeaponPair.Value;
        if (!Weapon) continue;

        TArray<AActor*> AttachedActors{};

        Weapon->GetAttachedActors(AttachedActors);
        CollisionParams.AddIgnoredActors(AttachedActors);
    }
}

void UC_EquippedComponent::SetMainGunOrSubGun(EWeaponSlot InSlot)
{
    if (InSlot == EWeaponSlot::MAIN_GUN || InSlot == EWeaponSlot::SUB_GUN)
    {
        AC_Gun* CurrentGun = Cast<AC_Gun>(Weapons[InSlot]);
        if (IsValid(CurrentGun))
        {
            if (InSlot == EWeaponSlot::MAIN_GUN)
                CurrentGun->SetMainOrSubSlot(EGunState::MAIN_GUN);
            else
                CurrentGun->SetMainOrSubSlot(EGunState::SUB_GUN);

        }
    }
}




