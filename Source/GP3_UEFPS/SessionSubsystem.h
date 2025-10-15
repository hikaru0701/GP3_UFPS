// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Online/OnlineSessionNames.h"
#include "OnlineSessionSettings.h"
#include "SessionSubsystem.generated.h"

UCLASS()
class GP3_UEFPS_API USessionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category = "Session")
    void CreateLanSession(int32 PublicConnections = 3);

private:
    // Online Subsystem �擾
    bool EnsureOnline();

    // �f���Q�[�g�n���h���Ǘ��i�����Y��h�~�j
    void ClearDelegates();

    // Create �̑O�Ɋ����Z�b�V����������Ή�
    void DestroyThenRecreate(int32 PublicConnections);

    // Online �C���^�t�F�[�X
    IOnlineSubsystem* OSS = nullptr;
    IOnlineSessionPtr  Session;

    // �f���Q�[�g
    FDelegateHandle OnCreateHandle;
    FDelegateHandle OnDestroyHandle;

    // �R�[���o�b�N
    void OnCreateComplete(FName SessionName, bool bOk);
};
