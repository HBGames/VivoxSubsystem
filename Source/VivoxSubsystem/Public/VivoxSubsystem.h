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

template <class T>
class CachedProperty
{
public:
	explicit CachedProperty(T value)
	{
		m_dirty = false;
		m_value = value;
	}

	const T& GetValue() const
	{
		return m_value;
	}

	void SetValue(const T& value)
	{
		if (m_value != value)
		{
			m_value = value;
			m_dirty = true;
		}
	}

	void SetDirty(bool value)
	{
		m_dirty = value;
	}

	bool IsDirty() const
	{
		return m_dirty;
	}

protected:
	bool m_dirty;
	T m_value;
};

UENUM(BlueprintType)
enum class EVivoxVoiceChatChannelType : uint8
{
	/** Non positional/2d audio channel */
	NonPositional,
	/** Positional/3d audio channel */
	Positional,
	/** Echo channel. Will only ever have one player and will echo anything you say */
	Echo
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
	void Connect();

	/**
	* Get an array of channels the user is in
	*
	* @return Array of connected channel names
	*/
	UFUNCTION(BlueprintCallable, Category="Vivox")
	TArray<FString> GetChannels() const;

	UFUNCTION(BlueprintCallable, Category="Vivox")
	EVivoxVoiceChatChannelType GetChannelType(const FString& ChannelName) const;

	UFUNCTION(BlueprintCallable, Category="Vivox")
	void Login(const FString& PlayerName);

	/**
	* Mute or unmute a player
	*
	* @param PlayerName Player to mute
	* @param bMuted true if the player should be muted
	*/
	UFUNCTION(BlueprintCallable, Category="Vivox")
	void SetPlayerMuted(const FString& PlayerName, bool bMuted) const;

	/**
	* Are we logged in?
	*
	* @return true if we are logged in to the voice server
	*/
	UFUNCTION(BlueprintPure, Category="Vivox")
	bool IsLoggedIn() const;
	
	/**
	* Are we logging in?
	*
	* @return true if we are logging in to the voice server
	*/
	UFUNCTION(BlueprintPure, Category="Vivox")
	bool IsLoggingIn() const;

	/**
	 * Check if a player is muted
	 *
	 * @param PlayerName Player to get the mute state of
	 * @return true if player is muted
	 */
	UFUNCTION(BlueprintPure, Category="Vivox")
	bool IsPlayerMuted(const FString& PlayerName) const;

	/**
	 * Mute or unmute the audio input device
	 *
	 * @param bIsMuted set to true to mute the device or false to unmute
	 */
	UFUNCTION(BlueprintCallable, Category="Vivox")
	void SetAudioInputDeviceMuted(bool bIsMuted) const;

	/**
	 * Mute or unmute the audio output device
	 *
	 * @param bIsMuted set to true to mute the device or false to unmute
	 */
	UFUNCTION(BlueprintCallable, Category="Vivox")
	void SetAudioOutputDeviceMuted(bool bIsMuted) const;

	UFUNCTION(BlueprintCallable, Category="Vivox")
	void Update3DPosition(APawn* Pawn);

	static FString GetVivoxSafePlayerName(FString BaseName);
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
