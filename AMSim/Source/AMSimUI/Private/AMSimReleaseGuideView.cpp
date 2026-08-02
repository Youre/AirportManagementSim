#include "AMSimReleaseGuideView.h"

#include "AMSimAccessibilityProfile.h"
#include "AMSimContextHelp.h"
#include "AMSimHUD.h"
#include "AMSimUITheme.h"
#include "AMSimUISoundSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/UserInterfaceSettings.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

namespace AMSimReleaseGuidePrivate
{
	using namespace AMSim::UITheme;

	UTextBlock* Text(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const FString& Copy,
		const int32 Size,
		const FLinearColor& Color,
		const bool bBold = false)
	{
		UTextBlock* Result =
			Tree->ConstructWidget<UTextBlock>(
				UTextBlock::StaticClass(),
				Name);
		Result->SetText(FText::FromString(Copy));
		Result->SetAutoWrapText(true);
		Result->SetLineHeightPercentage(1.0f);
		StyleText(
			Result,
			Size,
			Color,
			bBold,
			true);
		return Result;
	}

	UBorder* Surface(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const ESurface Kind,
		const FMargin Padding,
		const float Radius = 18.0f)
	{
		UBorder* Result =
			Tree->ConstructWidget<UBorder>(
				UBorder::StaticClass(),
				Name);
		StyleSurface(
			Result,
			Kind,
			Padding,
			Radius,
			1.6f);
		return Result;
	}

	UButton* Button(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const FString& Label,
		const EButton Kind)
	{
		UButton* Result =
			Tree->ConstructWidget<UButton>(
				UButton::StaticClass(),
				Name);
		Result->SetStyle(ButtonStyle(Kind, 11.0f));
		UTextBlock* LabelText = Text(
			Tree,
			*FString::Printf(TEXT("%sLabel"), Name),
			Label,
			11,
			White(),
			true);
		LabelText->SetJustification(ETextJustify::Center);
		Result->SetContent(LabelText);
		return Result;
	}

	void AddVertical(
		UVerticalBox* Box,
		UWidget* Child,
		const float Bottom = 8.0f,
		const bool bFill = false)
	{
		UVerticalBoxSlot* Slot =
			Box->AddChildToVerticalBox(Child);
		Slot->SetPadding(
			FMargin(0.0f, 0.0f, 0.0f, Bottom));
		Slot->SetSize(FSlateChildSize(
			bFill
				? ESlateSizeRule::Fill
				: ESlateSizeRule::Automatic));
	}

	void AddHorizontal(
		UHorizontalBox* Box,
		UWidget* Child,
		const float Right = 7.0f,
		const bool bFill = false)
	{
		UHorizontalBoxSlot* Slot =
			Box->AddChildToHorizontalBox(Child);
		Slot->SetPadding(
			FMargin(0.0f, 0.0f, Right, 0.0f));
		Slot->SetSize(FSlateChildSize(
			bFill
				? ESlateSizeRule::Fill
				: ESlateSizeRule::Automatic));
		Slot->SetVerticalAlignment(VAlign_Center);
	}
}

TSharedRef<SWidget> UAMSimReleaseGuideView::RebuildWidget()
{
	using namespace AMSimReleaseGuidePrivate;
	using namespace AMSim::UITheme;
	if (!WidgetTree)
	{
		return Super::RebuildWidget();
	}
	UCanvasPanel* Root =
		WidgetTree->ConstructWidget<UCanvasPanel>(
			UCanvasPanel::StaticClass(),
			TEXT("ReleaseGuideRoot"));
	WidgetTree->RootWidget = Root;

	UBorder* Dimmer =
		WidgetTree->ConstructWidget<UBorder>(
			UBorder::StaticClass(),
			TEXT("ReleaseGuideDimmer"));
	Dimmer->SetBrushColor(FLinearColor(
		0.008f,
		0.016f,
		0.035f,
		0.94f));
	UCanvasPanelSlot* DimmerSlot =
		Root->AddChildToCanvas(Dimmer);
	DimmerSlot->SetAnchors(FAnchors(0, 0, 1, 1));
	DimmerSlot->SetOffsets(FMargin(0));

	ModalSurface = Surface(
		WidgetTree,
		TEXT("ReleaseGuideModal"),
		ESurface::Chrome,
		FMargin(20.0f, 16.0f),
		22.0f);
	UCanvasPanelSlot* ModalSlot =
		Root->AddChildToCanvas(ModalSurface);
	ModalSlot->SetAnchors(
		FAnchors(0.025f, 0.035f, 0.975f, 0.965f));
	ModalSlot->SetOffsets(FMargin(0));
	ModalSlot->SetZOrder(3);

	UVerticalBox* Page =
		WidgetTree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(),
			TEXT("ReleaseGuidePage"));
	ModalSurface->SetContent(Page);

	UHorizontalBox* Header =
		WidgetTree->ConstructWidget<UHorizontalBox>(
			UHorizontalBox::StaticClass(),
			TEXT("ReleaseGuideHeader"));
	UVerticalBox* Identity =
		WidgetTree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(),
			TEXT("ReleaseGuideIdentity"));
	AddVertical(
		Identity,
		Text(
			WidgetTree,
			TEXT("ReleaseGuideBrand"),
			TEXT("RIVERBEND AIRPORT HANDBOOK"),
			10,
			Cyan(),
			true),
		2.0f);
	AddVertical(
		Identity,
		Text(
			WidgetTree,
			TEXT("ReleaseGuideTitle"),
			TEXT("Help that stays beside the airport"),
			24,
			White(),
			true),
		0.0f);
	AddHorizontal(Header, Identity, 14.0f, true);
	UButton* CloseButton = Button(
		WidgetTree,
		TEXT("ReleaseGuideClose"),
		TEXT("CLOSE"),
		EButton::Secondary);
	CloseButton->OnClicked.AddDynamic(
		this,
		&UAMSimReleaseGuideView::ClosePressed);
	AddHorizontal(Header, CloseButton, 0.0f);
	AddVertical(Page, Header, 12.0f);

	UScrollBox* TabScroll =
		WidgetTree->ConstructWidget<UScrollBox>(
			UScrollBox::StaticClass(),
			TEXT("ReleaseGuideTabScroll"));
	TabScroll->SetOrientation(Orient_Horizontal);
	TabScroll->SetScrollBarVisibility(
		ESlateVisibility::Hidden);
	UHorizontalBox* Tabs =
		WidgetTree->ConstructWidget<UHorizontalBox>(
			UHorizontalBox::StaticClass(),
			TEXT("ReleaseGuideTabs"));
	TabScroll->AddChild(Tabs);
	struct FTab
	{
		const TCHAR* Name;
		const TCHAR* Label;
		FName Handler;
	};
	const FTab TabDefinitions[] = {
		{TEXT("GuideStart"), TEXT("01  START"), TEXT("ShowStart")},
		{TEXT("GuideControls"), TEXT("02  CONTROLS"), TEXT("ShowControls")},
		{TEXT("GuideAirport"), TEXT("03  AIRPORT"), TEXT("ShowAirport")},
		{TEXT("GuideOperations"), TEXT("04  OPERATIONS"), TEXT("ShowOperations")},
		{TEXT("GuideAccessibility"), TEXT("05  ACCESSIBILITY"), TEXT("ShowAccessibility")}
	};
	for (const FTab& Tab : TabDefinitions)
	{
		UButton* TabButton = Button(
			WidgetTree,
			Tab.Name,
			Tab.Label,
			EButton::Tool);
		FScriptDelegate Delegate;
		Delegate.BindUFunction(this, Tab.Handler);
		TabButton->OnClicked.Add(Delegate);
		AddHorizontal(Tabs, TabButton, 6.0f, true);
	}
	AddVertical(Page, TabScroll, 13.0f);

	UHorizontalBox* Body =
		WidgetTree->ConstructWidget<UHorizontalBox>(
			UHorizontalBox::StaticClass(),
			TEXT("ReleaseGuideBody"));
	UBorder* ContentSurface = Surface(
		WidgetTree,
		TEXT("ReleaseGuideContentSurface"),
		ESurface::Panel,
		FMargin(18.0f),
		18.0f);
	UVerticalBox* Content =
		WidgetTree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(),
			TEXT("ReleaseGuideContent"));
	ContentSurface->SetContent(Content);
	SectionEyebrowText = Text(
		WidgetTree,
		TEXT("ReleaseGuideSectionEyebrow"),
		TEXT("START"),
		10,
		Cyan(),
		true);
	AddVertical(Content, SectionEyebrowText, 3.0f);
	SectionHeadlineText = Text(
		WidgetTree,
		TEXT("ReleaseGuideSectionHeadline"),
		TEXT("Build your first airfield"),
		22,
		White(),
		true);
	AddVertical(Content, SectionHeadlineText, 5.0f);
	SectionIntroText = Text(
		WidgetTree,
		TEXT("ReleaseGuideSectionIntro"),
		TEXT("Choose what to explore. Nothing here blocks play."),
		14,
		Muted());
	AddVertical(Content, SectionIntroText, 10.0f);
	UScrollBox* ContentScroll =
		WidgetTree->ConstructWidget<UScrollBox>(
			UScrollBox::StaticClass(),
			TEXT("ReleaseGuideContentScroll"));
	ContentCards =
		WidgetTree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(),
			TEXT("ReleaseGuideContentCards"));
	ContentScroll->AddChild(ContentCards);
	AddVertical(Content, ContentScroll, 0.0f, true);
	AddHorizontal(Body, ContentSurface, 12.0f, true);

	const UUserInterfaceSettings* Settings =
		GetDefault<UUserInterfaceSettings>();
	const float Scale = Settings
		? Settings->GetDPIScaleBasedOnSize(
			FIntPoint(1920, 1080))
		: 1.0f;
	if (Scale < 1.75f)
	{
		UBorder* QuickSurface = Surface(
			WidgetTree,
			TEXT("ReleaseGuideQuickSurface"),
			ESurface::RaisedCard,
			FMargin(16.0f),
			18.0f);
		UVerticalBox* Quick =
			WidgetTree->ConstructWidget<UVerticalBox>(
				UVerticalBox::StaticClass(),
				TEXT("ReleaseGuideQuick"));
		QuickSurface->SetContent(Quick);
		AddVertical(
			Quick,
			Text(
				WidgetTree,
				TEXT("ReleaseGuideQuickTitle"),
				TEXT("READY BY DEFAULT"),
				10,
				Cyan(),
				true),
			7.0f);
		for (const FString& Line : {
			TEXT("CAPTIONS  ON"),
			TEXT("MOUSE CURSOR  VISIBLE"),
			TEXT("CAMERA SHAKE  OFF"),
			TEXT("OFFLINE  ALWAYS"),
			TEXT("STATUS  ICON + TEXT")})
		{
			AddVertical(
				Quick,
				Text(
					WidgetTree,
					*FString::Printf(
						TEXT("ReleaseGuideQuick%d"),
						Quick->GetChildrenCount()),
					Line,
					12,
					White(),
					true),
				8.0f);
		}
		AddVertical(
			Quick,
			Text(
				WidgetTree,
				TEXT("ReleaseGuideQuickNote"),
				TEXT("Authentic airport terms include a plain explanation. "
					"Warnings state the cause, consequence, and nearest action."),
				13,
				Muted()),
			0.0f,
			true);
		UHorizontalBoxSlot* QuickSlot =
			Body->AddChildToHorizontalBox(QuickSurface);
		QuickSlot->SetSize(
			FSlateChildSize(ESlateSizeRule::Fill));
		QuickSlot->SetPadding(FMargin(0.0f));
	}
	AddVertical(Page, Body, 8.0f, true);
	AddVertical(
		Page,
		Text(
			WidgetTree,
			TEXT("ReleaseGuideFooter"),
			TEXT("F1 / HELP  |  Captions describe every operational call  |  "
				"The game never requires an internet connection"),
			11,
			Muted(),
			true),
		0.0f);

	SetSection(0);
	bRequestedOpen =
		bRequestedOpen ||
		FParse::Param(
			FCommandLine::Get(),
			TEXT("AMSimReleaseGuide"));
	SetVisibility(
		bRequestedOpen
			? ESlateVisibility::Visible
			: ESlateVisibility::Collapsed);
	return Super::RebuildWidget();
}

void UAMSimReleaseGuideView::OpenGuide()
{
	const bool bWasOpen = IsGuideOpen();
	bRequestedOpen = true;
	SetVisibility(ESlateVisibility::Visible);
	SetSection(ActiveSection);
	if (!bWasOpen)
	{
		AMSim::UIAudio::Play(this, EAMSimUISound::PanelOpen);
	}
	if (APlayerController* Controller = GetOwningPlayer())
	{
		Controller->bShowMouseCursor = true;
	}
}

void UAMSimReleaseGuideView::CloseGuide()
{
	const bool bWasOpen = IsGuideOpen();
	bRequestedOpen = false;
	SetVisibility(ESlateVisibility::Collapsed);
	if (bWasOpen)
	{
		AMSim::UIAudio::Play(this, EAMSimUISound::PanelClose);
	}
}

bool UAMSimReleaseGuideView::IsGuideOpen() const
{
	return GetVisibility() != ESlateVisibility::Collapsed;
}

void UAMSimReleaseGuideView::ClosePressed()
{
	CloseGuide();
}

void UAMSimReleaseGuideView::ShowStart() { SetSection(0); }
void UAMSimReleaseGuideView::ShowControls() { SetSection(1); }
void UAMSimReleaseGuideView::ShowAirport() { SetSection(2); }
void UAMSimReleaseGuideView::ShowOperations() { SetSection(3); }
void UAMSimReleaseGuideView::ShowAccessibility() { SetSection(4); }

void UAMSimReleaseGuideView::SetSection(const int32 SectionIndex)
{
	const int32 PreviousSection = ActiveSection;
	if (!ContentCards || !SectionEyebrowText ||
		!SectionHeadlineText || !SectionIntroText)
	{
		ActiveSection = FMath::Clamp(SectionIndex, 0, 4);
		return;
	}
	using namespace AMSim::UITheme;
	ActiveSection = FMath::Clamp(SectionIndex, 0, 4);
	if (ActiveSection != PreviousSection && IsGuideOpen())
	{
		AMSim::UIAudio::Play(this, EAMSimUISound::TabSwitch);
	}
	ContentCards->ClearChildren();
	const FString Eyebrows[] = {
		TEXT("START"),
		TEXT("CONTROLS"),
		TEXT("AIRPORT TERMS"),
		TEXT("OPERATIONS"),
		TEXT("ACCESSIBILITY")
	};
	const FString Headlines[] = {
		TEXT("Your first safe aircraft visit"),
		TEXT("Move quickly without losing the mouse"),
		TEXT("Authentic words, explained in place"),
		TEXT("Read the airport before you act"),
		TEXT("Comfortable at every supported scale")
	};
	const FString Intros[] = {
		TEXT("Six optional steps lead from an empty parcel to a complete "
			"turnaround. You can close this panel at any time."),
		TEXT("Controls are semantic and remappable. Clicking the world keeps "
			"the Windows cursor visible."),
		TEXT("The game keeps real aviation terms and gives each one a short "
			"plain-language meaning."),
		TEXT("Automatic systems do the driving. You manage priorities, "
			"capacity, schedules, readiness, and recovery."),
		TEXT("Text stays readable and operational state never depends on color alone.")
	};
	SectionEyebrowText->SetText(
		FText::FromString(Eyebrows[ActiveSection]));
	SectionHeadlineText->SetText(
		FText::FromString(Headlines[ActiveSection]));
	SectionIntroText->SetText(
		FText::FromString(Intros[ActiveSection]));

	if (ActiveSection == 0)
	{
		for (const AMSim::FContextHelpDefinition& Help :
			AMSim::GetPhase1ContextHelpCatalog())
		{
			AddContentCard(
				Help.Eyebrow,
				Help.Title,
				Help.Body + TEXT("\nNEXT  ") +
					Help.NextAction,
				Cyan());
		}
	}
	else if (ActiveSection == 1)
	{
		AddContentCard(
			TEXT("WORLD"),
			TEXT("Pan, zoom, select"),
			TEXT("WASD or middle-mouse drag pans. The wheel zooms toward "
				"the pointer. Click selects; repeated clicks cycle overlaps. "
				"Focus centers the selected item."),
			Cyan());
		AddContentCard(
			TEXT("TIME"),
			TEXT("Pause, 1x, 2x, 4x, 8x"),
			TEXT("Planning works while paused. Commands apply in stable order. "
				"Critical incidents slow fast time so warnings stay readable."),
			Amber());
		AddContentCard(
			TEXT("PANELS"),
			TEXT("One main task at a time"),
			TEXT("Build, schedules, tenants, staff, overlays, alerts, and Help "
				"use labeled tools. Escape closes a modal or ends follow mode."),
			Green());
	}
	else if (ActiveSection == 2)
	{
		AddContentCard(
			TEXT("AIRSIDE"),
			TEXT("Runway • taxiway • stand • gate"),
			TEXT("A runway handles takeoff and landing. A taxiway connects it "
				"to a stand. A stand parks and services aircraft. A gate links "
				"a passenger area to its stand."),
			Cyan());
		AddContentCard(
			TEXT("READINESS"),
			TEXT("Compatibility"),
			TEXT("Compatibility asks whether the runway, route, stand, weather, "
				"staff, and required services fit a specific aircraft."),
			Green());
		AddContentCard(
			TEXT("GROWTH"),
			TEXT("Capability is not a class"),
			TEXT("Established, Regional, Advanced, and Major describe proven "
				"capability. GA, school, charter, cargo, passenger, and mixed "
				"paths can be combined."),
			Amber());
	}
	else if (ActiveSection == 3)
	{
		AddContentCard(
			TEXT("FLIGHTS"),
			TEXT("Schedule exact time and space"),
			TEXT("Timetable cards reserve a runway, stand or gate, services, "
				"and connection time. Patterned warnings mark the affected "
				"flight and explain a remedy."),
			Cyan());
		AddContentCard(
			TEXT("TURNAROUND"),
			TEXT("Automatic dispatch"),
			TEXT("Teams and vehicles perform compatible work automatically. "
				"You set priority or safe assignment; you do not manually drive."),
			Green());
		AddContentCard(
			TEXT("RECOVERY"),
			TEXT("Warnings stay recoverable"),
			TEXT("Weather and incidents identify the affected area, cause, "
				"consequence, response, and reopening path. Other safe "
				"operations remain available."),
			Amber());
	}
	else
	{
		AddContentCard(
			TEXT("APPLICATION SCALE"),
			TEXT("Choose a comfortable size"),
			TEXT("100-150% keeps normal rails. 175-200% uses compact drawers "
				"instead of shrinking text or covering the airport."),
			Cyan());
		UHorizontalBox* ScaleRow =
			WidgetTree->ConstructWidget<UHorizontalBox>(
				UHorizontalBox::StaticClass(),
				TEXT("ReleaseGuideScaleRow"));
		struct FScaleButton
		{
			const TCHAR* Name;
			const TCHAR* Label;
			FName Handler;
		};
		const FScaleButton Scales[] = {
			{TEXT("Scale100"), TEXT("100%"), TEXT("Scale100")},
			{TEXT("Scale125"), TEXT("125%"), TEXT("Scale125")},
			{TEXT("Scale150"), TEXT("150%"), TEXT("Scale150")},
			{TEXT("Scale175"), TEXT("175%"), TEXT("Scale175")},
			{TEXT("Scale200"), TEXT("200%"), TEXT("Scale200")}
		};
		for (const FScaleButton& Scale : Scales)
		{
			UButton* ScaleButton =
				AMSimReleaseGuidePrivate::Button(
					WidgetTree,
					Scale.Name,
					Scale.Label,
					EButton::Tool);
			FScriptDelegate Delegate;
			Delegate.BindUFunction(this, Scale.Handler);
			ScaleButton->OnClicked.Add(Delegate);
			AMSimReleaseGuidePrivate::AddHorizontal(
				ScaleRow,
				ScaleButton,
				5.0f,
				true);
		}
		AMSimReleaseGuidePrivate::AddVertical(
			ContentCards,
			ScaleRow,
			10.0f);
		AccessibilitySummaryText =
			AMSimReleaseGuidePrivate::Text(
				WidgetTree,
				TEXT("ReleaseGuideAccessibilitySummary"),
				TEXT(""),
				13,
				White(),
				true);
		AMSimReleaseGuidePrivate::AddVertical(
			ContentCards,
			AccessibilitySummaryText,
			8.0f);
		ReducedMotionButton =
			AMSimReleaseGuidePrivate::Button(
				WidgetTree,
				TEXT("ReleaseGuideReducedMotion"),
				TEXT("REDUCED MOTION"),
				EButton::Secondary);
		ReducedMotionButton->OnClicked.AddDynamic(
			this,
			&UAMSimReleaseGuideView::ToggleReducedMotion);
		AMSimReleaseGuidePrivate::AddVertical(
			ContentCards,
			ReducedMotionButton,
			7.0f);
		UButton* ResetButton =
			AMSimReleaseGuidePrivate::Button(
				WidgetTree,
				TEXT("ReleaseGuideReset"),
				TEXT("RESTORE ACCESSIBLE DEFAULTS"),
				EButton::Quiet);
		ResetButton->OnClicked.AddDynamic(
			this,
			&UAMSimReleaseGuideView::ResetAccessibility);
		AMSimReleaseGuidePrivate::AddVertical(
			ContentCards,
			ResetButton,
			0.0f);
		RefreshAccessibilitySummary();
	}
}

void UAMSimReleaseGuideView::AddContentCard(
	const FString& Eyebrow,
	const FString& Headline,
	const FString& Body,
	const FLinearColor& Accent)
{
	using namespace AMSimReleaseGuidePrivate;
	using namespace AMSim::UITheme;
	if (!ContentCards)
	{
		return;
	}
	UBorder* Card = Surface(
		WidgetTree,
		*FString::Printf(
			TEXT("ReleaseGuideCard%d"),
			ContentCards->GetChildrenCount()),
		ESurface::Card,
		FMargin(15.0f, 12.0f),
		15.0f);
	UVerticalBox* Column =
		WidgetTree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(),
			*FString::Printf(
				TEXT("ReleaseGuideCardColumn%d"),
				ContentCards->GetChildrenCount()));
	Card->SetContent(Column);
	AddVertical(
		Column,
		Text(
			WidgetTree,
			*FString::Printf(
				TEXT("ReleaseGuideCardEyebrow%d"),
				ContentCards->GetChildrenCount()),
			Eyebrow,
			9,
			Accent,
			true),
		2.0f);
	AddVertical(
		Column,
		Text(
			WidgetTree,
			*FString::Printf(
				TEXT("ReleaseGuideCardHeadline%d"),
				ContentCards->GetChildrenCount()),
			Headline,
			16,
			White(),
			true),
		4.0f);
	AddVertical(
		Column,
		Text(
			WidgetTree,
			*FString::Printf(
				TEXT("ReleaseGuideCardBody%d"),
				ContentCards->GetChildrenCount()),
			Body,
			13,
			White()),
		0.0f);
	AddVertical(ContentCards, Card, 8.0f);
}

void UAMSimReleaseGuideView::ApplyScale(const float Scale)
{
	if (APlayerController* Controller = GetOwningPlayer())
	{
		if (AAMSimHUD* Hud =
			Cast<AAMSimHUD>(Controller->GetHUD()))
		{
			Hud->ApplyInterfaceScale(Scale);
		}
	}
}

void UAMSimReleaseGuideView::Scale100() { ApplyScale(1.0f); }
void UAMSimReleaseGuideView::Scale125() { ApplyScale(1.25f); }
void UAMSimReleaseGuideView::Scale150() { ApplyScale(1.5f); }
void UAMSimReleaseGuideView::Scale175() { ApplyScale(1.75f); }
void UAMSimReleaseGuideView::Scale200() { ApplyScale(2.0f); }

void UAMSimReleaseGuideView::ToggleReducedMotion()
{
	UAMSimAccessibilityProfile::Get()->ToggleReducedMotion();
	RefreshAccessibilitySummary();
}

void UAMSimReleaseGuideView::ResetAccessibility()
{
	UAMSimAccessibilityProfile::Get()->ResetToDefaults();
	ApplyScale(1.0f);
}

void UAMSimReleaseGuideView::RefreshAccessibilitySummary()
{
	if (!AccessibilitySummaryText)
	{
		return;
	}
	const UAMSimAccessibilityProfile* Profile =
		UAMSimAccessibilityProfile::Get();
	AccessibilitySummaryText->SetText(FText::FromString(
		FString::Printf(
			TEXT("SCALE  %d%%\nCAPTIONS  ON\nREDUCED MOTION  %s\n"
				"CAMERA SHAKE  OFF\nSTATUS  ICON + PATTERN + TEXT"),
			FMath::RoundToInt(
				Profile->GetInterfaceScale() * 100.0f),
			Profile->IsReducedMotionEnabled()
				? TEXT("ON")
				: TEXT("OFF"))));
	if (ReducedMotionButton)
	{
		if (UTextBlock* Label =
			Cast<UTextBlock>(
				ReducedMotionButton->GetContent()))
		{
			Label->SetText(FText::FromString(
				Profile->IsReducedMotionEnabled()
					? TEXT("REDUCED MOTION  ON")
					: TEXT("REDUCED MOTION  OFF")));
		}
	}
}
