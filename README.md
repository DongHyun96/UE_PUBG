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

### 구조 설계
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


## # 김동현
### 구조 설계
* Character basic FSM 구조 설계
    * Character MainState / HandState / PoseState
* Weapon 기본 구조 및 Weapon Button strategy pattern 구조 설계

### Character Implementation
* Character HandState WEAPON_MELEE / WEAPON_THROWABLE BlendSpaces 구현
    * WEAPON_MELEE - Stand BS
    * WEAPON_MELEE - Crouch BS
    * WEAPON_MELEE - Crawl BS
    * WEAPON_THROWABLE - Stand BS
    * WEAPON_THROWABLE - Crouch BS 
    * WEAPON_THROABLE - Stand BS

* AnimMontage Priority에 따른 PlayAnimMontage 방식 수정
    * PriorityAnimMontage 구조 설계 및 구현
    * AnimMontage끼리의 우선순위에 따른 재생 구현

* PoseState별 Collider 처리 및 지형지물에 따른 자세전환 제한 구현
* 자세 및 Boost 량에 따른 속도 조절
* Swimming State 구현
* SkyDiving State 구현

### 투척류 Implementation
* Grenade
* SmokeGrenade
* FlashBang
* 투척 예상 경로 그리기 구현
* 투척류 폭발 Strategy에 따라 폭발 처리 구조 Strategy pattern 구현
* 투척류 현 State에 따른 캐릭터 모션처리

### Melee Weapon Implementation
* 근접 무기 프라이팬 구현

### EquippedComponent Implementation
* 캐릭터에 장착된 총기1, 2 / 투척류 / 근접무기 Slot 관리 구현
    * 캐릭터 HandState 전환( ex) 총기1 상태에서 투척류 전환 or 무기 집어넣기/꺼내기)에 따른 무기 전환 처리 구현

### StatComponent Implementation
* 캐릭터 HP / Boosting Amount / Oxygen Amount(수영 시 산소량)에 따른 캐릭터 상태 처리
* 부위별 피격 Damage에 따른 Damage량 조절

### Consumable Item Usage Implementation
* Template method pattern 구조를 통한 Consumable Item 사용 기능 구현
* Heal Item Usage
    * MedKit Usage(의료용 키트)
    * FirstAidKit Usage(구급 상자)
    * Bandage Usage(붕대)
* Boost Item Usage
    * PainKiller Usage(진통제)
    * EnergyDrink Usage(에너지드링크)
        
## # 박현호
