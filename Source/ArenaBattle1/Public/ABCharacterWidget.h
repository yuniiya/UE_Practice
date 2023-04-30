// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle1.h"
#include "Blueprint/UserWidget.h"
#include "ABCharacterWidget.generated.h"


// ABCharacterStatComponent �� ������ ĳ������ ������ ��ȭ�� ������ ���α׷������� ������ ������Ʈ�ϴ� Ŭ����.
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
	// TWeakObjectPtr: �𸮾��� weakptr. ���� �������� UI�� ĳ���ʹ� ���縦 �����ϱ� ������ ��ǻ� weakptr�� ����� �ʿ������, ���� �� ���Ͱ� ������ ���Ͷ�� weakptr�� ����ϴ� ���� �ٶ�����.
	TWeakObjectPtr<class UABCharacterStatComponent> CurrentCharacterStat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HPBar, Meta = (AllowPrivateAccess = true))
	class UProgressBar* HPProgressBar;
};
