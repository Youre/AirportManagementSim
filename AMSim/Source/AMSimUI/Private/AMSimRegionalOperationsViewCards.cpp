#include "AMSimRegionalOperationsView.h"

#include "AMSimTimetableGeometry.h"
#include "AMSimUITheme.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"

namespace AMSimRegionalOperationsViewCardsPrivate
{
	FString MinuteDisplay(const int32 Minute)
	{
		return FString::Printf(
			TEXT("%02d:%02d"),
			(Minute / 60) % 24,
			Minute % 60);
	}

	FString AircraftShortLabel(const FString& DisplayName)
	{
		if (DisplayName.Contains(TEXT("Trailwing")))
		{
			return TEXT("TW42");
		}
		if (DisplayName.Contains(TEXT("Skylark")))
		{
			return TEXT("SL72");
		}
		if (DisplayName.Contains(TEXT("Harborliner")))
		{
			return TEXT("HL126");
		}
		return DisplayName.Left(6).ToUpper();
	}

	FString OperatorShortLabel(const FString& DisplayName)
	{
		if (DisplayName.Contains(TEXT("Riverbend")))
		{
			return TEXT("RIVERBEND");
		}
		if (DisplayName.Contains(TEXT("Northstar")))
		{
			return TEXT("NORTHSTAR");
		}
		if (DisplayName.Contains(TEXT("Coastal")))
		{
			return TEXT("COASTAL");
		}
		return DisplayName.Left(9).ToUpper();
	}
}

using namespace AMSimRegionalOperationsViewCardsPrivate;

void UAMSimRegionalOperationsView::RefreshContractCards(
	const AMSim::FPhase4State& State)
{
	int32 SelectedIndex = INDEX_NONE;
	for (int32 Index = 0; Index < State.Contracts.Num(); ++Index)
	{
		if (!State.Contracts[Index].bAccepted)
		{
			SelectedIndex = Index;
			break;
		}
	}
	if (SelectedIndex == INDEX_NONE && !State.Contracts.IsEmpty())
	{
		SelectedIndex = 0;
	}

	for (int32 Index = 0; Index < ContractCards.Num(); ++Index)
	{
		if (!State.Contracts.IsValidIndex(Index))
		{
			ContractCards[Index]->SetVisibility(ESlateVisibility::Collapsed);
			continue;
		}
		const AMSim::FPhase4ContractRecord& Contract = State.Contracts[Index];
		const AMSim::FPhase4FlightRecord* RepresentativeFlight =
			State.Flights.FindByPredicate(
				[&Contract](const AMSim::FPhase4FlightRecord& Flight)
				{
					return Flight.ContractId == Contract.Id;
				});
		const FString Route = RepresentativeFlight
			? FString::Printf(
				TEXT("%s > %s"),
				*RepresentativeFlight->OriginRegion.ToString().ToUpper(),
				*RepresentativeFlight->DestinationRegion.ToString().ToUpper())
			: TEXT("ROUTE ASSIGNED ON ACCEPTANCE");
		const bool bSelected = Index == SelectedIndex;
		ContractCards[Index]->SetVisibility(ESlateVisibility::Visible);
		ContractCardTexts[Index]->SetText(FText::FromString(FString::Printf(
			TEXT("%s%s\n%s\n%s / %d SEATS\n%d/WK / %lld CR / %s"),
			bSelected ? TEXT("SELECTED / ") : TEXT(""),
			*Contract.OperatorDisplayName.ToUpper(),
			*Route,
			*Contract.AircraftDisplayName.ToUpper(),
			Contract.PassengerCapacity,
			Contract.FrequencyPerWeek,
			Contract.RewardPerCompletedFlightCredits,
			Contract.bAccepted ? TEXT("ACCEPTED") : TEXT("AVAILABLE"))));
		AMSim::UITheme::StyleSurface(
			ContractCards[Index],
			Contract.bAccepted
				? AMSim::UITheme::ESurface::Positive
				: bSelected
					? AMSim::UITheme::ESurface::RaisedCard
					: AMSim::UITheme::ESurface::Card,
			FMargin(9.0f),
			12.0f,
			bSelected ? 2.0f : 1.4f);
	}
}

void UAMSimRegionalOperationsView::RefreshFlightCards(
	const AMSim::FPhase4State& State)
{
	const int32 VisibleDay = FMath::Clamp(
		State.CurrentOperatingDay > 0 ? State.CurrentOperatingDay : 1,
		1,
		7);
	for (int32 Index = 0; Index < FlightCards.Num(); ++Index)
	{
		if (!State.Flights.IsValidIndex(Index))
		{
			FlightCards[Index]->SetVisibility(ESlateVisibility::Collapsed);
			continue;
		}
		const AMSim::FPhase4FlightRecord& Flight = State.Flights[Index];
		const bool bVisible = !bCompactLayout || Flight.DayIndex == VisibleDay;
		FlightCards[Index]->SetVisibility(
			bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		if (!bVisible)
		{
			continue;
		}
		if (FlightCardSlots.IsValidIndex(Index))
		{
			const AMSim::FTimetableCardGeometry Geometry =
				AMSim::FTimetableGeometry::MakeCard(
					Flight.DayIndex,
					Flight.PlannedArrivalMinute,
					Flight.PlannedDepartureMinute,
					bCompactLayout);
			FlightCardSlots[Index]->SetAnchors(FAnchors(
				Geometry.Left,
				Geometry.Top,
				Geometry.Right,
				Geometry.Bottom));
			FlightCardSlots[Index]->SetOffsets(FMargin(0.0f));
		}
		const AMSim::FPhase4FlightCardViewState CardView =
			AMSim::MakePhase4FlightCardViewState(Flight);
		const FString CardText = FString::Printf(
			TEXT("%s / %s\n%s-%s / %s\n%s / %s"),
			*Flight.FlightCode.ToString(),
			*OperatorShortLabel(Flight.OperatorDisplayName),
			*MinuteDisplay(Flight.PlannedArrivalMinute),
			*MinuteDisplay(Flight.PlannedDepartureMinute),
			*Flight.AssignedGateId.ToString(),
			*AircraftShortLabel(Flight.AircraftDisplayName),
			*CardView.Status);
		FlightCardTexts[Index]->SetText(FText::FromString(CardText));
		const AMSim::UITheme::ESurface Surface =
			CardView.bWarning
				? AMSim::UITheme::ESurface::Warning
				: CardView.bPositive
					? AMSim::UITheme::ESurface::Positive
					: AMSim::UITheme::ESurface::Card;
		AMSim::UITheme::StyleSurface(
			FlightCards[Index],
			Surface,
			FMargin(bCompactLayout ? 5.0f : 6.0f),
			10.0f,
			1.4f);
	}
	for (int32 Day = 0; Day < DayChips.Num(); ++Day)
	{
		const bool bActive = Day + 1 == VisibleDay;
		DayChips[Day]->SetVisibility(
			!bCompactLayout || bActive
				? ESlateVisibility::Visible
				: ESlateVisibility::Collapsed);
		AMSim::UITheme::StyleSurface(
			DayChips[Day],
			bActive
				? AMSim::UITheme::ESurface::RaisedCard
				: AMSim::UITheme::ESurface::Chip,
			FMargin(5.0f),
			8.0f);
		if (DayChipTexts.IsValidIndex(Day))
		{
			DayChipTexts[Day]->SetColorAndOpacity(
				bActive ? AMSim::UITheme::Cyan() : AMSim::UITheme::Muted());
		}
	}

	int32 WeatherDay = 0;
	int32 WeatherStart = MAX_int32;
	int32 WeatherEnd = 0;
	TArray<FString> WeatherFlights;
	bool bLockedHorizonRisk = false;
	for (const AMSim::FPhase4FlightRecord& Flight : State.Flights)
	{
		if (!Flight.bWeatherRestricted)
		{
			continue;
		}
		if (WeatherDay == 0)
		{
			WeatherDay = Flight.DayIndex;
		}
		if (Flight.DayIndex != WeatherDay)
		{
			continue;
		}
		WeatherStart = FMath::Min(WeatherStart, Flight.PlannedArrivalMinute);
		WeatherEnd = FMath::Max(WeatherEnd, Flight.PlannedDepartureMinute);
		WeatherFlights.Add(Flight.FlightCode.ToString());
		bLockedHorizonRisk |=
			Flight.bGateChanged && Flight.bOverrideRecorded;
	}
	const bool bShowWeatherWindow =
		WeatherDay > 0 &&
		WeatherStart < MAX_int32 &&
		(!bCompactLayout || WeatherDay == VisibleDay);
	if (WeatherWindowOverlay && WeatherWindowSlot && WeatherWindowText)
	{
		WeatherWindowOverlay->SetVisibility(
			bShowWeatherWindow
				? ESlateVisibility::HitTestInvisible
				: ESlateVisibility::Collapsed);
		if (bShowWeatherWindow)
		{
			const AMSim::FTimetableCardGeometry Geometry =
				AMSim::FTimetableGeometry::MakeCard(
					WeatherDay,
					FMath::Max(6 * 60, WeatherStart - 60),
					FMath::Min(22 * 60, WeatherEnd + 30),
					bCompactLayout);
			WeatherWindowSlot->SetAnchors(FAnchors(
				Geometry.Left,
				Geometry.Top,
				Geometry.Right,
				Geometry.Bottom));
			WeatherWindowSlot->SetOffsets(FMargin(0.0f));
			WeatherWindowText->SetText(FText::FromString(FString::Printf(
				TEXT("%s\nRAIN D%d / %s-%s / %s"),
				bLockedHorizonRisk
					? TEXT("LOCK RISK ///")
					: TEXT("WX CONFLICT ///"),
				WeatherDay,
				*MinuteDisplay(WeatherStart),
				*MinuteDisplay(WeatherEnd),
				*FString::Join(WeatherFlights, TEXT(" / ")))));
		}
	}
}
