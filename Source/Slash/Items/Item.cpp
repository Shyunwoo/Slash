// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Slash/DebugMacros.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Slash/Characters/SlashCharacter.h"
#include "NiagaraComponent.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;
	
	ItemSkeletalMesh=CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMeshComponent"));
	RootComponent=ItemSkeletalMesh;

	ItemSkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ItemStaticMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemStaticMesh"));
	ItemStaticMesh->SetupAttachment(GetRootComponent());

	ItemStaticMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());

	EmbersEffect=CreateDefaultSubobject<UNiagaraComponent>(TEXT("Embers"));
	EmbersEffect->SetupAttachment(GetRootComponent());
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
}

float AItem::TransformedSin()
{
	return Amplitude*FMath::Sin(RunningTime*TimeConstant);
}

float AItem::TransformedCos()
{
	return Amplitude*FMath::Cos(RunningTime*TimeConstant);
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(OtherActor);
	if(SlashCharacter)
	{
		SlashCharacter->SetOverlappingItem(this);
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(OtherActor);
	if(SlashCharacter)
	{
		SlashCharacter->SetOverlappingItem(nullptr);
	}
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime+=DeltaTime;

	if(ItemState==EItemState::EIS_Hovering)
	{
		AddActorWorldOffset(FVector(0.f,0.f,TransformedSin()));
	}
}

