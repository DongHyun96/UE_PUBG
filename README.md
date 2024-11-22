# UE_PUBG
Developed with Unreal Engine 5

---

# 제작
  * 김동현 (kdh09@ajou.ac.kr)
  * 박현호 (jkiaj001@gmail.com)
  * 전상연 (johnsjun@naver.com)

---

# 구현한 파트 및 구현 내용 정리

## # 전상연

### 구조설계
* 아이템 구조 설계
* 캐릭터 Inventory 구조 설계

### Character Implementation
* Character basic input mapping
    * Character Movement input mapping
* Character HandState UNARMED Blendspace 구현
    * UNARMED - Stand BS
    * UNARMED - Crouch BS
    * UNARMED - Crawl BS

### AC_Item Implementation
* MoveToInven, Around, Slot을 통해 아이템의 시스템 위치 이동을 구현
* SpawnItem을 통해 새로운 아이템 객체 생성을 구현
* FItemData를 통해 아이템의 기본 정보를 묶어 놓음

### Inven Implementation
* 외부에서 들어오는 아이템에 대하여 VolumeCheck를 통한 아이템 습득 가/부를 결정
* ItemList에 대한 출입함수 구현
* BackPack에 따른 MaxVolume조절 구현
* InvenUI와 상호 작용

### InvenUI Implementation
* Around Item List Widget 구현
* Inventoru Item List Widget 구현
* Volume System 구현
* MainGunSlot, SubGunSlot Widget 구현(이미지만)
* ThrowableWeaponSlot Widget 구현(이미지만)
* SlotWidget들의 경우 블프의 내용을 C++로 이전하면서 이미지만 띄우고 있음
* 블프에서는 우클릭까지 가능 했었음
<br/><br/><br/>

## # 김동현


## # 박현호
