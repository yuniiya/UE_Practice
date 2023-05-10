// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle1.h"
#include "Blueprint/UserWidget.h"
#include "ABCharacterSelectWidget.generated.h"

// ���� ������ �ִ� ���̷�Ż �޽� ������ ����� ��������, ��ư�� ���� ������ ���̷�Ż �޽� ������Ʈ�� ������ ���̷�Ż �޽ø� �����ϴ� ����� �ϴ� Ŭ����
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
