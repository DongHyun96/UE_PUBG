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
* SlotWidget들의 경우 블루프린트의 내용을 C++로 이전하면서 이미지만 띄우고 있음
* 블루프린트에서는 우클릭까지 가능 했었음

<br/><br/><br/><br/><br/>

# \# 김동현
### **구조 설계**
*   **Character basic FSM 구조 설계**
    *   Character MainState / HandState / PoseState
*   **Weapon 기본 구조 및 Weapon Button strategy pattern 구조 설계**

### **Character Implementation**
*   **Character HandState WEAPON\_MELEE / WEAPON\_THROWABLE Blendspaces 구현**
    *   WEAPON\_MELEE - Stand BS
    *   WEAPON\_MELEE - Crouch BS
    *   WEAPON\_MELEE - Crawl BS
    *   WEAPON\_THROWABLE - Stand BS
    *   WEAPON\_THROWABLE - Crouch BS
    *   WEAPON\_THROABLE - Stand BS

*   **AnimMontage Priority에 따른 PlayAnimMontage 방식 수정**
    *   PriorityAnimMontage 구조 설계 및 구현
    *   AnimMontage끼리의 우선순위에 따른 재생 구현

*   **PoseState별 Collider 처리 및 지형지물에 따른 자세전환 제한 구현**
*   자세 및 Boost 량에 따른 속도 조절
*   **Swimming State** 구현
*   **SkyDiving & Parachuting State** 구현

### 투척류 Implementation
*   **Grenade**
*   **SmokeGrenade**
*   **FlashBang**
*   **투척 예상 경로 그리기 구현**
*   **투척류 폭발 Strategy에 따라 폭발 처리 구조 Strategy pattern 구현**
*   **투척류 현 State에 따른 캐릭터 모션처리**

### Melee Weapon Implementation
*   **근접 무기 프라이팬 구현**

### EquippedComponent Implementation
*   **캐릭터에 장착된 총기1, 2 / 투척류 / 근접무기 Slot 관리 구현**
    *   캐릭터 HandState 전환( ex) 총기1 상태에서 투척류 전환 or 무기 집어넣기/꺼내기)에 따른 무기 전환 처리 구현

### StatComponent Implementation
*   **캐릭터 HP / Boosting Amount / Oxygen Amount(수영 시 산소량)에 따른 캐릭터 상태 처리**
*   **부위별 피격 Damage에 따른 Damage량 조절**

### Consumable Item Usage Implementation
*   **Template method pattern 구조 및 FSM을 통한 Consumable Item 사용 기능 구현**
    *   Consumable Item state
        *   Idle / Activating / ActivateCompleted / Used
*   **Heal Item Usage**
    *   MedKit Usage(의료용 키트)
    *   FirstAidKit Usage(구급 상자)
    *   Bandage Usage(붕대)
*   **Boost Item Usage**
    *   PainKiller Usage(진통제)
    *   EnergyDrink Usage(에너지드링크)

### Ping System Implementation
*   **Ping marker 표시 기능 구현**
    *   InGame Level에서 Player가 바라보는 지점 Ping 찍기 기능 구현
    *   Map에서 Ping 찍기 기능 구현

### UI / Player HUD Implementation
*   플레이어 Stat에 따른 Stat 표기
    *   Player HP / Boosting Amount / Oxygen Amount
*   MiniMap 기능 구현
*   MainMap 기능 구현
*   SkyDiving State 시 현재 고도 및 속력 측정계 구현
*   상단 Compass Bar 구현
*   Inform 및 warning 로그 메시지 구현
*   킬로그 및 킬피드
*   GameOver widget
*   Tutorial Widgets
*   Training ground widgets
*   Lobby UI

### Airplane Manager Implementation
*   게임 첫 시작 비행 관리 구현
*   Random한 비행 동선 및 비행기 객체 관리 구현
*   Character들 SkyDive State 전환 관리 구현

### 자기장 System Implementation
*   Random Next Circle 기능 구현
*   Current Circle -> Next Circle 줄어드는 알고리즘 구현
*   자기장 페이즈 조절 구현

### Parkour System Implementation
*   지형지물 검사 및 각 지형지물에 맞는 Parkour action 종류 검사 pipeline 구현
*   Parkour actions
    *   Vaulting Low / High action
    *   Mantling Low / High action

### Enemy AI 구현
*   **상황 별 기본 FSM 처리 구현**
*   **Behavior tree 구현**
    *   **Idle condition Task(Sequence)**
        *   Custom wait task
        *   Stat care task
        *   Custom Move to task
        *   Take cover & stat care sequence
            1. Run EQS Query Task
                1. Find best cover location from TargetCharacter location
            2. Move To Cover location
            3. Try change pose
            4. Try using smoke grenade
            5. Wait few seconds
            6. Try stat care

       *   **Combat condition sequence**
        1. SwapWeapon task
        2. Attack task
        3. Wait task

       *   **SkyDive task**

*   **Custom EQS trace test 구현**
    *   기본 EQS trace test에서 ignore actor 추가 기능이 없음
    *   Custom EQS trace test 클래스(UC\_EQSCustomTraceTest)에서 필요한 ignore actor들 추가 처리

*   **Training ground 각 TestField 기능에 해당하는 AI Tester 기능 구현**
    *   AI movement tester
    *   AI Stat-care & Take cover tester
    *   AI Sky dive tester
    *   AI Combat tester
    *   InGame playable AI Enemy (게임플레이에서 사용하는 위의 기능 모두 통합 처리한 Enemy type)

### Training ground Level / Lobby Level 구현



<br/><br/><br/><br/><br/>

## # 박현호

### Animation System
* AnimBasicCharacter
* Player의 Animation과 Function 전반을 관리하는 핵심 클래스.
* Aim Offset 및 Lerp를 활용하여 자연스러운 시선 이동 구현:
    * 시선 이동: 머리와 시선이 먼저 움직이고 이후 몸체가 따라오는 방식으로 애니메이션 처리.
* C++에서 필요한 수치와 Boolean 값을 처리하여 Blueprint에 전달.
* 애니메이션은 ABP_Player에서 집중적으로 처리.
* **Player의 상태(State)**에 따라 애니메이션을 동적으로 전환.

### Weapon System
* Gun
    * Item Class를 상속받아 제작된 Weapon Class.
    * 무기 관련 모든 상호작용과 기능을 이 클래스에서 처리.
 
### Bullet Firing System
* Firing Logic
    * Player의 상태에 따라 총알 발사 로직이 상이.
        * 조준(Aim) 모드 및 일반 모드:
            * Crosshair 내부에서 Ray Trace를 수행하여 충돌 위치를 결정.
            * 총구에서 충돌 위치까지 총알을 발사.
            * 100m 이내 충돌:
                * 중력을 적용하지 않고 탄을 직선으로 발사.
            * 100m 초과 충돌:
                * 중력을 적용하여 탄도가 자연스럽게 낙차를 보이도록 설정.
        * Aim Down Sight (ADS) 모드:
            * Zeroing(영점) 거리: 100m.
            * 화면 중심에서 Ray Trace를 수행하여 100m 지점에 닿도록 발사 각도 조정.
            * 항상 중력 적용하여 사실적인 탄도를 구현.

### Timeline Component
* CurveFloat 데이터를 기반으로 Tick 독립적으로 작동.
* 총기 발사 반동, Aim 시 카메라 이동, Controller 진동 등의 처리를 담당.
* CurveFloat 값 조정으로 세부적인 진동 및 이동 효과 설정 가능.
* 결과적으로 부드럽고 세밀한 카메라 이동 및 진동 효과를 구현.
  
### Attachment System
* Attachment Component
    * Attachment 아이템들을 관리하는 핵심 클래스.
    * Player는 하나의 Attachment Component를 소유.
    * 2개의 총기를 사용할 수 있으므로 각 총기에 대한 Attachment Array를 관리.
* Attachment Actor
    * 추상 클래스로 설계:
        * 자식 클래스들이 특정 총기와의 상호작용을 독립적으로 구현 가능.
* Attachment 종류
    * Red Dot Sight:
        * 기본적으로 총기에 장착.
        * Ray Trace를 통해 화면 중심에서 Hit Event 발생 시 붉은 점 표시.
        * Red Dot이 범위를 벗어나면 붉은 점을 비활성화.
    * 4x Scope:
        * Render Target 및 Scene Capture 2D를 활용하여 Scope의 유리에 확대 이미지를 렌더링.
        * Bump Mapping을 적용하여 2D 이미지를 통해 깊이감을 부여.

### Crosshair Widget Component
* 캐릭터 상태에 따라 Crosshair의 크기, 가시성 등을 조정.
* 다양한 게임 상황(조준, 반동, 이동 등)에 따른 동적 처리를 지원.

### 추가 고려 사항
* 이벤트 중심 설계를 통해 성능 최적화:
    * 발사 로직 및 Crosshair 업데이트는 필요 시점에만 수행.
* 모듈화된 구조:
    * 각 기능은 독립적으로 설계되어 유지보수와 확장이 용이.
