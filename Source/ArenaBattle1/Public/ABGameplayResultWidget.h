// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle1.h"
#include "ABGameplayWidget.h"
#include "ABGameplayResultWidget.generated.h"

// 결과 화면 UI를 담당할 클래스
/**
 * 
 */
UCLASS()
class ARENABATTLE1_API UABGameplayResultWidget : public UABGameplayWidget
{
	GENERATED_BODY()
	
public:
	void BindGameState(class AABGameState* GameState);

protected:
	virtual void NativeConstruct() override;

private:
	TWeakObjectPtr<class AABGameState> CurrentGameState;
};
