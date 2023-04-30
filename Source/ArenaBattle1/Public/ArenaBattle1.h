// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"

// 캐릭터 스테이트. 블루프린트에서도 사용하기 위한 매크로 선언
UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	PREINIT,
	LOADING,
	READY,
	DEAD
};

// 로그 카테고리 생성
DECLARE_LOG_CATEGORY_EXTERN(ArenaBattle1, Log, All);

// 로그 커스텀 매크로
// 코드가 들어있는 파일 이름, 함수, 라인 정보를 로그로 남김.
#define ABLOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define ABLOG_S(Verbosity) UE_LOG(ArenaBattle1, Verbosity, TEXT("%s"), *ABLOG_CALLINFO)
#define ABLOG(Verbosity, Format, ...) UE_LOG(ArenaBattle1, Verbosity, TEXT("%s %s"), *ABLOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

// 매크로 안의 조건이 아닐 경우, 에러 로그 발생
#define ABCHECK(Expr, ...) {if(!(Expr)) { ABLOG(Error, TEXT("ASSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__; }}
