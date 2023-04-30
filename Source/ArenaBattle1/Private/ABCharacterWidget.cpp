// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterWidget.h"
#include "ABCharacterStatComponent.h"
#include "Components//ProgressBar.h"

void UABCharacterWidget::BindCharacterStat(UABCharacterStatComponent* NewCharacterStat)
{
	ABCHECK(nullptr != NewCharacterStat);

	CurrentCharacterStat = NewCharacterStat;
	// AddUObject : ��� �Լ��� ���ε�
	NewCharacterStat->OnHPChanged.AddUObject(this, &UABCharacterWidget::UpdateHPWidget);
}

// ������ BeginPlay() �� ����� �뵵.
void UABCharacterWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HPProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_HPBar")));
	ABCHECK(nullptr != HPProgressBar);
	UpdateHPWidget();
}

void UABCharacterWidget::UpdateHPWidget()
{
	if (nullptr != HPProgressBar)
	{
		HPProgressBar->SetPercent(CurrentCharacterStat->GetHPRatio());
		ABLOG(Warning, TEXT("HPRatio : %f"), CurrentCharacterStat->GetHPRatio());
	}
}
