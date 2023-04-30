// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle1.h"
#include "Animation/AnimInstance.h"
#include "ABAnimInstance.generated.h"

// ��������Ʈ ����� ����ϸ� �ִ� �ν��Ͻ��� �ڽ��� ��������Ʈ�� ����ϴ� ��ü�� �������� ���� ��������Ʈ�� ����� �Լ��� ȣ���ϸ� ��.
// ��, �ٸ� Ŭ������ �������� ���� ���谡 ����

// ���� ���� �Լ��� ��ϵǵ��� ��Ƽĳ��Ʈ�� ����
DECLARE_MULTICAST_DELEGATE(FOnNextAttackCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackHitCheckDelegate);

// �ִϸ��̼� ��� ����Ʈ = �ִ� �׷��� + �ִ� �ν��Ͻ�
// �ִ� �ν��Ͻ� : ���̷�Ż �޽ø� �����ϴ� ���� ������ �޾� �ִ� �׷����� ������ �����͸� ����
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
	// �𸮾� ��Ÿ���� ������ �� �ֵ��� ��ũ�� ����
	// ��Ÿ�� �ִϸ��̼��� ����ϸ� ��� ������ ��ġ�� ��Ƽ���̸� ȣ���ϰ�, �𸮾� ������ �ִ� �ν��Ͻ��� �ִ� AnimNotify_�Լ��� ã�Ƽ� ȣ��
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

	// ��Ÿ�� ������ ������ ����
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsDead;
	
	
};
