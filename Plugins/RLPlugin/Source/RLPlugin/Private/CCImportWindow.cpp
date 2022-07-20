#include "CCImportWindow.h"
//#include "UnrealEd.h"
#include "IDocumentation.h"
//#include <Slate.h>
#include "CCImportUI.h"
//#include "UIAction.h"
#include <Editor/PropertyEditor/Public/PropertyEditorModule.h>
#define LOCTEXT_NAMESPACE "CCImport"
void SCCImportWindow::Construct(const FArguments& InArgs)
{
	ccImportUI = InArgs._ImportUI;
	check(ccImportUI);
	//TargetPath = InArgs._TargetPath.ToString();	
	WidgetWindow = InArgs._WidgetWindow;
    
	
	bool livelink = true;//出版本記得改 false
/*
	if (livelink && !ccImportUI->isCanChangeAutoEnable)
	{
		
		this->ChildSlot		
		[
			
			SNew(SVerticalBox)			
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(3.0f, 3.0f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.VAlign(VAlign_Center)		
				.HAlign(HAlign_Left)
				.AutoHeight()
				[
					SNew(STextBlock)
					.MinDesiredWidth(100)
					.Font(FEditorStyle::GetFontStyle("CurveEd.LabelFont"))
					.Text(LOCTEXT("CC_Auto_Setup", "CC Setup : "))
					//.Visibility(ccImportUI->isCanChangeAutoEnable ? EVisibility::Visible : EVisibility::Hidden)				
				]			

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(16.0f,3.0f)
				[				
					SAssignNew(CCAutoSetup_CheckBox, SCheckBox)
					.IsChecked(ccImportUI->isCCAutoSetup ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
					.OnCheckStateChanged(this, &SCCImportWindow::OnCCAutoSetupChanged)
					//.ToolTipText(LOCTEXT("Standard_Shader_Tip", "If checked standard shader will be setup"))
					//.Visibility(ccImportUI->isCanChangeAutoEnable ? EVisibility::Visible : EVisibility::Hidden)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("Enable_CCAutoSetup", "Enable"))
					]
				]

				+ SVerticalBox::Slot()
				.VAlign(VAlign_Center)		
				.HAlign(HAlign_Left)
				[
					SNew(STextBlock)
					.MinDesiredWidth(100)
					.Font(FEditorStyle::GetFontStyle("CurveEd.LabelFont"))
					.Text(LOCTEXT("Shader_Type", "Shader type : "))
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(16.0f, 3.0f)
				[				
					SAssignNew(Standard_CheckBox, SCheckBox)
					.IsChecked(ccImportUI->isStandardSkin ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
					.OnCheckStateChanged(this, &SCCImportWindow::OnStandardChanged)
					.ToolTipText(LOCTEXT("Standard_Shader_Tip", "The default character materials in Unreal"))	
					.IsEnabled(ccImportUI->isCCAutoSetup)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("Standard_Shader", "Standard Shader"))
					]				
				]
	
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(16.0f, 3.0f)
				[
					SAssignNew(LW_CheckBox, SCheckBox)
					.IsChecked(ccImportUI->isLWSkin ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
					.OnCheckStateChanged(this, &SCCImportWindow::OnLWChanged)
					.ToolTipText(LOCTEXT("LW_Shader_Tip", "A light weight skin shader is provided while the HQ shader is deployed to other parts of the body: eyes, teeth, tongue, hair, and eyelashes for better overall quality."))	
					.IsEnabled((ccImportUI->isCCAutoSetup && ccImportUI->hasCCShader))
					[
						SNew(STextBlock)
						.Text(LOCTEXT("LW_Shader", "LW Shader"))
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(16.0f, 3.0f)
				[
					SAssignNew(HQ_CheckBox, SCheckBox)
					.IsChecked(ccImportUI->isHQSkin ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
					.OnCheckStateChanged(this, &SCCImportWindow::OnHQChanged)
					.ToolTipText(LOCTEXT("HQ_Shader_Tip", "An ultra-high quality skin shader is provided while the LW shader is deployed to other parts of the body: eyes, teeth, tongue, hair, and eyelashes for better overall quality."))
					.IsEnabled((ccImportUI->isCCAutoSetup && ccImportUI->hasCCShader))
					[
						SNew(STextBlock)
						.Text(LOCTEXT("HQ_Shader", "HQ Shader"))
					]
				]
			]
		
			+ SVerticalBox::Slot()
			.VAlign(VAlign_Center)		
			.HAlign(HAlign_Left)
			.AutoHeight()
			.Padding(0.0f, 25.0f, 16.0f, 10.0f)
			[
				SNew(STextBlock)
				.MinDesiredWidth(100)
				.Font(FEditorStyle::GetFontStyle("CurveEd.LabelFont"))
				.Text(LOCTEXT("Create_Live_Link_Actor", "Create Live Link Actor : "))
				//.Visibility(ccImportUI->isCanChangeAutoEnable ? EVisibility::Visible : EVisibility::Hidden)				
			]			
			
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(16.0f, 3.0f)
			[				
				SNew(SCheckBox)
				.IsChecked(ccImportUI->isLiveLink ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
				.OnCheckStateChanged(this, &SCCImportWindow::OnCCLiveLinkChanged)
				//.ToolTipText(LOCTEXT("Standard_Shader_Tip", "If checked standard shader will be setup"))
				//.Visibility(ccImportUI->isCanChangeAutoEnable ? EVisibility::Visible : EVisibility::Hidden)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("Enable_Live_Link", "Enable"))
				]
			]
			


			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Bottom)
			.Padding(2)
			[
				SNew(SUniformGridPanel)
				.SlotPadding(2)
				+ SUniformGridPanel::Slot(1, 0)
				[
					SNew(SButton)
					.HAlign(HAlign_Center)
					.Text(LOCTEXT("CCImportOptions_Next", "Next"))				
					.ToolTipText(LOCTEXT("CCImportOptions_Next_Tip", "The fbx file import options will be displayed next."))
				//	.IsEnabled(this, &SToolbagImportOptionsWindow::CanImport)
					.OnClicked(this, &SCCImportWindow::OnNext)
				]				
				
				+ SUniformGridPanel::Slot(2, 0)
				[
					SNew(SButton)
					.HAlign(HAlign_Center)
					.Text(LOCTEXT("CCImportOptions_Cancel", "Cancel"))
					.ToolTipText(LOCTEXT("ImportOptionsWindow_Cancel_Tip", "Cancels importing with CC Setup"))
					.OnClicked(this, &SCCImportWindow::OnCancel)
				]
			]		
		];


	}
	else*/ if(ccImportUI->isCanChangeAutoEnable)
	{
		this->ChildSlot		
		[
			//SNew(SBox)
			//[
				SNew(SVerticalBox)
			/*	+ SVerticalBox::Slot()
				.Padding(3.0f, 1.0f)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.Padding(2.0f)
					[
						SNew(SComboButton)
						//.OnGetMenuContent(this, &SCCImportWindow::GetTestContent)
						.ButtonContent()
						[
							SNew(STextBlock)
							.Text(LOCTEXT("BlockingVolumeMenu", "Create Blocking Volume"))		
						]	
					]
				]*/
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(3.0f, 3.0f)
				[/*
					SNew(SExpandableArea)
					.AreaTitle( LOCTEXT("CCImportOptions", "Material Options") )
					.InitiallyCollapsed(true)
					.BodyContent()
					[
						
					]*/
					SNew(SVerticalBox)
				/*	+ SVerticalBox::Slot()
					.VAlign(VAlign_Center)		
					.HAlign(HAlign_Left)
					.AutoHeight()
					[
						SNew(STextBlock)
						.MinDesiredWidth(100)
						.Font(FEditorStyle::GetFontStyle("CurveEd.LabelFont"))
						.Text(LOCTEXT("CC_Auto_Setup", "CC and iClone Auto Setup : "))
						//.Visibility(ccImportUI->isCanChangeAutoEnable ? EVisibility::Visible : EVisibility::Hidden)				
					]			
                    */

					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(2.0f, 6.0f, 6.0f,3.0f)
					[				
						SAssignNew(CCAutoSetup_CheckBox, SCheckBox)
						.IsChecked(ccImportUI->isCCAutoSetup ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
						.OnCheckStateChanged(this, &SCCImportWindow::OnCCAutoSetupChanged)
						//.ToolTipText(LOCTEXT("Standard_Shader_Tip", "If checked standard shader will be setup"))
						//.Visibility(ccImportUI->isCanChangeAutoEnable ? EVisibility::Visible : EVisibility::Hidden)
						[
							SNew(STextBlock)
                            .Font(FEditorStyle::GetFontStyle("CurveEd.LabelFont"))
							.Text(LOCTEXT("Enable_CCAutoSetup", "Enable Character Creator & iClone Auto Setup"))
						]
					]
                    + SVerticalBox::Slot()                    
                    .HAlign(HAlign_Left)
                    .AutoHeight()
                    .Padding(20.0f, 0.0f, 6.0f, 0.0f)
                    [
                        SNew(STextBlock)
                        .MinDesiredWidth(100) 
                        .AutoWrapText(true)
                        .Text(LOCTEXT("Enable_Tips", "You should uncheck this option for FBX files that do not originate from Character Creator (CC) and iClone."))
                    ]


					+ SVerticalBox::Slot()
					.VAlign(VAlign_Center)		
					.HAlign(HAlign_Left)
                    .Padding(20.0f, 3.0f, 6.0f, 3.0f)
					[
						SNew(STextBlock)
						.MinDesiredWidth(100)
						.Font(FEditorStyle::GetFontStyle("CurveEd.LabelFont"))
						.Text(LOCTEXT("Shader_Type", "Shader Type : "))
					]

					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(32.0f, 3.0f)
					[				
						SAssignNew(Standard_CheckBox, SCheckBox)
						.IsChecked(ccImportUI->isStandardSkin ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
						.OnCheckStateChanged(this, &SCCImportWindow::OnStandardChanged)
						//.ToolTipText(LOCTEXT("Standard_Shader_Tip", "The default character materials in Unreal"))	
						.IsEnabled(ccImportUI->isCCAutoSetup)
						.Style(FCoreStyle::Get(), "RadioButton")
						[
							SNew(STextBlock)
							.Text(LOCTEXT("Standard_Shader", "Standard Shader"))
						]				
					]
	
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(32.0f, 3.0f)
					[
						SAssignNew(LW_CheckBox, SCheckBox)
						.IsChecked(ccImportUI->isLWSkin ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
						.OnCheckStateChanged(this, &SCCImportWindow::OnLWChanged)
						.ToolTipText(LOCTEXT("LW_Shader_Tip", "A light weight skin shader is provided while the HQ shader is deployed to other parts of the body: eyes, teeth, tongue, hair, and eyelashes for better overall quality."))	
						.IsEnabled((ccImportUI->isCCAutoSetup && ccImportUI->hasCCShader))
						.Style(FCoreStyle::Get(), "RadioButton")
						[
							SNew(STextBlock)
							.Text(LOCTEXT("LW_Shader", "LW Shader"))
						]
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(32.0f, 3.0f)
					[
						SAssignNew(HQ_CheckBox, SCheckBox)
						.IsChecked(ccImportUI->isHQSkin ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
						.OnCheckStateChanged(this, &SCCImportWindow::OnHQChanged)
						.ToolTipText(LOCTEXT("HQ_Shader_Tip", "An ultra-high quality skin shader is provided while the LW shader is deployed to other parts of the body: eyes, teeth, tongue, hair, and eyelashes for better overall quality."))
						.IsEnabled((ccImportUI->isCCAutoSetup && ccImportUI->hasCCShader))
						.Style(FCoreStyle::Get(), "RadioButton")
						[
							SNew(STextBlock)
							.Text(LOCTEXT("HQ_Shader", "HQ Shader"))
						]
					]
				]

                + SVerticalBox::Slot()
                .AutoHeight()
                .HAlign(HAlign_Right)
                .VAlign(VAlign_Bottom)
                .Padding(2, 3, 2, 0)
				[
                    SNew(SUniformGridPanel)
                    .SlotPadding(FMargin(2, 0, 2, 0))
                    + SUniformGridPanel::Slot(0, 0)                    
					[
						SNew(SButton)
						.HAlign(HAlign_Center)
						.Text(LOCTEXT("CCImportOptions_Next", "OK"))				
						//.ToolTipText(LOCTEXT("CCImportOptions_Next_Tip", "The fbx file import options will be displayed next."))
					//	.IsEnabled(this, &SToolbagImportOptionsWindow::CanImport)
						.OnClicked(this, &SCCImportWindow::OnNext)
					]
                    + SUniformGridPanel::Slot(1, 0)
					[
						SNew(SButton)
						.HAlign(HAlign_Center)
						.Text(LOCTEXT("CCImportOptions_Cancel", "Cancel"))
						//.ToolTipText(LOCTEXT("ImportOptionsWindow_Cancel_Tip", "Cancels importing with CC Setup"))
						.OnClicked(this, &SCCImportWindow::OnCancel)
					]
                    + SUniformGridPanel::Slot(2, 0)
                    [
                        SNew(SButton)
                        .HAlign(HAlign_Center)
                        .Text(LOCTEXT("CCImportOptions_More", "More"))
                        //.ToolTipText(LOCTEXT("CCImportOptions_More_Tip", "More"))                   
                        .OnClicked(this, &SCCImportWindow::OnMore)
                    ]                    
				]
		//	]
		];
	}
	else
	{
		this->ChildSlot		
		[			
			SNew(SVerticalBox)			
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(3.0f, 3.0f)
			[
				SNew(SVerticalBox)			
				+ SVerticalBox::Slot()
				.VAlign(VAlign_Center)		
				.HAlign(HAlign_Left)
                .Padding(0.0f, 6.0f, 0.0f, 3.0f)
				[
					SNew(STextBlock)
					.MinDesiredWidth(100)           
					.Font(FEditorStyle::GetFontStyle("CurveEd.LabelFont"))
					.Text(LOCTEXT("Shader_Type", "Shader Type :"))
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(16.0f, 3.0f)
				[				
					SAssignNew(Standard_CheckBox, SCheckBox)
					.IsChecked(ccImportUI->isStandardSkin ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
					.OnCheckStateChanged(this, &SCCImportWindow::OnStandardChanged)
					//.ToolTipText(LOCTEXT("Standard_Shader_Tip", "The default character materials in Unreal"))	
					.IsEnabled(ccImportUI->isCCAutoSetup)
					.Style(FCoreStyle::Get(), "RadioButton")
					[
						SNew(STextBlock)
						.Text(LOCTEXT("Standard_Shader", "Standard Shader"))
					]				
				]
	
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(16.0f, 3.0f)
				[
					SAssignNew(LW_CheckBox, SCheckBox)
					.IsChecked(ccImportUI->isLWSkin ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
					.OnCheckStateChanged(this, &SCCImportWindow::OnLWChanged)
					.ToolTipText(LOCTEXT("LW_Shader_Tip", "A light weight skin shader is provided while the HQ shader is deployed to other parts of the body: eyes, teeth, tongue, hair, and eyelashes for better overall quality."))	
					.IsEnabled((ccImportUI->isCCAutoSetup && ccImportUI->hasCCShader))
					.Style(FCoreStyle::Get(), "RadioButton")
					[
						SNew(STextBlock)
						.Text(LOCTEXT("LW_Shader", "LW Shader"))
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(16.0f, 3.0f)
				[
					SAssignNew(HQ_CheckBox, SCheckBox)
					.IsChecked(ccImportUI->isHQSkin ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
					.OnCheckStateChanged(this, &SCCImportWindow::OnHQChanged)
					.ToolTipText(LOCTEXT("HQ_Shader_Tip", "An ultra-high quality skin shader is provided while the LW shader is deployed to other parts of the body: eyes, teeth, tongue, hair, and eyelashes for better overall quality."))
					.IsEnabled((ccImportUI->isCCAutoSetup && ccImportUI->hasCCShader))
					.Style(FCoreStyle::Get(), "RadioButton")
					[
						SNew(STextBlock)
						.Text(LOCTEXT("HQ_Shader", "HQ Shader"))
					]
				]
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
            .HAlign(HAlign_Right)
			.VAlign(VAlign_Bottom)
            .Padding(2, 3, 2, 0)
			[               
				SNew(SUniformGridPanel)     
                .SlotPadding(FMargin(2, 0, 2, 0))
                + SUniformGridPanel::Slot(0, 0)
                [
					SNew(SButton)
					.HAlign(HAlign_Center)
					.Text(LOCTEXT("CCImportOptions_Start", "Apply"))				
				//	.ToolTipText(LOCTEXT("CCImportOptions_Start_Tip", "Start CC Setup"))
				//	.IsEnabled(this, &SToolbagImportOptionsWindow::CanImport)
					.OnClicked(this, &SCCImportWindow::OnNext)
				]		
				+ SUniformGridPanel::Slot(1, 0)                    
				[
					SNew(SButton)
					.HAlign(HAlign_Center)
					.Text(LOCTEXT("CCImportOptions_Cancel", "Cancel"))
					//.ToolTipText(LOCTEXT("ImportOptionsWindow_Cancel_Tip", "Cancels importing with CC Setup"))
					.OnClicked(this, &SCCImportWindow::OnCancel)
				]
                + SUniformGridPanel::Slot(2, 0)
                [
                    SNew(SButton)
                    .HAlign(HAlign_Center)
                    .Text(LOCTEXT("CCImportOptions_More", "More"))
                    //.ToolTipText(LOCTEXT("CCImportOptions_More_Tip", "More"))                   
                    .OnClicked(this, &SCCImportWindow::OnMore)
                ]                            
			]		
		];
	}
}
/*
TSharedRef<SWidget> SCCImportWindow::GetTestContent() const
{
	FMenuBuilder MenuBuilder(true, NULL);

	MenuBuilder.AddMenuEntry(
		
		FText::FromString(TEXT("AAA")),
		FText::FromString(TEXT("BBB")),
		FSlateIcon()
		
	);

	return MenuBuilder.MakeWidget();
}
*/


ECheckBoxState SCCImportWindow::IsStandardActive() const
{
	if (skinType == "Standard")
	{
		return ECheckBoxState::Checked;
	}
	return  ECheckBoxState::Unchecked;
}
ECheckBoxState SCCImportWindow::IsHQActive() const
{
	if (skinType == "HQ")
	{
		return ECheckBoxState::Checked;
	}
	return  ECheckBoxState::Unchecked;
}
ECheckBoxState SCCImportWindow::IsLWActive() const
{
	if (skinType == "LW")
	{
		return ECheckBoxState::Checked;
	}
	return  ECheckBoxState::Unchecked;
}

void SCCImportWindow::OnCCLiveLinkChanged(ECheckBoxState InCheckedState)
{
	if (InCheckedState == ECheckBoxState::Checked)
	{/*
		Standard_CheckBox->SetEnabled(false);
		LW_CheckBox->SetEnabled(false);
		HQ_CheckBox->SetEnabled(false);
		ccImportUI->isCCAutoSetup = false;
		CCAutoSetup_CheckBox.Get()->SetIsChecked(ECheckBoxState::Checked);
		*/
		ccImportUI->isLiveLink = true;
	}
	else
	{/*
		Standard_CheckBox->SetEnabled(true);
		if (ccImportUI->hasCCShader)
		{
			LW_CheckBox->SetEnabled(true);
			HQ_CheckBox->SetEnabled(true);
		}
		ccImportUI->isCCAutoSetup = true;
		CCAutoSetup_CheckBox.Get()->SetIsChecked(ECheckBoxState::Unchecked);
		*/
		ccImportUI->isLiveLink = false;
	}
}


void SCCImportWindow::OnCCAutoSetupChanged(ECheckBoxState InCheckedState)
{
	if (InCheckedState == ECheckBoxState::Checked)
	{
		Standard_CheckBox->SetEnabled(true);
		if (ccImportUI->hasCCShader)
		{
			LW_CheckBox->SetEnabled(true);
			HQ_CheckBox->SetEnabled(true);
		}

		ccImportUI->isCCAutoSetup = true;
		CCAutoSetup_CheckBox.Get()->SetIsChecked(ECheckBoxState::Checked);
	}
	else
	{
		Standard_CheckBox->SetEnabled(false);
		LW_CheckBox->SetEnabled(false);
		HQ_CheckBox->SetEnabled(false);
		ccImportUI->isCCAutoSetup = false;
		CCAutoSetup_CheckBox.Get()->SetIsChecked(ECheckBoxState::Unchecked);
	}
}

void SCCImportWindow::OnStandardChanged(ECheckBoxState InCheckedState)
{	
	ccImportUI->isHQSkin=false;
	HQ_CheckBox.Get()->SetIsChecked(ECheckBoxState::Unchecked);
	ccImportUI->isLWSkin = false;
	LW_CheckBox.Get()->SetIsChecked(ECheckBoxState::Unchecked);
	ccImportUI->isStandardSkin = true;
	Standard_CheckBox.Get()->SetIsChecked(ECheckBoxState::Checked);	
}
void SCCImportWindow::OnHQChanged(ECheckBoxState InCheckedState)
{		
	ccImportUI->isHQSkin = true;
	HQ_CheckBox.Get()->SetIsChecked(ECheckBoxState::Checked);
	ccImportUI->isLWSkin = false;
	LW_CheckBox.Get()->SetIsChecked(ECheckBoxState::Unchecked);
	ccImportUI->isStandardSkin = false;
	Standard_CheckBox.Get()->SetIsChecked(ECheckBoxState::Unchecked);	
}
void SCCImportWindow::OnLWChanged(ECheckBoxState InCheckedState)
{	
	ccImportUI->isHQSkin = false;
	HQ_CheckBox.Get()->SetIsChecked(ECheckBoxState::Unchecked);
	ccImportUI->isLWSkin = true;
	LW_CheckBox.Get()->SetIsChecked(ECheckBoxState::Checked);
	ccImportUI->isStandardSkin = false;
	Standard_CheckBox.Get()->SetIsChecked(ECheckBoxState::Unchecked);	
}
FReply SCCImportWindow::OnCancel()
{
	ccImportUI->isCanceled = true;	
	if (WidgetWindow.IsValid())
	{
		WidgetWindow.Pin()->RequestDestroyWindow();
	}
	return FReply::Handled();
}
FReply SCCImportWindow::OnNext()
{
	ccImportUI->isCanceled = false;
	if (WidgetWindow.IsValid())
	{
		WidgetWindow.Pin()->RequestDestroyWindow();
	}
	return FReply::Handled();
}

FReply SCCImportWindow::OnMore()
{    
    FPlatformProcess::LaunchURL(TEXT("https://www.reallusion.com/linkcount/linkcount.aspx?lid=CC3WBASIC_300_030_"), NULL, NULL);
    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE