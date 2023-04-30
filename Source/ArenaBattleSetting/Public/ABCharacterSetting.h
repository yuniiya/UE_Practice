// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ABCharacterSetting.generated.h"

// 캐릭터 에셋 목록을 보관하는 클래스.
// 언리얼 엔진은 언리얼 오브젝트의 기본값을 유연하게 관리하도록 외부 INI 파일에서 기본 속성 값을 지정하는 기능을 제공.
// 언리얼 오브젝트가 기본값을 INI 파일에서 불러들이기 위한 기능을 이 곳 클래스에서 구현. 

// config = 불러들일 INI 파일 이름 -> 언리얼 엔진은 초기화 단계에서 Config 폴더에 위차한 INI 파일을 읽어들여와 CharacterAssets 값을 설정. 
UCLASS(config=ArenaBattle)
class ARENABATTLESETTING_API UABCharacterSetting : public UObject
{
	GENERATED_BODY()

public:
	UABCharacterSetting();

	// 불러들일 속성에 config 키워드를 선언하면, 언리얼 엔진이 오브젝트를 초기화할 때 해당 속성의 값을 INI 파일에서 읽어서 설정.
	UPROPERTY(config)
	TArray<FSoftObjectPath> CharacterAssets;

};
