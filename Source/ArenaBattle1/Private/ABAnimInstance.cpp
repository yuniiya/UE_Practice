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

// Tick마다 호출되는 함수
// 게임 엔진은 틱 마다 입력 시스템 -> 게임 로직 -> 애니메이션 시스템 순으로 로직을 실행
void UABAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	// 폰의 속도 정보를 가져온 후, 이를 CurrentPawnSpeed에 업데이트해 폰의 속도에 따라 다른 애니메이션을 재생
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	auto Pawn = TryGetPawnOwner();
	// 폰 객체 유효성 검사
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
			// 폰이 현재 점프 중인지 정보를 애님 인스턴스의 멤버 변수로 보관
			IsInAir = Character->GetMovementComponent()->IsFalling();
		}
	}

}

void UABAnimInstance::PlayAttackMontage()
{
	// 델리게이트에 의해 공격의 시작과 종료가 감지되므로 Montage_IsPlaying 함수는 더이상 필요 x
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
	// Broadcast() : 멀티캐스트 델리게이트에 등록된 모든 함수를 호출하는 명령
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
