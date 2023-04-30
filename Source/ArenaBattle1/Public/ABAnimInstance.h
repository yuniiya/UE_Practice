// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle1.h"
#include "Animation/AnimInstance.h"
#include "ABAnimInstance.generated.h"

// 델리게이트 기능을 사용하면 애님 인스턴스는 자신의 델리게이트를 사용하는 객체가 누구인지 몰라도 델리게이트에 연결된 함수만 호출하면 됨.
// 즉, 다른 클래스에 의존하지 않은 설계가 가능

// 여러 개의 함수가 등록되도록 멀티캐스트로 선언
DECLARE_MULTICAST_DELEGATE(FOnNextAttackCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackHitCheckDelegate);

// 애니메이션 블루 프린트 = 애님 그래프 + 애님 인스턴스
// 애님 인스턴스 : 스켈레탈 메시를 소유하는 폰의 정보를 받아 애님 그래프가 참조할 데이터를 제공
UCLASS()
class ARENABATTLE1_API UABAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UABAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void PlayAttackMontage();
	void JumpToAttackMontageSection(int32 NewSection);

public:
	FOnNextAttackCheckDelegate OnNextAttackCheck;
	FOnAttackHitCheckDelegate OnAttackHitCheck;
	void SetDeadAnim() { IsDead = true; }

private:
	// 언리얼 런타임이 감지할 수 있도록 매크로 지정
	// 몽타주 애니메이션을 재생하면 재생 구간에 위치한 노티파이를 호출하고, 언리얼 엔진은 애님 인스턴스에 있는 AnimNotify_함수를 찾아서 호출
	UFUNCTION()
	void AnimNotify_AttackHitCheck();

	UFUNCTION()
	void AnimNotify_NextAttackCheck();

	FName GetAttackMontageSectionName(int32 Section);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float CurrentPawnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsInAir;

	// 몽타주 에셋을 저장할 변수
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsDead;
	
	
};
