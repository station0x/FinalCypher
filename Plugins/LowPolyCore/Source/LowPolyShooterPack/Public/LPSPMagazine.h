//Copyright 2021, Infima Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LPSPAttachment.h"
#include "LPSPMagazine.generated.h"

UCLASS(Abstract)
class LOWPOLYSHOOTERPACK_API ALPSPMagazine final : public ALPSPAttachment
{
	GENERATED_BODY()

public:
	/**Called when beginning the game.*/
	virtual void BeginPlay() override;

	/**Returns the current ammunition left in this mag.*/
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Low Poly Shooter Pack | Magazine")
	int GetAmmo() const { return CurrentAmmo; }

	/**Returns the total ammunition that this magazine can hold.*/
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Low Poly Shooter Pack | Magazine")
	int GetTotalAmmo() const { return TotalAmmo; }

	/**Adds ammunition to the mag. This function is also used to remove ammunition from the mag by using a negative amount.*/
	UFUNCTION(BlueprintCallable, Category = "Low Poly Shooter Pack | Magazine")
	void AddAmmo(const int Amount);

	/**Fills the ammunition in the mag.*/
	UFUNCTION(BlueprintCallable, Category = "Low Poly Shooter Pack | Magazine")
	void Fill();

	/**Returns true if the magazine currently contains no ammo.*/
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Low Poly Shooter Pack | Magazine")
	bool IsEmpty() const { return CurrentAmmo <= 0; }
	
	/**Returns true if the magazine is currently maxed out.*/
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Low Poly Shooter Pack | Magazine")
    bool IsFull() const { return CurrentAmmo == TotalAmmo; }

	/**Returns the class of casing that this mag uses.*/
	UFUNCTION(BlueprintPure, BlueprintCallable, BlueprintNativeEvent, Category = "Low Poly Shooter Pack | Magazine")
	bool TryGetCasingType(TSubclassOf<class ALPSPCasing> &Type) const;

	/**Returns the range of possible impulses for casings.*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Low Poly Shooter Pack | Magazine")
	FVector2D GetCasingImpulseRange() const;

	/**Returns the offset from a weapon's ejection port for a casing from this magazine.*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Low Poly Shooter Pack | Magazine")
	float GetCasingOffset() const;

	/**Returns the type of the projectile this magazine fires.*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Low Poly Shooter Pack | Magazine")
	bool TryGetProjectileType(TSubclassOf<class ALPSPProjectile> &Type) const;

	/**Returns the range of possible velocities to use when firing projectiles from this magazine.*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Low Poly Shooter Pack | Magazine")
	FVector2D GetProjectileVelocityRange() const;

	/**Returns the range of possible pellets shot from this magazine's projectile.*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Low Poly Shooter Pack | Magazine")
	FVector2D GetProjectilePelletRange() const;
	
	/**Event called when this magazine is fully filled with ammunition.*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Low Poly Shooter Pack | Magazine")
	void OnFill();

	/**Event called when the current ammo count changes.*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Low Poly Shooter Pack | Magazine")
	void OnChangeAmmoCount();

protected:
	/**Start Casing Implementations.*/
	bool TryGetCasingType_Implementation(TSubclassOf<class ALPSPCasing> &Type) const;
	FVector2D GetCasingImpulseRange_Implementation() const { return CasingImpulseRange; }
	float GetCasingOffset_Implementation() const { return CasingOffset; }
	/**End Casing Implementations.*/

	/**Start Projectile Implementations.*/
	bool TryGetProjectileType_Implementation(TSubclassOf<class ALPSPProjectile> &Type) const;
	FVector2D GetProjectileVelocityRange_Implementation() const { return ProjectileVelocityRange; }
	FVector2D GetProjectilePelletRange_Implementation() const { return ProjectilePelletRange; }
	/**End Projectile Implementations.*/

private:
	/**Total ammunition this mag can hold. Also used as the starting ammunition.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Magazine")
	int TotalAmmo = 10;

	/**Type of casing used when spawning casings with this mag equipped.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Magazine")
	TSubclassOf<class ALPSPCasing> CasingType;

	/**Range of possible impulses for casings coming out of weapons using this magazine.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Magazine")
	FVector2D CasingImpulseRange = FVector2D(75.0f, 100.0f);

	/**Offset from a weapon's ejection port that this magazine's casing will be spawned at. (Casing notifies also have an offset that can add to this one).*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Magazine")
	float CasingOffset = 0.0f;

	/**Type of projectile fired from this type of magazine.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Magazine")
	TSubclassOf<class ALPSPProjectile> ProjectileType;

	/**Range of possible velocities for projectiles fired from weapons using this magazine.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Magazine")
	FVector2D ProjectileVelocityRange = FVector2D(13000.0f, 17000.0f);

	/**Range of possible pellet counts to spawn when firing from this magazine.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Magazine")
	FVector2D ProjectilePelletRange = FVector2D(1.0f, 1.0f);
	
	/**Current ammunition left in this mag.*/
	int CurrentAmmo;	
};