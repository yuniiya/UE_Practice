// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacter.h"
#include "ABAnimInstance.h"
#include "ABWeapon.h"
#include "ABCharacterStatComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/WidgetComponent.h"		// ���� ������Ʈ�� ����� �� �ֵ��� �߰� 
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
	GetCharacterMovement()->JumpZVelocity = 800.0f;		// ���� ���� ����
	GetCharacterMovement()->GravityScale = 2.f;

	IsAttacking = false;

	MaxCombo = 4;
	AttackEndComboState();

	// ĳ������ ĸ�� ������Ʈ�� ABCharacter �������� ����ϵ��� ����
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("ABCharacter"));

	AttackRange = 250.0f;
	AttackRadius = 50.0f;

	HPBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 180.f));
	// EWidgetSpace::Screen : �׻� �÷��̾ ���� ������
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);		
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_HUD(TEXT("/Game/Book/UI/UI_HPBar.UI_HPBar_C"));
	if (UI_HUD.Succeeded())
	{
		HPBarWidget->SetWidgetClass(UI_HUD.Class);
		HPBarWidget->SetDrawSize(FVector2D(150.f, 50.f));
	}

	// ������ ��ġ�ϰų� ���Ӱ� �����Ǵ� ĳ���͸��� AIController ���͸� ������, �÷��̾� ���� ������ ��� ���� AIController�� ���踦 ����.
	AIControllerClass = AABAIController::StaticClass();						// AIController Ŭ���� �Ӽ� ����
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;					// AI ���� �ɼ� 

	// �÷��̾ ����� ����
	AssetIndex = 4;

	// PREINIT ������Ʈ(ĳ���� ���� ��)������ ĳ���Ϳ� UI�� ����.
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
		// �Է� ��Ȱ��ȭ
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


		// �𸮾� 4.21�������� ������ �ʱ�ȭ ������ PostInitalizeComponents -> BeginPlay �� ����.
		// ĳ������ ���� ������Ʈ�� UI ������ ����
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
			// �����̺�� Ʈ�� ���� ����
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
			// �Է� ó�� �ߴ�
			DisableInput(ABPlayerController);
		}
		else
		{
			// �����̺�� Ʈ�� �ߴ�
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

	// GetDefault() : �޸𸮿� �ö� Ŭ���� �⺻ ��ü�� �������� �Լ� 
	// Ŭ���� �⺻ ��ü : ����� �ε��Ǹ鼭 �ڽſ��� ���� ��� �𸮾� ������Ʈ�� �⺻���� �����ϰ� �����ϴµ�, �̸� Ŭ���� �⺻��ü��� ��.)

	auto DefaultSetting = GetDefault<UABCharacterSetting>();

	if (true == bIsPlayer)
	{
		AssetIndex = 4;
	}
	else
	{
		// NPC�� ������ ��, ���� ��� �� �������� ����
		AssetIndex = FMath::RandRange(0, DefaultSetting->CharacterAssets.Num() - 1);
	}

	// RequestAsyncLoad(): �񵿱� ������� ������ �ε��ϴ� ���. FStreamableDelegate ������ ��������Ʈ�� �Ѱ��� ���, ������ �ε��� �Ϸ��ϸ� ��������Ʈ�� ����� �Լ��� ȣ������.
	// CreateUObject(): ��������Ʈ ����
	CharacterAssetToLoad = DefaultSetting->CharacterAssets[AssetIndex];
	auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
	ABCHECK(nullptr != ABGameInstance);
	AssetStreamingHandle = ABGameInstance->StreamableManager.RequestAsyncLoad(CharacterAssetToLoad, FStreamableDelegate::CreateUObject(this, &AABCharacter::OnAssetLoadCompleted));
	SetCharacterState(ECharacterState::LOADING);

	//FName WeaponSocket(TEXT("hand_rSocket"));
	////SpawnActor<������ ���� Ŭ����>(��ġ, ȸ�� ��) : ���忡 ���ο� ���͸� �����ϴ� ���
	//auto CurWeapon = GetWorld()->SpawnActor<AABWeapon>(FVector::ZeroVector, FRotator::ZeroRotator);
	//if (nullptr != CurWeapon)
	//{
	//	// hand_rSocket ���� ��ġ�� �������� ������ Ʈ�������� ����
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

		// ĳ���Ͱ� �����̴� �������� ĳ���͸� ȸ�������ִ� ������Ʈ
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);	// ȸ�� �ӵ�
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
		// NPC�� �̵� ���⿡ ���� ȸ���ϵ��� ĳ���� �����Ʈ�� ����. 
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

	// FInterpTo: ������ �ӷ����� ��ǥ �������� ������ ����
	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, ArmLengthTo, DeltaTime, ArmLengthSpeed);

	switch (CurrentControlMode)
	{
	case AABCharacter::EControlMode::GTA: 
		// FRotator ������ ȸ�� ��� ���� ��, �̸� ���� ��ȯ�� ��ǥ���� X, Y�� ������ ������
		break;
	case AABCharacter::EControlMode::DIABLO:
	{
		if (DirectionToMove.SizeSquared() > 0.0f)
		{
			// MakeFromX : �ϳ��� ���ͷκ��� ȸ�� ����� ����� ���
			// �ϳ��� ���� ���� �̿� �����ϴ� ������ �� ���� ���� ȸ�� ��� ����
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

	// OnMontageEnded: �ִϸ��̼� ��Ÿ�� ����� ������ �ߵ��ϴ� ��������Ʈ
	// ĳ������ OnMontageEnded ��� �Լ��� ���⿡ ����� ��Ÿ�� ����� ������ Ÿ�̹��� �ִ� �ν��Ͻ� �ʿ��� �ľ��� �� �ֵ��� ��. 
	ABAnim->OnMontageEnded.AddDynamic(this, &AABCharacter::OnAttackMontageEnded);

	// [������ ȯ��] (����) -> ��ȯŸ�� {�Լ� ����}
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

	// �浹 ���� Ž�� ��� ����
	// this => ������ ���� ��Ͽ� �ֱ�
	ABAnim->OnAttackHitCheck.AddUObject(this, &AABCharacter::AttackCheck);

	// OnHPIsZero �ߵ�(?)��, ����� ����
	//CharacterStat->OnHPIsZero.AddLambda([this]() -> void
	//{
	//	ABLOG(Warning, TEXT("OnHPisZero"));
	//	ABAnim->SetDeadAnim();
	//	// ���� �Ŀ��� ������ �浹 ������ false�� �ξ ���̻� �浹 �̺�Ʈ�� �߻����� �ʵ��� ����
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

// �÷��̾� ��Ʈ�ѷ��� ���� ���ǵǴ� ������ ȣ��
//void AABCharacter::PossessedBy(AController* NewController)
//{
//	Super::PossessedBy(NewController);
//
//	// ���� �÷��̾�����, NPC������ ���� ��Ʈ�� ���� �ӵ��� �ٸ��� ����. 
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

	// �׼� ���� �Է� ������ ����
	PlayerInputComponent->BindAction(TEXT("ViewChange"), EInputEvent::IE_Pressed, this, &AABCharacter::ViewChange);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AABCharacter::Attack);

	// �� ���� �Է� ������ ����
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
		// ������ ���� ��, ������ �������� ĳ������ ������ �� ���Ͽ� ������ �� ������ �����ָ� ����
		NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
		NewWeapon->SetOwner(this);
		CurrentWeapon = NewWeapon;
	}
}

void AABCharacter::UpDown(float NewAxisValue)
{
	// �ü� ���� ���� ��������
	//AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X), NewAxisValue);

	// FRotationMatrix : ȸ���� ��ǥ�� ������ �����ϴ� ���
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
	// ���� ���� ���� ��������
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
	// ���콺 �Է� ��ȣ���� �÷��̾� ��Ʈ�ѷ��� ��Ʈ
	// �� ȸ�� ������ ��ȯ�ϵ��� ���
	// Y�� ȸ��
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
	// Z�� ȸ��
	//AddControllerYawInput(NewAxisValue);
	switch (CurrentControlMode)
	{
	case AABCharacter::EControlMode::GTA:
		AddControllerYawInput(NewAxisValue);
		break;
	case AABCharacter::EControlMode::DIABLO:
		break;
	//	���¾� ������ �𸮾��� ���� �̵�� �� �� �Ʒ�����
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
		// IsWithinInclusive: ���� ���� ���� �ִ��� üũ (�ּ� ~ �ִ� ����)
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

	//// ��Ÿ�� ������ �̿��� ���� ���
	//ABAnim->PlayAttackMontage();
	//IsAttacking = true;
}

void AABCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// �̹� ���� ���̶�� ���� �α� �߻�
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
	// �浹�� ���� ���� ������ ����ü�� �Ѱ��༭ Ž��
	FHitResult HitResult;

	// 1. Ž���� ����� ���� ����
	// 2. ������ Ž�� ���� ����
	// 3. Ž�� ��� ����

	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,
		GetActorLocation(),										// Ž�� ���� ��ġ
		GetActorLocation() + AttackRange,						// Ž�� ���� ��ġ
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(AttackRadius),				// 50cm �������� �� ����
		Params);	

	// �ݸ��� ����� ��θ�
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

			// TakeDamage(������ ����, ������ ����, ���� ������, ������ ���� ����)
			FDamageEvent DamageEvent;
			HitResult.GetActor()->TakeDamage(CharacterStat->GetAttack(), DamageEvent, GetController(), this);
		}
	}
}

void AABCharacter::OnAssetLoadCompleted()
{
	AssetStreamingHandle->ReleaseHandle();
	// ���� �ε� ���� ����
	TSoftObjectPtr<USkeletalMesh> LoadedAssetPath(CharacterAssetToLoad);
	ABCHECK(LoadedAssetPath.IsValid());

	GetMesh()->SetSkeletalMesh(LoadedAssetPath.Get());
	SetCharacterState(ECharacterState::READY);
}

