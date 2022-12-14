// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Slash/Interfaces/HitInterface.h"
#include "CharacterTypes.h"
#include "BaseCharacter.generated.h"

UCLASS()
class SLASH_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

protected:
	virtual void BeginPlay() override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void Attack();
	virtual void Die();
	bool IsAlive();
	void DisableMeshCollision();

	void PlayHitReactMontage(const FName& SectionName);
	virtual void DirectionalHitReact(const FVector& ImpactPoint);
	void SpawnHitParticle(const FVector& ImpactPoint);
	virtual void HandleDamage(float DamageAmount);
	void PlayMontageSection(class UAnimMontage* Montage, const FName& SectionName);
	int32 PlayRandomMontageSection(UAnimMontage* Montage, TArray<FName>& SectionNames);
	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
	virtual void PlayDegeMontage();
	void DisableCapsule();
	void StopAttackMontage();

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();
	virtual bool CanAttack();

	UFUNCTION(BlueprintCallable)
	virtual void DodgeEnd();
protected:
	UPROPERTY(VisibleAnywhere, Category=Weapon)
	class AWeapon* EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, Category=Montages)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category=Montages)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category=Montages)
	UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category=Montages)
	UAnimMontage* DodgeMontage;

	UPROPERTY(EditAnywhere, Category= Combat)
	TArray<FName> AttackMontageSections;

	UPROPERTY(EditAnywhere, Category= Combat)
	TArray<FName> DeathMontageSections;

	UPROPERTY(VisibleAnywhere)
	class UAttributeComponent* Attributes;

	UPROPERTY(BlueprintReadOnly, Category=Combat)
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere, Category=Combat)
	double WarpTargetDistance=150.f;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;

private:
	UPROPERTY(EditAnywhere, Category=VisualEffects)
	UParticleSystem* HitParticles;

public:
	FORCEINLINE TEnumAsByte<EDeathPose> GetDeathPose() const {return DeathPose;}
};
