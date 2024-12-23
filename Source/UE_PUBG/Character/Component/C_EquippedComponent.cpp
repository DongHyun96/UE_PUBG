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

    //SpawnWeaponsForTesting();
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
        // 현재 들고 있는 무기의 slot에 새로운 무기로 바꿔버리려 할 때
        if (GetCurWeapon() == PrevSlotWeapon)
        {
            OwnerCharacter->SetHandState(EHandState::UNARMED);
            if (!Weapon) CurWeaponType = EWeaponSlot::NONE;
        }

        // 이전 무기 해제에 대한 PoseTransitionEnd 델리게이트 해제
        OwnerCharacter->Delegate_OnPoseTransitionFin.RemoveAll(PrevSlotWeapon);

        //C_Item의 detachment에서 처리중, 혹시몰라 남겨둠.
        //PrevSlotWeapon->SetOwnerCharacter(nullptr);
    }
    
    Weapons[InSlot] = Weapon; // 새로 들어온 무기로 교체
    

    if (AC_Player* Player = Cast<AC_Player>(OwnerCharacter))
    {
        Player->GetInvenSystem()->InitializeList();
    }

    if (!Weapons[InSlot]) return PrevSlotWeapon; // Slot에 새로 지정한 무기가 nullptr -> early return
    
    //아이템의 위치 변경,1108 상연
    Weapons[InSlot]->SetItemPlace(EItemPlace::SLOT);

    SetMainGunOrSubGun(InSlot);


    Weapons[InSlot]->SetOwnerCharacter(OwnerCharacter); // 새로운 OwnerCharacter 지정

    //충돌체 켜주기, 1117 상연, 근접무기는 장착만 해도 보이는 상태.
    if (InSlot == EWeaponSlot::MELEE_WEAPON)
        Weapons[InSlot]->SetActorHiddenInGame(false);

    //꺼주면 제자리 파쿠르 방지.
    Weapons[InSlot]->SetActorEnableCollision(false); 
    // Attach to Holster 하기 전에 Local transform 초기화
    //Weapons[InSlot]->SetActorRelativeTransform(FTransform::Identity);
 
    Weapons[InSlot]->SetRelativeTranformToInitial();
    Weapons[InSlot]->AttachToHolster(OwnerCharacter->GetMesh());

    // 새로 장착된 무기에 대한 PoseTransitionEnd 델리게이트 callback 걸기
    OwnerCharacter->Delegate_OnPoseTransitionFin.AddUObject(Weapons[InSlot], &AC_Weapon::OnOwnerCharacterPoseTransitionFin);

    return PrevSlotWeapon;
}


void UC_EquippedComponent::DetachmentWeapon(EWeaponSlot InSlot)
{
    AC_Weapon* curWeapon = Weapons[InSlot];
    
    //아이템이 없다면 종료
    if (!curWeapon) return;
    
    curWeapon->SetOwnerCharacter(nullptr);

    curWeapon->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);

    curWeapon->SetActorLocation(OwnerCharacter->GetActorLocation() + FVector(0.f, 0.f, -75.f));
    curWeapon->SetActorRotation(FRotator(0.f, 0.f, -90.f));
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

    if (CurWeaponType == NextWeaponType) return false; // 현재 무기와 다음 무기가 같을 때 무기를 굳이 다시 꺼내지 않음

    // NextWeaponType이 None이 아니고, 바꾸려는 무기 슬롯에 무기가 없을 때
    if (NextWeaponType != EWeaponSlot::NONE && !IsValid(Weapons[NextWeaponType])) return false;

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
    if (CurWeaponType == EWeaponSlot::MAIN_GUN || CurWeaponType == EWeaponSlot::SUB_GUN)
    {
        AC_Gun* TempWeapon = Cast<AC_Gun>(Weapons[CurWeaponType]);
        if (IsValid(TempWeapon))
            TempWeapon->BackToMainCamera();
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

void UC_EquippedComponent::SpawnWeaponsForTesting()
{
    // Test용 weapon spawn들
    /*FActorSpawnParameters Param{};
    Param.Owner = OwnerCharacter;
    AC_MeleeWeapon* MeleeTemp = GetWorld()->SpawnActor<AC_MeleeWeapon>(WeaponClasses[EWeaponSlot::MELEE_WEAPON], Param);
    MeleeTemp->SetOwnerCharacter(OwnerCharacter);
    MeleeTemp->AttachToHolster(OwnerCharacter->GetMesh());

    SetSlotWeapon(EWeaponSlot::MELEE_WEAPON, MeleeTemp);

    FActorSpawnParameters Param2{};
    Param2.Owner = OwnerCharacter;
    AC_Gun* ARTemp = GetWorld()->SpawnActor<AC_Gun>(WeaponClasses[EWeaponSlot::MAIN_GUN], Param2);
    ARTemp->SetOwnerCharacter(OwnerCharacter);
    ARTemp->AttachToHolster(OwnerCharacter->GetMesh());

    SetSlotWeapon(EWeaponSlot::MAIN_GUN, ARTemp);*/

    FActorSpawnParameters Param3{};
    Param3.Owner = OwnerCharacter;
    AC_ThrowingWeapon* ThrowTemp = GetWorld()->SpawnActor<AC_ThrowingWeapon>(WeaponClasses[EWeaponSlot::THROWABLE_WEAPON], Param3);
    ThrowTemp->SetOwnerCharacter(OwnerCharacter);
    ThrowTemp->AttachToHolster(OwnerCharacter->GetMesh());

    SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, ThrowTemp);

    AC_ThrowingWeapon::InitTestPool(OwnerCharacter, WeaponClasses[EWeaponSlot::THROWABLE_WEAPON], this);
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


