// Fill out your copyright notice in the Description page of Project Settings.


#include "SessionSubsystem.h"
#include "OnlineSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

// UE���ň�ʓI�Ɏg����Œ薼�i"GameSession"�j
static const FName SESSION_NAME = NAME_GameSession;

bool USessionSubsystem::EnsureOnline()
{
    // Online Subsystem ���擾�iNull: LAN / Steam: Steam / EOS: Epic �Ȃǃv���b�g�t�H�[���ʂɐؑցj
    if (!OSS) OSS = IOnlineSubsystem::Get();
    if (!OSS) { UE_LOG(LogTemp, Error, TEXT("No OnlineSubsystem")); return false; }

    if (!Session.IsValid()) Session = OSS->GetSessionInterface();
    if (!Session.IsValid()) { UE_LOG(LogTemp, Error, TEXT("No SessionInterface")); return false; }

    return true;
}

void USessionSubsystem::ClearDelegates()
{
    if (!Session.IsValid()) return;

    if (OnCreateHandle.IsValid())  Session->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateHandle);
    if (OnDestroyHandle.IsValid()) Session->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroyHandle);

    OnCreateHandle.Reset();
    OnDestroyHandle.Reset();
}

void USessionSubsystem::CreateLanSession(int32 PublicConnections)
{
    if (!EnsureOnline()) return;

    // ���ɓ����Z�b�V�������c���Ă�����j�����Ă���č쐬
    if (Session->GetNamedSession(SESSION_NAME))
    {
        DestroyThenRecreate(PublicConnections);
        return;
    }

    // === �Z�b�V�����ݒ� ===
    FOnlineSessionSettings Settings;
    Settings.bIsLANMatch           = true;  // LAN�����Ώۂɂ���iNull Subsystem �O��j
    Settings.bShouldAdvertise      = true;  // Find �ɏo��
    Settings.bAllowJoinInProgress  = true;  // �r���Q��OK
    Settings.bUsesPresence         = false; // Null/LAN�Ȃ�s�v
    Settings.bUseLobbiesIfAvailable= false; // Null/LAN�ł̓��r�[�@�\�͎g��Ȃ�
    Settings.NumPublicConnections  = FMath::Max(1, PublicConnections); // �Q���g�i�z�X�g�����g����OK�j

    // �R�[���o�b�N�o�^
    OnCreateHandle = Session->AddOnCreateSessionCompleteDelegate_Handle(
        FOnCreateSessionCompleteDelegate::CreateUObject(this, &USessionSubsystem::OnCreateComplete));

    // ���s�iUserIndex=0��OK�B����LocalPlayer������ꍇ�͐ؑցj
    const bool bIssued = Session->CreateSession(/*LocalUserNum=*/0, SESSION_NAME, Settings);
    if (!bIssued)
    {
        UKismetSystemLibrary::PrintString(this, "CreateSession: immediate failure",
            true, true, FColor::Red, 4.f, TEXT("None"));
    }
}

void USessionSubsystem::DestroyThenRecreate(int32 PublicConnections)
{
    ClearDelegates();
    OnDestroyHandle = Session->AddOnDestroySessionCompleteDelegate_Handle(
        FOnDestroySessionCompleteDelegate::CreateWeakLambda(this, [this, PublicConnections](FName, bool)
            {
                CreateLanSession(PublicConnections);
            }));
    Session->DestroySession(SESSION_NAME);
}

void USessionSubsystem::OnCreateComplete(FName, bool bOk)
{
    // �Z�b�V�����J�n�i������Ԃ��u�X�^�[�g�v�Ɂj
    Session->StartSession(SESSION_NAME);

    UKismetSystemLibrary::PrintString(this, "OnCreateComplete: Success!!",
        true, true, FColor::Cyan, 4.f, TEXT("None"));
}

