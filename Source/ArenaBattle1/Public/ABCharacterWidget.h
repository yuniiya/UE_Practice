// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle1.h"
#include "Blueprint/UserWidget.h"
#include "ABCharacterWidget.generated.h"


// ABCharacterStatComponent 와 연동해 캐릭터의 스탯이 변화할 때마다 프로그래스바의 내용을 업데이트하는 클래스.
/**
 * 
 */
UCLASS()
class ARENABATTLE1_API UABCharacterWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void BindCharacterStat(class UABCharacterStatComponent* NewCharacterStat);

protected:
	virtual void NativeConstruct() override;
	void UpdateHPWidget();

private:
	// TWeakObjectPtr: 언리얼의 weakptr. 현재 구조에서 UI와 캐릭터는 생사를 같이하기 때문에 사실상 weakptr의 사용은 필요없으나, 만약 두 엑터가 별개의 엑터라면 weakptr을 사용하는 편이 바람직함.
	TWeakObjectPtr<class UABCharacterStatComponent> CurrentCharacterStat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HPBar, Meta = (AllowPrivateAccess = true))
	class UProgressBar* HPProgressBar;
};
