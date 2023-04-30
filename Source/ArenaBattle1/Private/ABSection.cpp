// Fill out your copyright notice in the Description page of Project Settings.


#include "ABSection.h"
#include "ABCharacter.h"
#include "ABItemBox.h"

// Sets default values
AABSection::AABSection()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	RootComponent = Mesh;

	FString AssetPath = TEXT("/Game/Book/StaticMesh/SM_SQUARE.SM_SQUARE");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_SQUARE(*AssetPath);
	if (SM_SQUARE.Succeeded())
	{
		Mesh->SetStaticMesh(SM_SQUARE.Object);
	}
	else
	{
		ABLOG(Error, TEXT("Failed to load staticmesh asset : %s"), *AssetPath);
	}

	// 섹션에 플레이어가 입장했는지 판별하는 트리거
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
	Trigger->SetBoxExtent(FVector(775.f, 775.f, 300.f));
	Trigger->SetupAttachment(RootComponent);
	Trigger->SetRelativeLocation(FVector(0.f, 0.f, 250.f));
	Trigger->SetCollisionProfileName(TEXT("ABTrigger"));
	// 바인딩
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AABSection::OnTriggerBeginOverlap);

	FString GateAssetPath = TEXT("/Game/Book/StaticMesh/SM_GATE.SM_GATE");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_GATE(*GateAssetPath);
	if (false == SM_GATE.Succeeded())
	{
		ABLOG(Error, TEXT("Failed to load staticmesh asset : %s"), *GateAssetPath);
	}

	static FName GateSockets[] = { {TEXT("+XGate")}, {TEXT("-XGate")}, {TEXT("+YGate")}, {TEXT("-YGate")} };
	for (FName GateSocket : GateSockets)
	{
		// GateSockets 이름을 가진 소켓이 메시에 있는지 확인 후, 있다면 소켓에 메시 컴포넌트 부착
		ABCHECK(Mesh->DoesSocketExist(GateSocket));
		UStaticMeshComponent* NewGate = CreateDefaultSubobject<UStaticMeshComponent>(*GateSocket.ToString());
		NewGate->SetStaticMesh(SM_GATE.Object);
		NewGate->SetupAttachment(RootComponent, GateSocket);
		NewGate->SetRelativeLocation(FVector(0.f, -80.5f, 0.f));
		GateMeshes.Add(NewGate);

		// 어느 출입문으로 들어갔는지 판별하기 위한 출입문 박스 트리거
		UBoxComponent* NewGateTrigger = CreateDefaultSubobject<UBoxComponent>(*GateSocket.ToString().Append(TEXT("Trigger")));
		NewGateTrigger->SetBoxExtent(FVector(100.f, 100.f, 300.f));
		NewGateTrigger->SetupAttachment(RootComponent, GateSocket);
		NewGateTrigger->SetRelativeLocation(FVector(70.f, 0.f, 250.f));
		NewGateTrigger->SetCollisionProfileName(TEXT("ABTrigger"));
		GateTriggers.Add(NewGateTrigger);

		// 바인딩
		// 박스 컴포넌트가 어느 출입문에 있는 컴포넌트인지 구분할 수 있도록 소켓 이름으로 Tags 를 설정
		NewGateTrigger->OnComponentBeginOverlap.AddDynamic(this, &AABSection::OnGateTriggerBeginOverlap);
		NewGateTrigger->ComponentTags.Add(GateSocket);
	}

	// 게임 처음 시작 시
	bNoBattle = false;

	EnemySpawnTime = 2.f;
	ItemBoxSpawnTime = 5.f;
}

// 에디터에서 선택한 엑터의 속성이나 트랜스폼 정보가 변경될 때, OnConstruction 함수가 실행.
void AABSection::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SetState(bNoBattle ? ESectionState::COMPLETE : ESectionState::READY);
}

// Called when the game starts or when spawned
void AABSection::BeginPlay()
{
	Super::BeginPlay();
	
	SetState(bNoBattle ? ESectionState::COMPLETE : ESectionState::READY);
}

void AABSection::SetState(ESectionState NewState)
{
	switch (NewState)
	{
	case AABSection::ESectionState::READY: 
	{
		Trigger->SetCollisionProfileName(TEXT("ABTrigger"));
		for (UBoxComponent* GateTrigger : GateTriggers)
		{
			GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
		}

		OperateGates(true);
	}
		break;
	case AABSection::ESectionState::BATTLE:
	{
		Trigger->SetCollisionProfileName(TEXT("NoCollision"));
		for (UBoxComponent* GateTrigger : GateTriggers)
		{
			GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
		}

		OperateGates(false);

		// 델리게이트 생성과 동시에 OnNPCSpawn()와 바인딩
		// OnNPCSpawn()을 EnemySpawnTime 초 뒤 호출
		GetWorld()->GetTimerManager().SetTimer(SpawnNPCTimerHandle, FTimerDelegate::CreateUObject(this, &AABSection::OnNPCSpawn), EnemySpawnTime, false);

		// 아이템 박스 생성
		GetWorld()->GetTimerManager().SetTimer(SpawnItemBoxTimerHandle, FTimerDelegate::CreateLambda([this]() -> void
		{
			// RandPointInCircle(): 범위 안에서 랜덤 위치를 반환
			FVector2D RandXY = FMath::RandPointInCircle(600.f);
			GetWorld()->SpawnActor<AABItemBox>(GetActorLocation() + FVector(RandXY, 30.f), FRotator::ZeroRotator);

		}), ItemBoxSpawnTime, false);
	}
		break;
	case AABSection::ESectionState::COMPLETE:
	{
		Trigger->SetCollisionProfileName(TEXT("NoCollision"));
		for (UBoxComponent* GateTrigger : GateTriggers)
		{
			GateTrigger->SetCollisionProfileName(TEXT("ABTrigger"));
		}

		OperateGates(true);
	}
		break;
	default:
		break;
	}

	CurrentState = NewState;
}

void AABSection::OperateGates(bool bOpen)
{
	// true라면 문을 연다. 
	for (UStaticMeshComponent* Gate : GateMeshes)
	{
		Gate->SetRelativeRotation(bOpen ? FRotator(0.f, -90.f, 0.f) : FRotator::ZeroRotator);
	}
}

void AABSection::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (CurrentState == ESectionState::READY)
	{
		SetState(ESectionState::BATTLE);
	}
}

void AABSection::OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABCHECK(OverlappedComponent->ComponentTags.Num() == 1);

	// 현재 출입문의 태그 + 소켓 이름의 2번째 자리까지 SocketName으로 가져오기 (+XGate -> +X)
	FName ComponentTag = OverlappedComponent->ComponentTags[0];
	FName SocketName = FName(*ComponentTag.ToString().Left(2));
	if (false == Mesh->DoesSocketExist(SocketName))
	{
		return;
	}

	// 아래는 SocketName을 이용해 출입문 뒤쪽에 다른 섹션 엑터가 있는지 검사하고, 다음 섹션 엑터를 생성하는 코드.
	FVector NewLocation = Mesh->GetSocketLocation(SocketName);

	TArray<FOverlapResult> OverlapResults;
	// FCollisionQueryParams (태그 이름, 추적 복잡성 여부(디테일한 판정이 아니라면 false), 무시할 엑터 배열(자기 자신))
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, this);
	FCollisionObjectQueryParams ObjectQueryParam(FCollisionObjectQueryParams::InitType::AllObjects);
	// 다른 섹션 엑터의 콜리전하고 겹쳤는지 검사?
	bool bResult = GetWorld()->OverlapMultiByObjectType(
			OverlapResults,
			NewLocation,
			FQuat::Identity,
			ObjectQueryParam,
			FCollisionShape::MakeSphere(775.f),
			CollisionQueryParam
	);

	// 겹치지 않았으면 그 자리에 섹션 엑터 생성
	if (false == bResult)
	{
		auto NewSection = GetWorld()->SpawnActor<AABSection>(NewLocation, FRotator::ZeroRotator);
	}
	else
	{
		ABLOG(Warning, TEXT("New Section Area is not empty."));
	}
}

void AABSection::OnNPCSpawn()
{
	GetWorld()->SpawnActor<AABCharacter>(GetActorLocation() + FVector::UpVector * 88.f, FRotator::ZeroRotator);
}

// Called every frame
void AABSection::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

