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
		// 입력 비활성화
		if (true == bIsPlayer)
		{
			DisableInput(ABPlayerController);
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
				ABPlayerController->RestartLevel();
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
	// 클래스 기본 객체 : 모듈은 로딩되면서 자신에게 속한 모든 언리얼 오브젝트의 기본값을 지정하고 생성하는데, 이를 클래스 기본객체라고 함.)

	auto DefaultSetting = GetDefault<UABCharacterSetting>();

	if (true == bIsPlayer)
	{
		AssetIndex = 4;
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
		//SpringArm->TargetArmLength = 450.f;
		//SpringArm->SetRelativeRotation(FRotator::ZeroRotator);
		ArmLengthTo = 450.0f;
		SpringArm->bUsePawnControlRotation = true;
		SpringArm->bInheritPitch = true;
		SpringArm->bInheritRoll = true;
		SpringArm->bInheritYaw = true;
		SpringArm->bDoCollisionTest = true;
		bUseControllerRotationYaw = false;

		// 캐릭터가 움직이는 방향으로 캐릭터를 회전시켜주는 컴포넌트
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);	// 회전 속도
	}
		break;
	case AABCharacter::EControlMode::DIABLO:
	{
		//SpringArm->TargetArmLength = 800.f;
		//SpringArm->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
		ArmLengthTo = 800.0f;
		ArmRotationTo = FRotator(-45.0f, 0.0f, 0.0f);
		SpringArm->bUsePawnControlRotation = false;
		SpringArm->bInheritPitch = false;
		SpringArm->bInheritRoll = false;
		SpringArm->bInheritYaw = false;
		SpringArm->bDoCollisionTest = false;
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = false;
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
			// MakeFromX : 하나의 벡터로부터 회전 행렬을 만드는 명령
			// 하나이 벡터 값과 이에 직교하는 나머지 두 축을 구해 회전 행렬 생성
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

		if (IsComboInputOn)
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
	return nullptr == CurrentWeapon;
}

void AABCharacter::SetWeapon(AABWeapon* NewWeapon)
{
	ABCHECK(nullptr != NewWeapon && nullptr == CurrentWeapon);
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
	// 마우스 입력 신호값을 플레이어 컨트롤러의 컨트
	// 롤 회전 값으로 변환하도록 명령
	// Y축 회전
	//AddControllerPitchInput(NewAxisValue);
	switch (CurrentControlMode)
	{
	case AABCharacter::EControlMode::GTA:
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
	// Z축 회전
	//AddControllerYawInput(NewAxisValue);
	switch (CurrentControlMode)
	{
	case AABCharacter::EControlMode::GTA:
		AddControllerYawInput(NewAxisValue);
		break;
	case AABCharacter::EControlMode::DIABLO:
		break;
	//	나는야 이윤서 언리얼의 제왕 이득우 ㅋ 내 아래지모
	default:
		break;
	}
}

void AABCharacter::ViewChange()
{
	switch (CurrentControlMode)
	{
	case AABCharacter::EControlMode::GTA:
		GetController()->SetControlRotation(GetActorRotation());
		SetControlMode(EControlMode::DIABLO);
		break;
	case AABCharacter::EControlMode::DIABLO:
		GetController()->SetControlRotation(SpringArm->GetRelativeRotation());
		SetControlMode(EControlMode::GTA);
		break;
	default:
		break;
	}
}

void AABCharacter::Attack()
{
	if (IsAttacking)
	{
		// IsWithinInclusive: 값이 범위 내에 있는지 체크 (최소 ~ 최대 범위)
		ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 1, MaxCombo));
		if (CanNextCombo)
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
	// 충돌한 엑터 관련 정보를 구조체로 넘겨줘서 탐색
	FHitResult HitResult;

	// 1. 탐지에 사용할 도형 제작
	// 2. 도형의 탐색 영역 지정
	// 3. 탐색 방법 설정

	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,
		GetActorLocation(),										// 탐색 시작 위치
		GetActorLocation() + AttackRange,						// 탐색 끝낼 위치
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(AttackRadius),				// 50cm 반지름의 구 생성
		Params);	

	// 콜리전 디버그 드로릴
#if ENABLE_DRAW_DEBUG

	FVector TraceVec = GetActorForwardVector() * AttackRange;
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = AttackRange * 0.5f + AttackRadius;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResult ? FColor::Red : FColor::Green;
	float DebugLifeTime = 5.0f;

	DrawDebugCapsule(GetWorld(),
		Center,
		HalfHeight,
		AttackRadius,
		CapsuleRot,
		DrawColor,
		false,
		DebugLifeTime);

#endif

	if (bResult)
	{
		if (nullptr != HitResult.GetActor())
		{
			ABLOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.GetActor()->GetName());

			// TakeDamage(데미지 세기, 데미지 종류, 공격 가해자, 데미지 전달 도구)
			FDamageEvent DamageEvent;
			HitResult.GetActor()->TakeDamage(CharacterStat->GetAttack(), DamageEvent, GetController(), this);
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

