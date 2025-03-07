﻿// Copyright (c) 2025 Nick McClure (Protospatial). All Rights Reserved.

#include "NodeToCode.h"

#include <Core/N2CWidgetContainer.h>

#include "Models/N2CLogging.h"
#include "Core/N2CEditorIntegration.h"
#include "Core/N2CSettings.h"
#include "Code Editor/Models/N2CCodeEditorStyle.h"
#include "Code Editor/Syntax/N2CSyntaxDefinitionFactory.h"
#include "Code Editor/Widgets/N2CCodeEditorWidgetFactory.h"
#include "Models/N2CStyle.h"


DEFINE_LOG_CATEGORY(LogNodeToCode);

#define LOCTEXT_NAMESPACE "FNodeToCodeModule"

void FNodeToCodeModule::StartupModule()
{
    // Initialize logging
    FN2CLogger::Get().Log(TEXT("NodeToCode plugin starting up"), EN2CLogSeverity::Info);

    // Manually load the config files for UE 5.5 compatibility
    FString UserSecretsConfigPath = FPaths::ConvertRelativePathToFull(FPaths::Combine(
        FPaths::ProjectSavedDir(), TEXT("Config"), FPlatformProperties::PlatformName(), 
        TEXT("EditorNodeToCodeSecrets.ini")));
    
    // Attempt to load the config file
    GConfig->LoadFile(UserSecretsConfigPath);
    
    // Populate user secrets from loaded data
    UN2CUserSecrets* UserSecrets = GetMutableDefault<UN2CUserSecrets>();
    UserSecrets->LoadConfig();
    
    // Apply configured log severity from settings
    const UN2CSettings* Settings = GetDefault<UN2CSettings>();
    if (Settings)
    {
        FN2CLogger::Get().SetMinSeverity(Settings->MinSeverity);
        FN2CLogger::Get().Log(TEXT("Applied log severity from settings"), EN2CLogSeverity::Debug);
    }

    
    // Initialize style system
    N2CStyle::Initialize();
    FN2CLogger::Get().Log(TEXT("Node to Code style initialized"), EN2CLogSeverity::Debug);

    // Initialize code editor style system
    FN2CCodeEditorStyle::Initialize();
    FN2CLogger::Get().Log(TEXT("Code editor style initialized"), EN2CLogSeverity::Debug);

    // Initialize editor integration
    FN2CEditorIntegration::Get().Initialize();
    FN2CLogger::Get().Log(TEXT("Editor integration initialized"), EN2CLogSeverity::Debug);
    
    // Register widget factory
    FN2CCodeEditorWidgetFactory::Register();
    FN2CLogger::Get().Log(TEXT("Widget factory registered"), EN2CLogSeverity::Debug);
    
    // Verify syntax factory is working
    auto CPPSyntax = FN2CSyntaxDefinitionFactory::Get().CreateDefinition(EN2CCodeLanguage::Cpp);
    auto PythonSyntax = FN2CSyntaxDefinitionFactory::Get().CreateDefinition(EN2CCodeLanguage::Python);
    auto JSSyntax = FN2CSyntaxDefinitionFactory::Get().CreateDefinition(EN2CCodeLanguage::JavaScript);
    auto CSharpSyntax = FN2CSyntaxDefinitionFactory::Get().CreateDefinition(EN2CCodeLanguage::CSharp);
    auto SwiftSyntax = FN2CSyntaxDefinitionFactory::Get().CreateDefinition(EN2CCodeLanguage::Swift);
    auto PseudocodeSyntax = FN2CSyntaxDefinitionFactory::Get().CreateDefinition(EN2CCodeLanguage::Pseudocode);

    if (!CPPSyntax || !PythonSyntax || !JSSyntax || !CSharpSyntax || !SwiftSyntax || !PseudocodeSyntax)
    {
        FN2CLogger::Get().LogError(TEXT("Failed to initialize syntax definitions"), TEXT("NodeToCode"));
    }
    else 
    {
        FN2CLogger::Get().Log(TEXT("Syntax definitions initialized successfully"), EN2CLogSeverity::Debug);
    }
}

void FNodeToCodeModule::ShutdownModule()
{
    // Unregister menu extensions
    UToolMenus::UnRegisterStartupCallback(this);
    UToolMenus::UnregisterOwner(this);

    // Shutdown editor integration
    FN2CEditorIntegration::Get().Shutdown();

    // Unregister widget factory
    FN2CCodeEditorWidgetFactory::Unregister();

    // Shutdown code editor style system
    FN2CCodeEditorStyle::Shutdown();

    // Shutdown style system
    N2CStyle::Shutdown();
    
    // Clean up widget container
    if (!GExitPurge)  // Only clean up if not already in exit purge
    {
        UN2CWidgetContainer::Reset();
    }

    FN2CLogger::Get().Log(TEXT("NodeToCode plugin shutting down"), EN2CLogSeverity::Info);
}


#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FNodeToCodeModule, NodeToCode)
