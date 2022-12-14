// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

enum class EItemState:uint8
{
	EIS_Hovering,
	EIS_Equipped
};

UCLASS()
class SLASH_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AItem();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure)
	float TransformedSin();

	UFUNCTION(BlueprintPure)
	float TransformedCos();

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USkeletalMeshComponent* ItemSkeletalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ItemStaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sine Parameters")
	float Amplitude=0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sine Parameters")
	float TimeConstant=5.f;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* Sphere;

	EItemState ItemState=EItemState::EIS_Hovering;

	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* ItemEffect;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	float RunningTime;

};

