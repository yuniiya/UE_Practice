// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacter.h"
#include "ABAnimInstance.h"
#include "ABWeapon.h"
#include "ABCharacterStatComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/WidgetComponent.h"		// 위젯 컴포넌트를 사용할 수 있도록 추가 
#include "ABCharacterWidget.h"
#include "ABAIController.h"
#include "ABCharacterSetting.h"
#include "ABGameInstance.h"
#include "ABPlayerController.h"
#include "ABPlayerState.h"
#include "ABHUDWidget.h"
#include "ABGameMode.h"
#include "VisualLogger/VisualLogger.h"

// Sets default values
AABCharacter::AABCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	CharacterStat = CreateDefaultSubobject<UABCharacterStatComponent>(TEXT("CHARACTERSTAT"));
	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBARWIDGET"));

	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);
	HPBarWidget->SetupAttachment(GetMesh());


	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->SetRelativeRotation(FRotator(-50.0f, 0.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CARDBOARD(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Warrior.SK_CharM_Warrior"));
	if (SK_CARDBOARD.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_CARDBOARD.Object);
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(TEXT("/Game/Book/BluePrints/WarriorAnimBluePrint.WarriorAnimBluePrint_C"));
	if (WARRIOR_ANIM.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(WARRIOR_ANIM.Class);
	}

//	SetControlMode(EControlMode::DIABLO);

	ArmLengthSpeed = 3.0f;
	ArmRotationSpeed = 10.0f;
	GetCharacterMovement()->JumpZVelocity = 800.0f;		// 점프 높이 조정
	GetCharacterMovement()->GravityScale = 2.f;

	IsAttacking = false;

	MaxCombo = 4;
	AttackEndComboState();

	// 캐릭터의 캡슐 컴포넌트가 ABCharacter 프리셋을 사용하도록 설정
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("ABCharacter"));

	AttackRange = 250.0f;
	AttackRadius = 50.0f;

	HPBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 180.f));
	// EWidgetSpace::Screen : 항상 플레이어를 향해 보도록
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);		
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_HUD(TEXT("/Game/Book/UI/UI_HPBar.UI_HPBar_C"));
	if (UI_HUD.Succeeded())
	{
		HPBarWidget->SetWidgetClass(UI_HUD.Class);
		HPBarWidget->SetDrawSize(FVector2D(150.f, 50.f));
	}

	// 레벨에 배치하거나 새롭게 생성되는 캐릭터마다 AIController 엑터를 가지고, 플레이어 폰을 제외한 모든 폰은 AIController의 지배를 받음.
	AIControllerClass = AABAIController::StaticClass();						// AIController 클래스 속성 지정
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;					// AI 생성 옵션 

	// 플레이어가 사용할 에셋
	AssetIndex = 4;

	// PREINIT 스테이트(캐릭터 생성 전)에서는 캐릭터와 UI를 숨김.
	SetActorHiddenInGame(true);
	HPBarWidget->SetHiddenInGame(true);
	SetCanBeDamaged(false);

	DeadTimer = 5.f;
}

void AABCharacter::SetCharacterState(ECharacterState NewState)
{
	ABCHECK(CurrentState != NewState);
	CurrentState = NewState;

	switch (CurrentState)
	{
	case ECharacterState::PREINIT:
		break;
	case ECharacterState::LOADING:
	{
		if (true == bIsPlayer)
		{
			// 입력 비활성화
			DisableInput(ABPlayerController);

			// HUD 위젯과 캐릭터 스탯 컴포넌트를 연결
			ABPlayerController->GetHUDWidget()->BindCharacterStat(CharacterStat);

			// 플레이어 컨트롤러가 캐릭터에 빙의할 때, 캐릭터의 PlayerState 속성에 플레이어 스테이트의 포인터를 저장함.
			// 따라서 캐릭터의 PlayerState를 통해 정보를 가져올 수 있음.
			auto ABPlayerState = Cast<AABPlayerState>(GetPlayerState());
			ABCHECK(nullptr != ABPlayerState);
			CharacterStat->SetNewLevel(ABPlayerState->GetCharacterLevel());
		}
		else
		{
			// 현재 게임 스코어가 높을수록 높은 레벨의 NPC 생성
			auto ABGameMode = Cast<AABGameMode>(GetWorld()->GetAuthGameMode());
			ABCHECK(nullptr != ABGameMode);
			// CeilToInt() : 소수점 올림
			int32 TargetLevel = FMath::CeilToInt(((float)ABGameMode->GetScore() * 0.8f));

			// Clamp(): 같이 범위 안에 있는지 검사하고, 참이라면 그 값을 반환
			int32 FinalLevel = FMath::Clamp<int32>(TargetLevel, 1, 20);
			ABLOG(Warning, TEXT("New NPC Level : %d"), FinalLevel);
			CharacterStat->SetNewLevel(FinalLevel);
		}

		SetActorHiddenInGame(true);
		HPBarWidget->SetHiddenInGame(true);
		SetCanBeDamaged(false);
	}
		break;
	case ECharacterState::READY:
	{
		SetActorHiddenInGame(false);
		HPBarWidget->SetHiddenInGame(false);
		SetCanBeDamaged(true);

		CharacterStat->OnHPIsZero.AddLambda([this]() -> void
		{
			SetCharacterState(ECharacterState::DEAD);
		});


		// 언리얼 4.21버전부터 위젯의 초기화 시점이 PostInitalizeComponents -> BeginPlay 로 변경.
		// 캐릭터의 위젯 컴포넌트와 UI 위젯을 연결
		auto CharacterWidget = Cast<UABCharacterWidget>(HPBarWidget->GetUserWidgetObject());
		ABCHECK(nullptr != CharacterWidget);
		CharacterWidget->BindCharacterStat(CharacterStat);

		if (true == bIsPlayer)
		{
			SetControlMode(EControlMode::DIABLO);
			GetCharacterMovement()->MaxWalkSpeed = 600.f;
			EnableInput(ABPlayerController);
		}
		else
		{
			SetControlMode(EControlMode::NPC);
			GetCharacterMovement()->MaxWalkSpeed = 400.f;
			// 비헤이비어 트리 로직 실행
			ABAIController->RunAI();
		}
	}
		break;
	case ECharacterState::DEAD:
	{
		SetActorEnableCollision(false);
		GetMesh()->SetHiddenInGame(false);
		HPBarWidget->SetHiddenInGame(true);
		ABAnim->SetDeadAnim();
		SetCanBeDamaged(false);

		if (true == bIsPlayer)
		{
			// 입력 처리 중단
			DisableInput(ABPlayerController);
		}
		else
		{
			// 비헤이비어 트리 중단
			ABAIController->StopAI();
		}

		GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([this]() -> void
		{
			if (true == bIsPlayer)
			{
				ABPlayerController->ShowResultUI();
			}
			else
			{
				Destroy();
			}

		}), DeadTimer, false);
	}
		break;
	default:
		break;
	}
}

ECharacterState AABCharacter::GetCharacterState() const
{
	return CurrentState;
}

int32 AABCharacter::GetExp() const
{
	return CharacterStat->GetDropExp();
}

float AABCharacter::GetFinalAttackRange() const
{
	// 현재 무기에 따라 공격 범위를 다르게 설정
	return (nullptr != CurrentWeapon) ? CurrentWeapon->GetAttackRange() : AttackRange;
}

float AABCharacter::GetFinalAttackDamage() const
{
	float AttackDamage = (nullptr != CurrentWeapon) ? (CharacterStat->GetAttack() + CurrentWeapon->GetAttackDamage()) : CharacterStat->GetAttack();
	float AttackModifier = (nullptr != CurrentWeapon) ? CurrentWeapon->GetAttackModifier() : 1.f;

	return AttackDamage * AttackModifier;
}

// Called when the game starts or when spawned
void AABCharacter::BeginPlay()
{
	Super::BeginPlay();

	ABLOG(Warning, TEXT("Character Name : %s, ID : %d, Location X : %.3f"),
		*GetName(), ID, GetActorLocation().X);

	bIsPlayer = IsPlayerControlled();
	if (true == bIsPlayer)
	{
		ABPlayerController = Cast<AABPlayerController>(GetController());
		ABCHECK(nullptr != ABPlayerController);
	}
	else
	{
		ABAIController = Cast<AABAIController>(GetController());
		ABCHECK(nullptr != ABAIController);
	}

	// GetDefault() : 메모리에 올라간 클래스 기본 객체를 가져오는 함수 
	// 클래스 기본 객체 : 모듈은 로딩되면서 자신에게 속한 모든 언리얼 오브젝트의 기본값을 지정하고 생성하는데, 이를 클래스 기본객체라고 함.

	auto DefaultSetting = GetDefault<UABCharacterSetting>();

	if (true == bIsPlayer)
	{
		auto ABPlayerState = Cast<AABPlayerState>(GetPlayerState());
		ABCHECK(nullptr != ABPlayerState);
		AssetIndex = ABPlayerState->GetCharacterIndex();
	}
	else
	{
		// NPC가 생성될 때, 에셋 목록 중 랜덤으로 지정
		AssetIndex = FMath::RandRange(0, DefaultSetting->CharacterAssets.Num() - 1);
	}

	// RequestAsyncLoad(): 비동기 방식으로 에셋을 로딩하는 명령. FStreamableDelegate 형식의 델리게이트를 넘겨줄 경우, 에셋이 로딩을 완료하면 델리게이트에 연결된 함수를 호출해줌.
	// CreateUObject(): 델리게이트 생성
	CharacterAssetToLoad = DefaultSetting->CharacterAssets[AssetIndex];
	auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
	ABCHECK(nullptr != ABGameInstance);
	AssetStreamingHandle = ABGameInstance->StreamableManager.RequestAsyncLoad(CharacterAssetToLoad, FStreamableDelegate::CreateUObject(this, &AABCharacter::OnAssetLoadCompleted));
	SetCharacterState(ECharacterState::LOADING);

	//FName WeaponSocket(TEXT("hand_rSocket"));
	////SpawnActor<생성할 엑터 클래스>(위치, 회전 값) : 월드에 새로운 엑터를 생성하는 명령
	//auto CurWeapon = GetWorld()->SpawnActor<AABWeapon>(FVector::ZeroVector, FRotator::ZeroRotator);
	//if (nullptr != CurWeapon)
	//{
	//	// hand_rSocket 소켓 위치를 기준으로 무기의 트랜스폼이 설정
	//	CurWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
	//}
}

void AABCharacter::SetControlMode(EControlMode NewControlMode)
{
	CurrentControlMode = NewControlMode;

	switch (CurrentControlMode)
	{
	case AABCharacter::EControlMode::GTA:
	{
		ArmLengthTo = 450.0f;
		SpringArm->bUsePawnControlRotation = true;
		// bInherit ~ : 다음 속성이 true 라면 스프링 암이 상위 컴포넌트를 따라 회전.
		SpringArm->bInheritPitch = true;
		SpringArm->bInheritRoll = true;
		SpringArm->bInheritYaw = true;
		SpringArm->bDoCollisionTest = true;
		// 언리얼 캐릭터 모델은 기본으로 컨트롤 회전의 Yaw값과 폰의 Yaw값이 연동되어 있음.
		// bUseControllerRotationYaw은 이를 지정하는 속성. 이 설정이 true라면 마우스를 좌우로 움직이면 캐릭터는 z축으로 회전하지만, 마우스의 상하이동은 폰의 회전에 아무런 영향을 주지 않음. 
		bUseControllerRotationYaw = false;

		// bOrientRotationToMovement: 캐릭터가 움직이는 방향으로 캐릭터를 회전시켜주는 속성
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);	// 회전 속도
	}
		break;
	case AABCharacter::EControlMode::DIABLO:
	{
		ArmLengthTo = 800.0f;
		ArmRotationTo = FRotator(-45.0f, 0.0f, 0.0f);
		SpringArm->bUsePawnControlRotation = false;
		SpringArm->bInheritPitch = false;
		SpringArm->bInheritRoll = false;
		SpringArm->bInheritYaw = false;
		SpringArm->bDoCollisionTest = false;
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		// bOrientRotationToMovement 대신 bUseControllerDesiredRotation 속성을 체크해 캐릭터가 컨트롤러를 따라 부드럽게 회전하도록 함
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);

	}
		break;
	case AABCharacter::EControlMode::NPC:
	{
		// NPC는 이동 방향에 따라 회전하도록 캐릭터 무브먼트를 설정. 
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->RotationRate = FRotator(0.f, 480.f, 0.f);
	}
	break;
	default:
		break;
	}

}

// Called every frame
void AABCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// FInterpTo: 지정한 속력으로 목표 지점까지 서서히 진행
	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, ArmLengthTo, DeltaTime, ArmLengthSpeed);

	switch (CurrentControlMode)
	{
	case AABCharacter::EControlMode::GTA: 
		// FRotator 값으로 회전 행렬 생성 후, 이를 토대로 변환된 좌표계의 X, Y축 방향을 가져옴
		break;
	case AABCharacter::EControlMode::DIABLO:
	{
		if (DirectionToMove.SizeSquared() > 0.0f)
		{
			// MakeFromX : 하나의 벡터로부터 회전 행렬을 만드는 명령 (X축 = 시선 방향)
			// 하나의 벡터 값과 이에 직교하는 나머지 두 축을 구해 회전 행렬 생성
			GetController()->SetControlRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator());
			AddMovementInput(DirectionToMove);

			SpringArm->GetRelativeRotation() = FMath::RInterpTo(SpringArm->GetRelativeRotation(), ArmRotationTo, DeltaTime, ArmRotationSpeed);
		}
	}
		break;
	default:
		break;
	}

}

void AABCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ABAnim = Cast<UABAnimInstance>(GetMesh()->GetAnimInstance());
	ABCHECK(nullptr != ABAnim);

	// OnMontageEnded: 애니메이션 몽타주 재생이 끝나면 발동하는 델리게이트
	// 캐릭터의 OnMontageEnded 멤버 함수를 여기에 등록해 몽타주 재생이 끝나는 타이밍을 애님 인스턴스 쪽에서 파악할 수 있도록 함. 
	ABAnim->OnMontageEnded.AddDynamic(this, &AABCharacter::OnAttackMontageEnded);

	// [참조할 환경] (인자) -> 반환타입 {함수 로직}
	ABAnim->OnNextAttackCheck.AddLambda([this]() -> void
	{
		ABLOG(Warning, TEXT("OnNextAttackCheck"));
		CanNextCombo = false;

		if (true == IsComboInputOn)
		{
			AttackStartComboState();
			ABAnim->JumpToAttackMontageSection(CurrentCombo);
		}
	});

	// 충돌 영역 탐색 방법 설정
	// this => 무시할 엑터 목록에 넣기
	ABAnim->OnAttackHitCheck.AddUObject(this, &AABCharacter::AttackCheck);

	// OnHPIsZero 발동(?)시, 실행될 로직
	//CharacterStat->OnHPIsZero.AddLambda([this]() -> void
	//{
	//	ABLOG(Warning, TEXT("OnHPisZero"));
	//	ABAnim->SetDeadAnim();
	//	// 죽은 후에는 엑터의 충돌 설정을 false로 두어서 더이상 충돌 이벤트가 발생하지 않도록 설정
	//	SetActorEnableCollision(false);
	//});

}

float AABCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	ABLOG(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);

	CharacterStat->SetDamage(FinalDamage);
	// 현재 상태가 DEAD + 가해자가 플레이어 컨트롤러 이다
	if (CurrentState == ECharacterState::DEAD)
	{
		if (EventInstigator->IsPlayerController())
		{
			auto IntstigatorController = Cast<AABPlayerController>(EventInstigator);
			ABCHECK(nullptr != IntstigatorController, 0.f);
			// 죽은 NPC의 정보를 플레이어 컨트롤러를 통해 플레이어 스테이트에 전달
			IntstigatorController->NPCKill(this);
		}
	}
	return FinalDamage;
}

// 플레이어 컨트롤러에 폰이 빙의되는 시점에 호출
//void AABCharacter::PossessedBy(AController* NewController)
//{
//	Super::PossessedBy(NewController);
//
//	// 폰이 플레이어인지, NPC인지에 따라 컨트롤 모드와 속도를 다르게 설정. 
//	if (true == IsPlayerControlled())
//	{
//		SetControlMode(EControlMode::DIABLO);
//		GetCharacterMovement()->MaxWalkSpeed = 600.f;
//	}
//	else
//	{
//		SetControlMode(EControlMode::NPC);
//		GetCharacterMovement()->MaxWalkSpeed = 300.f;
//	}
//}

// Called to bind functionality to input
void AABCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 액션 매핑 입력 설정과 연동
	PlayerInputComponent->BindAction(TEXT("ViewChange"), EInputEvent::IE_Pressed, this, &AABCharacter::ViewChange);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AABCharacter::Attack);

	// 축 매핑 입력 설정과 연동
	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AABCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AABCharacter::LeftRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AABCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AABCharacter::Turn);

}

bool AABCharacter::CanSetWeapon()
{
	return true;
}

void AABCharacter::SetWeapon(AABWeapon* NewWeapon)
{
	ABCHECK(nullptr != NewWeapon);

	// 이미 무기가 장착되어 있다면, 캐릭터로부터 무기를 떼어내고 새 무기 장착
	if (nullptr != CurrentWeapon)
	{
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}

	FName WeaponSocket(TEXT("hand_rSocket"));
	if (nullptr != NewWeapon)
	{
		// 아이템 습득 시, 습득한 아이템을 캐릭터의 오른쪽 손 소켓에 부착한 뒤 엑터의 소유주를 변경
		NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
		NewWeapon->SetOwner(this);
		CurrentWeapon = NewWeapon;
	}
}

void AABCharacter::UpDown(float NewAxisValue)
{
	// 시선 방향 벡터 가져오기
	//AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X), NewAxisValue);

	// FRotationMatrix : 회전된 좌표계 정보를 저장하는 행렬
	switch (CurrentControlMode)
	{
	case AABCharacter::EControlMode::GTA:
		AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::X), NewAxisValue);
		break;
	case AABCharacter::EControlMode::DIABLO:
		DirectionToMove.X = NewAxisValue;
		break;
	default:
		break;
	}
}

void AABCharacter::LeftRight(float NewAxisValue)
{
	// 우측 방향 벡터 가져오기
	//AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y), NewAxisValue);

	switch (CurrentControlMode)
	{
	case AABCharacter::EControlMode::GTA:
		AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::Y), NewAxisValue);
		break;
	case AABCharacter::EControlMode::DIABLO:
		DirectionToMove.Y = NewAxisValue;
		break;
	default:
		break;
	}
}

void AABCharacter::LookUp(float NewAxisValue)
{
	// AddController ~ Input: 마우스 입력 신호값을 플레이어 컨트롤러의 컨트롤 회전 값으로 변환하도록 명령
	//AddControllerPitchInput(NewAxisValue);
	switch (CurrentControlMode)
	{
	case AABCharacter::EControlMode::GTA:
		// Y축 회전
		AddControllerPitchInput(NewAxisValue);
		break;
	case AABCharacter::EControlMode::DIABLO:
		break;
	default:
		break;
	}
}

void AABCharacter::Turn(float NewAxisValue)
{
	//AddControllerYawInput(NewAxisValue);
	switch (CurrentControlMode)
	{
	case AABCharacter::EControlMode::GTA:
		// Z축 회전
		AddControllerYawInput(NewAxisValue);
		break;
	case AABCharacter::EControlMode::DIABLO:
		break;
	default:
		break;
	}
}

void AABCharacter::ViewChange()
{
	switch (CurrentControlMode)
	{
	case AABCharacter::EControlMode::GTA:
		// GTA 모드였다면 컨트롤 회전이 스프링 암에 맞춰져 있을 것.
		// Diablo는 캐릭터의 회전이 컨트롤 회전을 결정하기 때문에 시점 변경 전에 캐릭터의 현재 회전 값을 컨트롤 회전으로 세팅해둬야 함.
		// 그렇지 않으면 컨트롤 회전 시 캐릭터도 회전하게 됨. 
		GetController()->SetControlRotation(GetActorRotation());
		SetControlMode(EControlMode::DIABLO);
		break;
	case AABCharacter::EControlMode::DIABLO:
		// Diablo 모드였다면 컨트롤 회전이 캐릭터 회전에 맞춰져 있을 것.
		// 따라서 현재 스프링 암의 회전값을 컨트롤 회전으로 세팅해둔 상태에서 시점 변경.
		GetController()->SetControlRotation(SpringArm->GetRelativeRotation());
		SetControlMode(EControlMode::GTA);
		break;
	default:
		break;
	}
}

void AABCharacter::Attack()
{
	if (true == IsAttacking)
	{
		// IsWithinInclusive: 값이 범위 내에 있는지 체크 (최소 ~ 최대 범위)
		ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 1, MaxCombo));
		if (true == CanNextCombo)
		{
			IsComboInputOn = true;
		}
	}
	else
	{
		ABCHECK(CurrentCombo == 0);
		AttackStartComboState();
		ABAnim->PlayAttackMontage();
		ABAnim->JumpToAttackMontageSection(CurrentCombo);
		IsAttacking = true;
	}

	//if (nullptr == ABAnim)	return;

	//// 몽타주 에셋을 이용해 공격 명령
	//ABAnim->PlayAttackMontage();
	//IsAttacking = true;
}

void AABCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 이미 공격 중이라면 에러 로그 발생
	ABCHECK(IsAttacking);
	ABCHECK(CurrentCombo > 0)
	IsAttacking = false;
	AttackEndComboState();
	
	OnAttackEnd.Broadcast();
}

void AABCharacter::AttackStartComboState()
{
	CanNextCombo = true;
	IsComboInputOn = false;
	ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1));
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
}

void AABCharacter::AttackEndComboState()
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;

}

void AABCharacter::AttackCheck()
{
	float FinalAttackRange = GetFinalAttackRange();

	// 충돌한 엑터 관련 정보를 구조체로 넘겨줘서 탐색
	FHitResult HitResult;

	// 1. 탐지에 사용할 도형 제작
	// 2. 도형의 탐색 영역 지정
	// 3. 탐색 방법 설정

	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,
		GetActorLocation(),														// 탐색 시작 위치
		GetActorLocation() + GetActorForwardVector() * FinalAttackRange,		// 탐색 끝낼 위치
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(AttackRadius),				// 50cm 반지름의 구 생성
		Params);	

	// 콜리전 디버그 드로잉
#if ENABLE_DRAW_DEBUG

	// 캡슐 콜리전을 시선 방향으로 눕히기 = 캡슐 상단을 향햐는 z벡터가 시선 방향과 일치한다는 것을 의미.
	FVector TraceVec = GetActorForwardVector() * FinalAttackRange;		// 시선 방향
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = FinalAttackRange * 0.5f + AttackRadius;
	// MakeFromZ(시선 방향) => 시선방향으로 눕히기 위해 필요한 회전 좌표축을 생성
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResult ? FColor::Red : FColor::Green;
	float DebugLifeTime = 4.0f;

	//DrawDebugCapsule(GetWorld(),
	//	Center,
	//	HalfHeight,
	//	AttackRadius,
	//	CapsuleRot,
	//	DrawColor,
	//	false,
	//	DebugLifeTime);

	//UE_VLOG_LOCATION(this, ArenaBattle1, Verbose, GetActorLocation(), 50.f, FColor::Blue, TEXT("Attack Position"));
	//UE_VLOG_CAPSULE(this, ArenaBattle1, Verbose, GetActorLocation() = GetActorForwardVector() * AttackRadius, HalfHeight, AttackRadius, CapsuleRot, DrawColor, TEXT("Attack Area"));

#endif

	if (true == bResult)
	{
		if (nullptr != HitResult.GetActor())
		{
			ABLOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.GetActor()->GetName());

			// TakeDamage(데미지 세기, 데미지 종류, 공격 가해자, 데미지 전달 도구) : 엑터에게 데미지를 전달하는 함수
			FDamageEvent DamageEvent;
			HitResult.GetActor()->TakeDamage(GetFinalAttackDamage(), DamageEvent, GetController(), this);
		}
	}
}

void AABCharacter::OnAssetLoadCompleted()
{
	AssetStreamingHandle->ReleaseHandle();
	// 에셋 로드 시점 제어
	TSoftObjectPtr<USkeletalMesh> LoadedAssetPath(CharacterAssetToLoad);
	ABCHECK(LoadedAssetPath.IsValid());

	GetMesh()->SetSkeletalMesh(LoadedAssetPath.Get());
	SetCharacterState(ECharacterState::READY);
}

