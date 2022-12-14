// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Slash/Items/Item.h"
#include "Weapon.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API AWeapon : public AItem
{
	GENERATED_BODY()
	
public:
	AWeapon();

	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);

	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);

	TArray<AActor*> IgnoreActors;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

private:
	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	class USoundBase* EquipSound; 

	UPROPERTY(VisibleAnywhere, Category="Weapon Properties")
	class UBoxComponent* WeaponBox;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceStart;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;

	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	float Damage=20.f;

	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	FVector BoxTraceExtent=FVector(24.f,24.f,5.0f);

	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	bool bShowBoxDebug=false;

	void PlayEquipSound();
	void DisableSphereCollision();
	void DeactivateEmbers();
	void BoxTrace(FHitResult& BoxHit);
	void ExecuteGetHit(FHitResult& BoxHit);
	bool ActorIsSameType(AActor* OtherActor);

public:
	FORCEINLINE UBoxComponent* GetWeaponBox()const{return WeaponBox;}
};
