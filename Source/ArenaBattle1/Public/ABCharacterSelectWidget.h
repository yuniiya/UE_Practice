// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle1.h"
#include "Blueprint/UserWidget.h"
#include "ABCharacterSelectWidget.generated.h"

// 현재 레벨에 있는 스켈레탈 메시 엑터의 목록을 가져오고, 버튼을 누를 때마다 스켈레탈 메시 컴포넌트에 지정한 스켈레탈 메시를 변경하는 기능을 하는 클래스
/**
 * 
 */
UCLASS()
class ARENABATTLE1_API UABCharacterSelectWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable)
	void NextCharacter(bool bForward = true);

	virtual void NativeConstruct() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character)
	int32 CurrentIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character)
	int32 MaxIndex;

	UPROPERTY()
	class UButton* PrevButton;

	UPROPERTY()
	class UButton* NextButton;

	UPROPERTY()
	class UEditableTextBox* TextBox;

	UPROPERTY()
	class UButton* ConfirmButton;

	TWeakObjectPtr<USkeletalMeshComponent> TargetComponent;

private:
	UFUNCTION()
	void OnPrevClicked();

	UFUNCTION()
	void OnNextClicked();

	UFUNCTION()
	void OnConfirmClicked();
	
};
