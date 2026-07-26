#include "AMSimPhase3Fixture.h"
#include "AMSimDeterminism.h"

namespace AMSim
{
	namespace
	{
		constexpr uint64 Phase3IdBase = 1ull << 48;

		struct FRouteSeed
		{
			ETerminalRouteKind Kind;
			ETerminalZone From;
			ETerminalZone To;
			bool bControlled;
		};
	}

	const FPhase3Fixture& GetPhase3Fixture()
	{
		static const FPhase3Fixture Fixture;
		return Fixture;
	}

	FString TerminalStageDisplayName(const ETerminalConstructionStage Stage)
	{
		switch (Stage)
		{
		case ETerminalConstructionStage::Funded: return TEXT("Funded");
		case ETerminalConstructionStage::Building: return TEXT("Building");
		case ETerminalConstructionStage::Inspection: return TEXT("Inspection");
		case ETerminalConstructionStage::ShellReady: return TEXT("Connect flows");
		case ETerminalConstructionStage::Operational: return TEXT("Operational");
		default: return TEXT("Not built");
		}
	}

	FString PassengerJourneyDisplayName(const EPassengerJourneyState State)
	{
		switch (State)
		{
		case EPassengerJourneyState::ApproachingAirport:
			return TEXT("Approaching airport");
		case EPassengerJourneyState::LandsideEntry:
			return TEXT("Terminal entrance");
		case EPassengerJourneyState::CheckInBagDrop:
			return TEXT("Check-in & bag drop");
		case EPassengerJourneyState::SecurityQueue:
			return TEXT("Security queue");
		case EPassengerJourneyState::Screening:
			return TEXT("Security");
		case EPassengerJourneyState::GateArea:
			return TEXT("Gate area");
		case EPassengerJourneyState::Boarding:
			return TEXT("Boarding");
		case EPassengerJourneyState::OnAircraft:
			return TEXT("On aircraft");
		case EPassengerJourneyState::ArrivalsCorridor:
			return TEXT("Arrivals corridor");
		case EPassengerJourneyState::BaggageReclaim:
			return TEXT("Baggage reclaim");
		case EPassengerJourneyState::GroundTransport:
			return TEXT("Exit to curb");
		case EPassengerJourneyState::Completed:
			return TEXT("Journey complete");
		default:
			return TEXT("Waiting for a connected route");
		}
	}

	FString Phase3FlightDisplayName(const EPhase3FlightState State)
	{
		switch (State)
		{
		case EPhase3FlightState::Scheduled: return TEXT("Scheduled");
		case EPhase3FlightState::AtGate: return TEXT("At Gate A1");
		case EPhase3FlightState::Boarding: return TEXT("Boarding");
		case EPhase3FlightState::ReadyToDepart: return TEXT("Ready to depart");
		case EPhase3FlightState::Outbound: return TEXT("Outbound");
		case EPhase3FlightState::Completed: return TEXT("Complete");
		default: return TEXT("Not scheduled");
		}
	}

	FPhase3State BuildPhase3LogicalScaleFixture(const int32 PassengerCount)
	{
		FPhase3State State;
		State.bInitialized = true;
		State.MasterSeed = GetPhase3Fixture().Seed;
		State.RandomStreamState = FDeterministicStream::SeedNamedStream(
			State.MasterSeed,
			TEXT("Phase3.PassengerTerminal"));
		State.NextDomainId = Phase3IdBase;
		State.NextEventSequence = 1;
		State.TerminalStage = ETerminalConstructionStage::Operational;
		State.bTerminalOpen = true;
		State.bSecurityIntegrityValid = true;
		State.bAccessibleRouteValid = true;
		State.TerminalCostCredits = GetPhase3Fixture().TerminalCostCredits;

		auto Allocate = [&State]() { return State.NextDomainId++; };
		const int32 SafeCount = FMath::Clamp(PassengerCount, 0, 10000);
		const FRouteSeed RouteSeeds[] = {
			{ETerminalRouteKind::LandsideRoad, ETerminalZone::Outside, ETerminalZone::Landside, false},
			{ETerminalRouteKind::EntranceToCheckIn, ETerminalZone::Landside, ETerminalZone::Landside, false},
			{ETerminalRouteKind::SecurityControlled, ETerminalZone::Landside, ETerminalZone::SterileDepartures, true},
			{ETerminalRouteKind::SterileGate, ETerminalZone::SterileDepartures, ETerminalZone::SterileDepartures, false},
			{ETerminalRouteKind::ArrivalsToCurb, ETerminalZone::SterileArrivals, ETerminalZone::Landside, true},
			{ETerminalRouteKind::BaggageOutbound, ETerminalZone::Landside, ETerminalZone::RestrictedBaggage, true},
			{ETerminalRouteKind::BaggageArrival, ETerminalZone::RestrictedBaggage, ETerminalZone::Landside, true}
		};
		for (const FRouteSeed& Route : RouteSeeds)
		{
			FTerminalRouteRecord Record;
			Record.Id = {Allocate()};
			Record.Kind = Route.Kind;
			Record.DefinitionId = *FString::Printf(
				TEXT("Network.Phase3.Scale.%d"),
				static_cast<int32>(Record.Kind));
			Record.FromZone = Route.From;
			Record.ToZone = Route.To;
			Record.bConnected = true;
			Record.bControlledTransition = Route.bControlled;
			Record.bAccessible = true;
			Record.Capacity = FMath::Max(1, SafeCount);
			State.Routes.Add(MoveTemp(Record));
		}
		State.Checkpoint.Id = {Allocate()};
		State.Checkpoint.DefinitionId = TEXT("Facility.Security.Domestic");
		State.Checkpoint.TeamId = {Allocate()};
		State.Checkpoint.bOpen = true;
		State.Checkpoint.bAccessibleLaneOpen = true;
		State.Checkpoint.QueueCapacity = FMath::Max(1, SafeCount);
		State.Checkpoint.ThroughputPerBucket = FMath::Max(1, SafeCount);
		FPhase3StaffTeamRecord SecurityTeam;
		SecurityTeam.Id = State.Checkpoint.TeamId;
		SecurityTeam.RoleId = TEXT("StaffRole.SecurityOfficer");
		SecurityTeam.ZoneId = TEXT("Zone.DomesticSecurity");
		SecurityTeam.TeamSize = FMath::Max(2, PassengerCount / 1000);
		SecurityTeam.WorkloadPercent = 40;
		State.Teams.Add(MoveTemp(SecurityTeam));

		State.Flight.Id = {Allocate()};
		State.Flight.FlightCode = TEXT("RB 304");
		State.Flight.OperatorContentId = TEXT("Operator.RiverbendConnect");
		State.Flight.AircraftContentId = TEXT("Aircraft.RegionalPassenger.Phase3");
		State.Flight.GateId = TEXT("Gate.A1");
		State.Flight.State = EPhase3FlightState::AtGate;
		State.Flight.DepartingPassengerCount = SafeCount;
		State.Parties.Reserve((SafeCount + 3) / 4);
		State.Passengers.Reserve(SafeCount);
		for (int32 Index = 0; Index < SafeCount; ++Index)
		{
			if (Index % 4 == 0)
			{
				FPassengerPartyRecord Party;
				Party.Id = {Allocate()};
				Party.DisplayName = FString::Printf(TEXT("Scale party %d"), Index / 4 + 1);
				Party.ArrivalMode = static_cast<ELandsideMode>((Index / 4) % 3);
				Party.TimeConfidencePercent = 88;
				Party.CompactNeeds = TEXT("Information · Restroom");
				State.Parties.Add(MoveTemp(Party));
			}
			FPassengerRecord Passenger;
			Passenger.Id = {Allocate()};
			Passenger.PartyId = State.Parties.Last().Id;
			Passenger.FlightId = State.Flight.Id;
			Passenger.DisplayName = FString::Printf(TEXT("Passenger %d"), Index + 1);
			Passenger.AgeBand = Index % 5 == 0 ? TEXT("Child") : TEXT("Adult");
			Passenger.Direction = EPassengerDirection::Departing;
			Passenger.JourneyState = EPassengerJourneyState::GateArea;
			Passenger.SecurityState = ESecurityClearanceState::Cleared;
			Passenger.LandsideMode = State.Parties.Last().ArrivalMode;
			Passenger.TimeConfidencePercent = 88;
			State.Parties.Last().Members.Add(Passenger.Id);
			State.Passengers.Add(MoveTemp(Passenger));
		}
		return State;
	}
}
