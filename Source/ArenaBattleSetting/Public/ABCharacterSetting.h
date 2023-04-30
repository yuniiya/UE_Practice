// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ABCharacterSetting.generated.h"

// ĳ���� ���� ����� �����ϴ� Ŭ����.
// �𸮾� ������ �𸮾� ������Ʈ�� �⺻���� �����ϰ� �����ϵ��� �ܺ� INI ���Ͽ��� �⺻ �Ӽ� ���� �����ϴ� ����� ����.
// �𸮾� ������Ʈ�� �⺻���� INI ���Ͽ��� �ҷ����̱� ���� ����� �� �� Ŭ�������� ����. 

// config = �ҷ����� INI ���� �̸� -> �𸮾� ������ �ʱ�ȭ �ܰ迡�� Config ������ ������ INI ������ �о�鿩�� CharacterAssets ���� ����. 
UCLASS(config=ArenaBattle)
class ARENABATTLESETTING_API UABCharacterSetting : public UObject
{
	GENERATED_BODY()

public:
	UABCharacterSetting();

	// �ҷ����� �Ӽ��� config Ű���带 �����ϸ�, �𸮾� ������ ������Ʈ�� �ʱ�ȭ�� �� �ش� �Ӽ��� ���� INI ���Ͽ��� �о ����.
	UPROPERTY(config)
	TArray<FSoftObjectPath> CharacterAssets;

};
