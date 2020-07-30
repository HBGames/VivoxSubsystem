// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "VoiceChat.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "VivoxSubsystem.generated.h"

class FVivoxSubsystemModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

template<class T>
class CachedProperty
{
public:
	explicit CachedProperty(T value) {
		m_dirty = false;
		m_value = value;
	}

	const T &GetValue() const {
		return m_value;
	}

	void SetValue(const T &value) {
		if(m_value != value) {
			m_value = value;
			m_dirty = true;
		}
	}

	void SetDirty(bool value) {
		m_dirty = value;
	}

	bool IsDirty() const {
		return m_dirty;
	}
protected:
	bool m_dirty;
	T m_value;
};

/*
 *
 */
UCLASS()
class VIVOXSUBSYSTEM_API UVivoxSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	UVivoxSubsystem();
	~UVivoxSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category="Vivox")
    void Update3DPosition(APawn* Pawn);
	
private:
	IVoiceChat* VivoxChat;
	FString ConnectedPositionalChannel;
	FString LastKnownTransmittingChannel;

	// Cached 3D position and orientation
	CachedProperty<FVector> CachedPosition = CachedProperty<FVector>(FVector());
	CachedProperty<FVector> CachedForwardVector = CachedProperty<FVector>(FVector());
	CachedProperty<FVector> CachedUpVector = CachedProperty<FVector>(FVector());

	// Private methods to check and clear dirtiness of cached 3D position
	bool Get3DValuesAreDirty() const;
	void Clear3DValuesAreDirty();
	
};
