#pragma once

UENUM(BLueprintType)
enum class ESoulsCharacterState : uint8
{
	ESCS_Unequipped UMETA(DisplayName = "Unequipped"),
	ESCS_EquippedWeapon UMETA(DisplayName = "EquippedWeapon")
};

UENUM(BLueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_AttackingState UMETA(DisplayName = "AttackingState")
};

UENUM(BLueprintType)
enum class EDeathState : uint8
{
	EDS_Alive UMETA(DisplayName = "Alive"),
	EDS_Death1 UMETA(DisplayName = "Death1"),
	EDS_Death2 UMETA(DisplayName = "Death2"),
	EDS_Death3 UMETA(DisplayName = "Death3"),
	EDS_Death4 UMETA(DisplayName = "Death4"),
	EDS_Death5 UMETA(DisplayName = "Death5")
	
};
