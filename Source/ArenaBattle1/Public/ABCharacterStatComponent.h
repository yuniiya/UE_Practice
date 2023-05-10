// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle1.h"
#include "Components/ActorComponent.h"
#include "ABCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHPIsZeroDelegate);
DECLARE_MULTICAST_DELEGATE(FOnHPChangeDelegate);		// ABCharacterWidget 클래스와 상호의존성을 가지지 않게 델리게이트 선언. HP의 값이 변할 때마다 UI 위젯 값이 자동으로 변하게 구성

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENABATTLE1_API UABCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UABCharacterStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	// 엑터의 PostInitializeComponents 에 대응하는 컴포넌트의 함수. PostInitializeComponents 함수가 호출되기 바로 전에 호출됨. 
	virtual void InitializeComponent() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void SetNewLevel(int32 NewLevel);
	void SetDamage(float NewDamage);
	void SetHP(float NewHP);
	float GetAttack();
	float GetHPRatio();
	int32 GetDropExp() const;

	FOnHPIsZeroDelegate OnHPIsZero;
	FOnHPChangeDelegate OnHPChanged;

private:
	struct FABCharacterData* CurrentStatData = nullptr;

	UPROPERTY(EditInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = true))
	int32 Level;

	// Transient: 휘발성. CurrnetHP 값은 게임을 시작할 때마다 변경되므로 보관하는 의미가 없기 때문에 직렬화에서 제외.
	// VisibleInstanceOnly: 프로퍼티 창에서 편집 불가
	// Meta = (AllowPrivateAccess = true: 캡슐화를 지키면서 동시에 에디티에서 편집 가능.
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = true))
	float CurrentHP;	
};
