// Fill out your copyright notice in the Description page of Project Settings.


#include "ABAnimInstance.h"

UABAnimInstance::UABAnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	IsInAir = false;
	IsDead = false;
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(TEXT("/Game/Book/Animations/CharacterAttack_Montage.CharacterAttack_Montage"));
	if (ATTACK_MONTAGE.Succeeded())
	{
		AttackMontage = ATTACK_MONTAGE.Object;
	}
}

// Tick���� ȣ��Ǵ� �Լ�
// ���� ������ ƽ ���� �Է� �ý��� -> ���� ���� -> �ִϸ��̼� �ý��� ������ ������ ����
void UABAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	// ���� �ӵ� ������ ������ ��, �̸� CurrentPawnSpeed�� ������Ʈ�� ���� �ӵ��� ���� �ٸ� �ִϸ��̼��� ���
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	auto Pawn = TryGetPawnOwner();
	// �� ��ü ��ȿ�� �˻�
	if (false == ::IsValid(Pawn))
	{
		return;
	}

	if (false == IsDead)
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();
		auto Character = Cast<ACharacter>(Pawn);
		if (Character)
		{
			// ���� ���� ���� ������ ������ �ִ� �ν��Ͻ��� ��� ������ ����
			IsInAir = Character->GetMovementComponent()->IsFalling();
		}
	}

}

void UABAnimInstance::PlayAttackMontage()
{
	// ��������Ʈ�� ���� ������ ���۰� ���ᰡ �����ǹǷ� Montage_IsPlaying �Լ��� ���̻� �ʿ� x
	/*if (!Montage_IsPlaying(AttackMontage))
	{
		Montage_Play(AttackMontage, 1.0f);
	}*/

	ABCHECK(!IsDead);
	Montage_Play(AttackMontage, 1.0f);
}

void UABAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	ABCHECK(!IsDead);
	ABCHECK(Montage_IsPlaying(AttackMontage));
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage);
}

void UABAnimInstance::AnimNotify_AttackHitCheck()
{
	// Broadcast() : ��Ƽĳ��Ʈ ��������Ʈ�� ��ϵ� ��� �Լ��� ȣ���ϴ� ���
	OnAttackHitCheck.Broadcast();
}

void UABAnimInstance::AnimNotify_NextAttackCheck()
{
	OnNextAttackCheck.Broadcast();
}

FName UABAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	ABCHECK(FMath::IsWithinInclusive<int32>(Section, 1, 4), NAME_None);
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}
