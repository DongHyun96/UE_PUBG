# Diff Details

Date : 2024-09-29 17:23:23

Directory c:\\Users\\wadan\\Desktop\\SGA\\Portfolio\\Unreal\\Project\\UE_PUBG\\Source\\UE_PUBG

Total : 110 files,  6839 codes, 1877 comments, 3015 blanks, all 11731 lines

[Summary](results.md) / [Details](details.md) / [Diff Summary](diff.md) / Diff Details

## Files
| filename | language | code | comment | blank | total |
| :--- | :--- | ---: | ---: | ---: | ---: |
| [Character/C_AnimBasicCharacter.cpp](/Character/C_AnimBasicCharacter.cpp) | C++ | 142 | 27 | 41 | 210 |
| [Character/C_AnimBasicCharacter.h](/Character/C_AnimBasicCharacter.h) | C++ | 71 | 7 | 38 | 116 |
| [Character/C_AnimPlayer.cpp](/Character/C_AnimPlayer.cpp) | C++ | 1 | 1 | 4 | 6 |
| [Character/C_AnimPlayer.h](/Character/C_AnimPlayer.h) | C++ | 9 | 4 | 5 | 18 |
| [Character/C_BasicCharacter.cpp](/Character/C_BasicCharacter.cpp) | C++ | 155 | 24 | 51 | 230 |
| [Character/C_BasicCharacter.h](/Character/C_BasicCharacter.h) | C++ | 185 | 62 | 98 | 345 |
| [Character/C_Enemy.cpp](/Character/C_Enemy.cpp) | C++ | 1 | 1 | 4 | 6 |
| [Character/C_Enemy.h](/Character/C_Enemy.h) | C++ | 9 | 4 | 5 | 18 |
| [Character/C_Player.cpp](/Character/C_Player.cpp) | C++ | 601 | 160 | 208 | 969 |
| [Character/C_Player.h](/Character/C_Player.h) | C++ | 155 | 86 | 96 | 337 |
| [Character/C_WorldPingActor.cpp](/Character/C_WorldPingActor.cpp) | C++ | 53 | 3 | 26 | 82 |
| [Character/C_WorldPingActor.h](/Character/C_WorldPingActor.h) | C++ | 25 | 3 | 18 | 46 |
| [Character/Component/C_ConsumableUsageMeshComponent.cpp](/Character/Component/C_ConsumableUsageMeshComponent.cpp) | C++ | 52 | 10 | 19 | 81 |
| [Character/Component/C_ConsumableUsageMeshComponent.h](/Character/Component/C_ConsumableUsageMeshComponent.h) | C++ | 42 | 10 | 21 | 73 |
| [Character/Component/C_EquippedComponent.cpp](/Character/Component/C_EquippedComponent.cpp) | C++ | 182 | 38 | 66 | 286 |
| [Character/Component/C_EquippedComponent.h](/Character/Component/C_EquippedComponent.h) | C++ | 61 | 40 | 38 | 139 |
| [Character/Component/C_InputComponent.cpp](/Character/Component/C_InputComponent.cpp) | C++ | 387 | 53 | 127 | 567 |
| [Character/Component/C_InputComponent.h](/Character/Component/C_InputComponent.h) | C++ | 97 | 19 | 51 | 167 |
| [Character/Component/C_InvenComponent.cpp](/Character/Component/C_InvenComponent.cpp) | C++ | 169 | 73 | 47 | 289 |
| [Character/Component/C_InvenComponent.h](/Character/Component/C_InvenComponent.h) | C++ | 64 | 29 | 36 | 129 |
| [Character/Component/C_PingSystemComponent.cpp](/Character/Component/C_PingSystemComponent.cpp) | C++ | 48 | 3 | 23 | 74 |
| [Character/Component/C_PingSystemComponent.h](/Character/Component/C_PingSystemComponent.h) | C++ | 28 | 10 | 25 | 63 |
| [Character/Component/C_PoseColliderHandlerComponent.cpp](/Character/Component/C_PoseColliderHandlerComponent.cpp) | C++ | 185 | 15 | 74 | 274 |
| [Character/Component/C_PoseColliderHandlerComponent.h](/Character/Component/C_PoseColliderHandlerComponent.h) | C++ | 62 | 44 | 40 | 146 |
| [Character/Component/C_StatComponent.cpp](/Character/Component/C_StatComponent.cpp) | C++ | 95 | 11 | 45 | 151 |
| [Character/Component/C_StatComponent.h](/Character/Component/C_StatComponent.h) | C++ | 99 | 30 | 47 | 176 |
| [HUD/C_HUDWidget.cpp](/HUD/C_HUDWidget.cpp) | C++ | 8 | 1 | 6 | 15 |
| [HUD/C_HUDWidget.h](/HUD/C_HUDWidget.h) | C++ | 56 | 34 | 34 | 124 |
| [HUD/C_MainMapWidget.cpp](/HUD/C_MainMapWidget.cpp) | C++ | 227 | 45 | 92 | 364 |
| [HUD/C_MainMapWidget.h](/HUD/C_MainMapWidget.h) | C++ | 61 | 27 | 42 | 130 |
| [HUD/C_MapWidget.cpp](/HUD/C_MapWidget.cpp) | C++ | 10 | 1 | 6 | 17 |
| [HUD/C_MapWidget.h](/HUD/C_MapWidget.h) | C++ | 25 | 40 | 17 | 82 |
| [InvenUserInterface/C_EquipSlot.cpp](/InvenUserInterface/C_EquipSlot.cpp) | C++ | 1 | 1 | 4 | 6 |
| [InvenUserInterface/C_EquipSlot.h](/InvenUserInterface/C_EquipSlot.h) | C++ | 9 | 4 | 5 | 18 |
| [InvenUserInterface/C_InvenUiWidget.cpp](/InvenUserInterface/C_InvenUiWidget.cpp) | C++ | 1 | 1 | 4 | 6 |
| [InvenUserInterface/C_InvenUiWidget.h](/InvenUserInterface/C_InvenUiWidget.h) | C++ | 9 | 4 | 5 | 18 |
| [Item/Attachment/C_AttachableItem.cpp](/Item/Attachment/C_AttachableItem.cpp) | C++ | 1 | 1 | 6 | 8 |
| [Item/Attachment/C_AttachableItem.h](/Item/Attachment/C_AttachableItem.h) | C++ | 11 | 4 | 7 | 22 |
| [Item/C_Item.cpp](/Item/C_Item.cpp) | C++ | 31 | 8 | 19 | 58 |
| [Item/C_Item.h](/Item/C_Item.h) | C++ | 69 | 25 | 21 | 115 |
| [Item/ConsumableItem/Boosting/C_EnergyDrink.cpp](/Item/ConsumableItem/Boosting/C_EnergyDrink.cpp) | C++ | 37 | 1 | 14 | 52 |
| [Item/ConsumableItem/Boosting/C_EnergyDrink.h](/Item/ConsumableItem/Boosting/C_EnergyDrink.h) | C++ | 19 | 6 | 10 | 35 |
| [Item/ConsumableItem/Boosting/C_PainKiller.cpp](/Item/ConsumableItem/Boosting/C_PainKiller.cpp) | C++ | 37 | 1 | 13 | 51 |
| [Item/ConsumableItem/Boosting/C_PainKiller.h](/Item/ConsumableItem/Boosting/C_PainKiller.h) | C++ | 19 | 6 | 10 | 35 |
| [Item/ConsumableItem/C_BoostItem.cpp](/Item/ConsumableItem/C_BoostItem.cpp) | C++ | 31 | 1 | 11 | 43 |
| [Item/ConsumableItem/C_BoostItem.h](/Item/ConsumableItem/C_BoostItem.h) | C++ | 21 | 4 | 15 | 40 |
| [Item/ConsumableItem/C_ConsumableItem.cpp](/Item/ConsumableItem/C_ConsumableItem.cpp) | C++ | 116 | 16 | 45 | 177 |
| [Item/ConsumableItem/C_ConsumableItem.h](/Item/ConsumableItem/C_ConsumableItem.h) | C++ | 47 | 35 | 35 | 117 |
| [Item/ConsumableItem/C_HealItem.cpp](/Item/ConsumableItem/C_HealItem.cpp) | C++ | 42 | 2 | 13 | 57 |
| [Item/ConsumableItem/C_HealItem.h](/Item/ConsumableItem/C_HealItem.h) | C++ | 24 | 5 | 16 | 45 |
| [Item/ConsumableItem/Healing/C_Bandage.cpp](/Item/ConsumableItem/Healing/C_Bandage.cpp) | C++ | 53 | 3 | 18 | 74 |
| [Item/ConsumableItem/Healing/C_Bandage.h](/Item/ConsumableItem/Healing/C_Bandage.h) | C++ | 25 | 6 | 15 | 46 |
| [Item/ConsumableItem/Healing/C_FirstAidKit.cpp](/Item/ConsumableItem/Healing/C_FirstAidKit.cpp) | C++ | 47 | 2 | 17 | 66 |
| [Item/ConsumableItem/Healing/C_FirstAidKit.h](/Item/ConsumableItem/Healing/C_FirstAidKit.h) | C++ | 20 | 6 | 10 | 36 |
| [Item/ConsumableItem/Healing/C_MedKit.cpp](/Item/ConsumableItem/Healing/C_MedKit.cpp) | C++ | 64 | 6 | 22 | 92 |
| [Item/ConsumableItem/Healing/C_MedKit.h](/Item/ConsumableItem/Healing/C_MedKit.h) | C++ | 27 | 6 | 14 | 47 |
| [Item/Equipment/C_BackPack.cpp](/Item/Equipment/C_BackPack.cpp) | C++ | 61 | 14 | 28 | 103 |
| [Item/Equipment/C_BackPack.h](/Item/Equipment/C_BackPack.h) | C++ | 27 | 14 | 19 | 60 |
| [Item/Equipment/C_EquipableItem.cpp](/Item/Equipment/C_EquipableItem.cpp) | C++ | 16 | 1 | 8 | 25 |
| [Item/Equipment/C_EquipableItem.h](/Item/Equipment/C_EquipableItem.h) | C++ | 16 | 7 | 9 | 32 |
| [Item/Weapon/C_Weapon.cpp](/Item/Weapon/C_Weapon.cpp) | C++ | 75 | 5 | 22 | 102 |
| [Item/Weapon/C_Weapon.h](/Item/Weapon/C_Weapon.h) | C++ | 47 | 49 | 36 | 132 |
| [Item/Weapon/Gun/C_AR.cpp](/Item/Weapon/Gun/C_AR.cpp) | C++ | 15 | 2 | 7 | 24 |
| [Item/Weapon/Gun/C_AR.h](/Item/Weapon/Gun/C_AR.h) | C++ | 15 | 6 | 8 | 29 |
| [Item/Weapon/Gun/C_Bullet.cpp](/Item/Weapon/Gun/C_Bullet.cpp) | C++ | 165 | 24 | 59 | 248 |
| [Item/Weapon/Gun/C_Bullet.h](/Item/Weapon/Gun/C_Bullet.h) | C++ | 45 | 11 | 16 | 72 |
| [Item/Weapon/Gun/C_Gun.cpp](/Item/Weapon/Gun/C_Gun.cpp) | C++ | 279 | 73 | 74 | 426 |
| [Item/Weapon/Gun/C_Gun.h](/Item/Weapon/Gun/C_Gun.h) | C++ | 95 | 14 | 24 | 133 |
| [Item/Weapon/Gun/C_SR.cpp](/Item/Weapon/Gun/C_SR.cpp) | C++ | 1 | 1 | 4 | 6 |
| [Item/Weapon/Gun/C_SR.h](/Item/Weapon/Gun/C_SR.h) | C++ | 9 | 4 | 5 | 18 |
| [Item/Weapon/MeleeWeapon/C_MeleeWeapon.cpp](/Item/Weapon/MeleeWeapon/C_MeleeWeapon.cpp) | C++ | 70 | 31 | 26 | 127 |
| [Item/Weapon/MeleeWeapon/C_MeleeWeapon.h](/Item/Weapon/MeleeWeapon/C_MeleeWeapon.h) | C++ | 49 | 17 | 31 | 97 |
| [Item/Weapon/ThrowingWeapon/C_FlashBangExplode.cpp](/Item/Weapon/ThrowingWeapon/C_FlashBangExplode.cpp) | C++ | 97 | 22 | 48 | 167 |
| [Item/Weapon/ThrowingWeapon/C_FlashBangExplode.h](/Item/Weapon/ThrowingWeapon/C_FlashBangExplode.h) | C++ | 21 | 21 | 21 | 63 |
| [Item/Weapon/ThrowingWeapon/C_GrenadeExplode.cpp](/Item/Weapon/ThrowingWeapon/C_GrenadeExplode.cpp) | C++ | 159 | 71 | 73 | 303 |
| [Item/Weapon/ThrowingWeapon/C_GrenadeExplode.h](/Item/Weapon/ThrowingWeapon/C_GrenadeExplode.h) | C++ | 24 | 29 | 21 | 74 |
| [Item/Weapon/ThrowingWeapon/C_ScreenShotWidget.cpp](/Item/Weapon/ThrowingWeapon/C_ScreenShotWidget.cpp) | C++ | 17 | 13 | 12 | 42 |
| [Item/Weapon/ThrowingWeapon/C_ScreenShotWidget.h](/Item/Weapon/ThrowingWeapon/C_ScreenShotWidget.h) | C++ | 17 | 4 | 12 | 33 |
| [Item/Weapon/ThrowingWeapon/C_SmokeGrndExplode.cpp](/Item/Weapon/ThrowingWeapon/C_SmokeGrndExplode.cpp) | C++ | 15 | 1 | 8 | 24 |
| [Item/Weapon/ThrowingWeapon/C_SmokeGrndExplode.h](/Item/Weapon/ThrowingWeapon/C_SmokeGrndExplode.h) | C++ | 12 | 1 | 8 | 21 |
| [Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.cpp](/Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.cpp) | C++ | 362 | 69 | 140 | 571 |
| [Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h](/Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h) | C++ | 138 | 52 | 88 | 278 |
| [Item/Weapon/ThrowingWeapon/I_ExplodeStrategy.cpp](/Item/Weapon/ThrowingWeapon/I_ExplodeStrategy.cpp) | C++ | 1 | 2 | 4 | 7 |
| [Item/Weapon/ThrowingWeapon/I_ExplodeStrategy.h](/Item/Weapon/ThrowingWeapon/I_ExplodeStrategy.h) | C++ | 15 | 10 | 7 | 32 |
| [Item/Weapon/WeaponStrategy/C_GunStrategy.cpp](/Item/Weapon/WeaponStrategy/C_GunStrategy.cpp) | C++ | 90 | 7 | 27 | 124 |
| [Item/Weapon/WeaponStrategy/C_GunStrategy.h](/Item/Weapon/WeaponStrategy/C_GunStrategy.h) | C++ | 24 | 4 | 11 | 39 |
| [Item/Weapon/WeaponStrategy/C_MeleeWeaponStrategy.cpp](/Item/Weapon/WeaponStrategy/C_MeleeWeaponStrategy.cpp) | C++ | 50 | 1 | 24 | 75 |
| [Item/Weapon/WeaponStrategy/C_MeleeWeaponStrategy.h](/Item/Weapon/WeaponStrategy/C_MeleeWeaponStrategy.h) | C++ | 19 | 4 | 11 | 34 |
| [Item/Weapon/WeaponStrategy/C_ThrowingWeaponStrategy.cpp](/Item/Weapon/WeaponStrategy/C_ThrowingWeaponStrategy.cpp) | C++ | 55 | 2 | 23 | 80 |
| [Item/Weapon/WeaponStrategy/C_ThrowingWeaponStrategy.h](/Item/Weapon/WeaponStrategy/C_ThrowingWeaponStrategy.h) | C++ | 19 | 5 | 11 | 35 |
| [Item/Weapon/WeaponStrategy/I_WeaponButtonStrategy.cpp](/Item/Weapon/WeaponStrategy/I_WeaponButtonStrategy.cpp) | C++ | 1 | 2 | 4 | 7 |
| [Item/Weapon/WeaponStrategy/I_WeaponButtonStrategy.h](/Item/Weapon/WeaponStrategy/I_WeaponButtonStrategy.h) | C++ | 22 | 54 | 17 | 93 |
| [MagneticField/C_MagneticFieldManager.cpp](/MagneticField/C_MagneticFieldManager.cpp) | C++ | 142 | 21 | 64 | 227 |
| [MagneticField/C_MagneticFieldManager.h](/MagneticField/C_MagneticFieldManager.h) | C++ | 84 | 39 | 46 | 169 |
| [MagneticField/C_MagneticWall.cpp](/MagneticField/C_MagneticWall.cpp) | C++ | 26 | 6 | 13 | 45 |
| [MagneticField/C_MagneticWall.h](/MagneticField/C_MagneticWall.h) | C++ | 19 | 4 | 12 | 35 |
| [Singleton/C_GameSceneManager.cpp](/Singleton/C_GameSceneManager.cpp) | C++ | 17 | 3 | 7 | 27 |
| [Singleton/C_GameSceneManager.h](/Singleton/C_GameSceneManager.h) | C++ | 19 | 7 | 14 | 40 |
| [UE_PUBG.Build.cs](/UE_PUBG.Build.cs) | C# | 23 | 17 | 9 | 49 |
| [UE_PUBG.cpp](/UE_PUBG.cpp) | C++ | 3 | 1 | 3 | 7 |
| [UE_PUBG.h](/UE_PUBG.h) | C++ | 2 | 1 | 3 | 6 |
| [UE_PUBGCharacter.cpp](/UE_PUBGCharacter.cpp) | C++ | 79 | 27 | 24 | 130 |
| [UE_PUBGCharacter.h](/UE_PUBGCharacter.h) | C++ | 39 | 13 | 22 | 74 |
| [UE_PUBGGameMode.cpp](/UE_PUBGGameMode.cpp) | C++ | 12 | 6 | 6 | 24 |
| [UE_PUBGGameMode.h](/UE_PUBGGameMode.h) | C++ | 11 | 1 | 8 | 20 |
| [Utility/C_TimelineComponent.cpp](/Utility/C_TimelineComponent.cpp) | C++ | 0 | 0 | 1 | 1 |
| [Utility/C_TimelineUtility.cpp](/Utility/C_TimelineUtility.cpp) | C++ | 7 | 1 | 5 | 13 |
| [Utility/C_TimelineUtility.h](/Utility/C_TimelineUtility.h) | C++ | 25 | 9 | 14 | 48 |
| [Utility/C_Util.cpp](/Utility/C_Util.cpp) | C++ | 43 | 1 | 12 | 56 |
| [Utility/C_Util.h](/Utility/C_Util.h) | C++ | 19 | 4 | 7 | 30 |

[Summary](results.md) / [Details](details.md) / [Diff Summary](diff.md) / Diff Details