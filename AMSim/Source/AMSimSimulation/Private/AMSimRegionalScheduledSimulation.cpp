#include "AMSimRegionalScheduledSimulation.h"

#include "AMSimDeterminism.h"
#include "AMSimStarterAirfieldSimulation.h"
#include "Algo/Count.h"

namespace AMSim
{
	namespace
	{
		constexpr uint64 Phase4IdBase = 1ull << 56;

		void HashBytes(uint64& Hash, const void* Data, const SIZE_T Size)
		{
			const uint8* Bytes = static_cast<const uint8*>(Data);
			for (SIZE_T Index = 0; Index < Size; ++Index)
			{
				Hash ^= Bytes[Index];
				Hash *= 1099511628211ull;
			}
		}

		template <typename T>
		void HashValue(uint64& Hash, const T& Value)
		{
			HashBytes(Hash, &Value, sizeof(T));
		}

		void HashString(uint64& Hash, const FString& Value)
		{
			HashValue(Hash, Value.Len());
			HashBytes(Hash, *Value, Value.Len() * sizeof(TCHAR));
		}

		void HashName(uint64& Hash, const FName Value)
		{
			HashString(Hash, Value.ToString());
		}

		const FPhase4FlightRecord* FindFlight(
			const FPhase4State& State,
			const FFlightId Id)
		{
			return State.Flights.FindByPredicate(
				[Id](const FPhase4FlightRecord& Flight)
				{
					return Flight.Id == Id;
				});
		}

		FPhase4FlightRecord* FindFlight(
			FPhase4State& State,
			const FFlightId Id)
		{
			return State.Flights.FindByPredicate(
				[Id](const FPhase4FlightRecord& Flight)
				{
					return Flight.Id == Id;
				});
		}

		FPhase4GateRecord* FindGate(FPhase4State& State, const FName GateId)
		{
			return State.Gates.FindByPredicate(
				[GateId](const FPhase4GateRecord& Gate)
				{
					return Gate.GateId == GateId;
				});
		}

		FString MinuteDisplay(const int32 Minute)
		{
			const int32 SafeMinute = FMath::Max(0, Minute);
			return FString::Printf(
				TEXT("%02d:%02d"),
				(SafeMinute / 60) % 24,
				SafeMinute % 60);
		}
	}

	FRegionalScheduledSimulation::FRegionalScheduledSimulation(
		const uint64 MasterSeed)
	{
		State.MasterSeed = MasterSeed == 0 ? 1 : MasterSeed;
		State.RandomStreamState = FDeterministicStream::SeedNamedStream(
			State.MasterSeed,
			TEXT("Phase4.RegionalScheduled"));
		State.NextDomainId = Phase4IdBase;
	}

	EPhase4CommandResult FRegionalScheduledSimulation::QueueCommand(
		const FPhase4Command& Command,
		const int64 CurrentGameMilliseconds,
		const FPhase1State& Phase1State,
		const FPhase2State& Phase2State,
		const FPhase3State& Phase3State)
	{
		const FPhase4Validation Validation = ValidateCommand(
			Command,
			CurrentGameMilliseconds,
			Phase1State,
			Phase2State,
			Phase3State);
		if (!Validation.bValid)
		{
			return Validation.Result;
		}
		if (Validation.bRequiresConfirmation)
		{
			return EPhase4CommandResult::AcceptedWithWarning;
		}
		PendingCommands.Add(Command);
		return EPhase4CommandResult::Accepted;
	}

	FPhase4Validation FRegionalScheduledSimulation::ValidateCommand(
		const FPhase4Command& Command,
		const int64 CurrentGameMilliseconds,
		const FPhase1State& Phase1State,
		const FPhase2State& Phase2State,
		const FPhase3State& Phase3State) const
	{
		FPhase4Validation Result;
		if (!Command.Id.IsValid())
		{
			Result.Result = EPhase4CommandResult::RejectedInvalidCommand;
			Result.Cause = TEXT("The regional command has no stable identity.");
			Result.Remedy = TEXT("Submit it through the airport command adapter.");
			return Result;
		}
		if (Command.Type == EPhase4CommandType::InitializeRegionalAirport)
		{
			if (!Phase1State.bAirportOpen ||
				!Phase2State.bInitialized ||
				!Phase3State.bTerminalOpen ||
				Phase3State.Flight.State != EPhase3FlightState::Completed)
			{
				Result.Result =
					EPhase4CommandResult::RejectedPhase3NotReady;
				Result.Cause =
					TEXT("Complete the first domestic passenger service first.");
				Result.Remedy =
					TEXT("Finish RB 304, then open regional planning.");
				return Result;
			}
			if (State.bInitialized)
			{
				Result.Result = EPhase4CommandResult::RejectedInvalidState;
				return Result;
			}
			Result.bValid = true;
			Result.Result = EPhase4CommandResult::Accepted;
			return Result;
		}
		if (!State.bInitialized)
		{
			Result.Result = EPhase4CommandResult::RejectedInvalidState;
			Result.Cause = TEXT("Regional planning is not initialized.");
			Result.Remedy = TEXT("Open the regional planning desk.");
			return Result;
		}

		switch (Command.Type)
		{
		case EPhase4CommandType::AcceptNextRecurringContract:
			if (!State.Contracts.ContainsByPredicate(
				[](const FPhase4ContractRecord& Contract)
					{
						return !Contract.bAccepted;
					}))
			{
				Result.Result = EPhase4CommandResult::RejectedNotReady;
				return Result;
			}
			break;
		case EPhase4CommandType::PublishSevenDayTimetable:
			if (State.bTimetablePublished ||
				State.Contracts.ContainsByPredicate(
					[](const FPhase4ContractRecord& Contract)
						{
							return !Contract.bAccepted;
						}))
			{
				Result.Result =
					EPhase4CommandResult::RejectedScheduleBlocked;
				Result.Cause =
					TEXT("All three recurring contracts must be accepted.");
				Result.Remedy =
					TEXT("Review each operator, then publish the week.");
				return Result;
			}
			break;
		case EPhase4CommandType::ReassignDisruptedFlight:
		{
			const FPhase4FlightRecord* Flight =
				FindFlight(State, Command.FlightId);
			if (!Flight)
			{
				Result.Result =
					EPhase4CommandResult::RejectedMissingReference;
				return Result;
			}
			if (!State.bTimetablePublished ||
				Flight->bCompleted ||
				Flight->DayIndex != 3)
			{
				Result.Result = EPhase4CommandResult::RejectedInvalidState;
				return Result;
			}
			if (Command.RequestedGateId != TEXT("R1"))
			{
				Result.Result =
					EPhase4CommandResult::RejectedGateConflict;
				Result.Cause =
					TEXT("The contact gates overlap the delayed service.");
				Result.Remedy =
					TEXT("Use remote Stand R1 with bus boarding.");
				return Result;
			}
			const FPhase4GateRecord* Gate =
				State.Gates.FindByPredicate(
					[&Command](const FPhase4GateRecord& Candidate)
						{
							return Candidate.GateId ==
								Command.RequestedGateId;
						});
			if (!Gate || !Gate->bOperational || !Gate->bBusAvailable)
			{
				Result.Result =
					EPhase4CommandResult::RejectedRemoteBusUnavailable;
				return Result;
			}
			Result.bValid = true;
			Result.bRequiresConfirmation = true;
			Result.Result = EPhase4CommandResult::AcceptedWithWarning;
			Result.Cause =
				TEXT("The change is inside the 30-minute locked horizon.");
			Result.Consequence =
				TEXT("Passengers, bags, and services must use the remote bus route.");
			Result.Remedy =
				TEXT("Confirm the high-risk change or keep the contact gate delay.");
			return Result;
		}
		case EPhase4CommandType::ConfirmHighRiskGateChange:
		{
			const FPhase4FlightRecord* Flight =
				FindFlight(State, Command.FlightId);
			if (!Flight || Command.RequestedGateId != TEXT("R1"))
			{
				Result.Result =
					EPhase4CommandResult::RejectedMissingReference;
				return Result;
			}
			if (!Command.bConfirmWarning)
			{
				Result.Result =
					EPhase4CommandResult::RejectedLockedHorizon;
				return Result;
			}
			break;
		}
		case EPhase4CommandType::HoldAffectedDepartures:
		case EPhase4CommandType::DivertAffectedArrivals:
		case EPhase4CommandType::DeployTowTeam:
			if (State.Incident.Lifecycle <
				EPhase4IncidentLifecycle::Alerted)
			{
				Result.Result = EPhase4CommandResult::RejectedNotReady;
				return Result;
			}
			break;
		case EPhase4CommandType::ProtectIncidentArea:
			if (!State.Incident.bTowDispatched ||
				!State.Incident.bDeparturesHeld ||
				!State.Incident.bArrivalsDiverted)
			{
				Result.Result = EPhase4CommandResult::RejectedNotReady;
				Result.Cause =
					TEXT("The response route is not protected yet.");
				Result.Remedy =
					TEXT("Hold departures, divert arrivals, and dispatch the tow team.");
				return Result;
			}
			break;
		case EPhase4CommandType::ReviewIncidentCause:
			if (State.Incident.Lifecycle <
				EPhase4IncidentLifecycle::AreaProtected)
			{
				Result.Result = EPhase4CommandResult::RejectedNotReady;
				return Result;
			}
			break;
		case EPhase4CommandType::ApplyRecoveryPlan:
			if (!State.Incident.bReportReviewed)
			{
				Result.Result = EPhase4CommandResult::RejectedNotReady;
				return Result;
			}
			break;
		case EPhase4CommandType::AcceptTenantRenewal:
			if (!State.Renewals.ContainsByPredicate(
				[](const FPhase4RenewalRecord& Renewal)
					{
						return Renewal.State ==
							EPhase4RenewalState::Offered;
					}))
			{
				Result.Result = EPhase4CommandResult::RejectedNotReady;
				return Result;
			}
			break;
		default:
			break;
		}

		Result.bValid = true;
		Result.Result = EPhase4CommandResult::Accepted;
		return Result;
	}

	void FRegionalScheduledSimulation::Step(
		const int64 CurrentGameMilliseconds,
		FStarterAirfieldSimulation& Phase1,
		const FPhase2State& Phase2State,
		const FPhase3State& Phase3State)
	{
		PendingCommands.Sort(
			[](const FPhase4Command& Left, const FPhase4Command& Right)
				{
					return Left.Id.Value < Right.Id.Value;
				});
		for (const FPhase4Command& Command : PendingCommands)
		{
			ApplyCommand(Command, CurrentGameMilliseconds, Phase1);
		}
		PendingCommands.Reset();
		State.LastUpdatedGameMilliseconds = CurrentGameMilliseconds;

		if (!State.bTimetablePublished)
		{
			return;
		}
		if (State.Incident.Lifecycle >=
				EPhase4IncidentLifecycle::Alerted &&
			State.Incident.Lifecycle <
				EPhase4IncidentLifecycle::Recovered)
		{
			UpdateIncident(CurrentGameMilliseconds);
			return;
		}

		const int64 Elapsed = FMath::Max<int64>(
			0,
			CurrentGameMilliseconds - State.PublishedAtGameMilliseconds);
		const int32 TargetDay = FMath::Clamp(
			static_cast<int32>(
				Elapsed / GetPhase4Fixture().OperatingDayMilliseconds) + 1,
			1,
			GetPhase4Fixture().TimetableDays);
		for (int32 Day = 1; Day <= TargetDay; ++Day)
		{
			const uint8 Bit = static_cast<uint8>(1u << (Day - 1));
			if ((State.ProcessedDayMask & Bit) != 0)
			{
				continue;
			}
			AdvanceOperatingDay(Day, CurrentGameMilliseconds, Phase1);
			State.ProcessedDayMask |= Bit;
			State.CurrentOperatingDay = Day;
			if (State.Incident.Lifecycle >=
					EPhase4IncidentLifecycle::Alerted &&
				State.Incident.Lifecycle <
					EPhase4IncidentLifecycle::Recovered)
			{
				break;
			}
		}

		if (State.CurrentOperatingDay == 7 &&
			State.Incident.Lifecycle == EPhase4IncidentLifecycle::Recovered &&
			State.Renewals.ContainsByPredicate(
				[](const FPhase4RenewalRecord& Renewal)
					{
						return Renewal.State ==
							EPhase4RenewalState::Offered;
					}) == false &&
			State.RenewalAcceptedCount == 0)
		{
			State.Renewals[0].State = EPhase4RenewalState::Offered;
			Emit(
				EPhase4EventType::TenantRenewalOffered,
				{},
				State.Renewals[0].TenantId.Value,
				CurrentGameMilliseconds,
				TEXT("Riverbend Connect offered a regional renewal."));
		}

		State.bFixtureCompleted =
			State.CurrentOperatingDay == 7 &&
			State.CompletedFlightCount == State.Flights.Num() &&
			State.CompletedTransferBagCount == State.TransferBags.Num() &&
			State.BorderProcessedPassengerCount ==
				State.InternationalPassengerCount &&
			State.RebookedPassengerCount ==
				State.MissedConnectionPassengerCount &&
			State.Incident.Lifecycle ==
				EPhase4IncidentLifecycle::Recovered &&
			State.RenewalAcceptedCount > 0;
		if (State.bFixtureCompleted &&
			!State.Events.ContainsByPredicate(
				[](const FPhase4Event& Event)
					{
						return Event.Type ==
							EPhase4EventType::WeekCompleted;
					}))
		{
			Emit(
				EPhase4EventType::WeekCompleted,
				{},
				0,
				CurrentGameMilliseconds,
				TEXT("The seven-day regional fixture completed and remains recoverable."));
		}
	}

	void FRegionalScheduledSimulation::ApplyCommand(
		const FPhase4Command& Command,
		const int64 CurrentGameMilliseconds,
		FStarterAirfieldSimulation& Phase1)
	{
		switch (Command.Type)
		{
		case EPhase4CommandType::InitializeRegionalAirport:
			InitializeFixture(CurrentGameMilliseconds);
			Emit(
				EPhase4EventType::RegionalAirportInitialized,
				Command.Id,
				0,
				CurrentGameMilliseconds,
				TEXT("Regional planning opened with three recurring offers."));
			break;
		case EPhase4CommandType::AcceptNextRecurringContract:
			for (FPhase4ContractRecord& Contract : State.Contracts)
			{
				if (!Contract.bAccepted)
				{
					Contract.bAccepted = true;
					Emit(
						EPhase4EventType::RecurringContractAccepted,
						Command.Id,
						Contract.Id.Value,
						CurrentGameMilliseconds,
						FString::Printf(
							TEXT("%s recurring service accepted."),
							*Contract.OperatorDisplayName));
					break;
				}
			}
			break;
		case EPhase4CommandType::PublishSevenDayTimetable:
			State.bTimetablePublished = true;
			State.PublishedAtGameMilliseconds =
				CurrentGameMilliseconds;
			State.CurrentOperatingDay = 1;
			Emit(
				EPhase4EventType::TimetablePublished,
				Command.Id,
				0,
				CurrentGameMilliseconds,
				TEXT("Seven-day timetable published in exact five-minute slots."));
			break;
		case EPhase4CommandType::ConfirmHighRiskGateChange:
		{
			FPhase4FlightRecord* Flight =
				FindFlight(State, Command.FlightId);
			if (Flight)
			{
				Flight->AssignedGateId = Command.RequestedGateId;
				Flight->BoardingMode = EPhase4BoardingMode::RemoteBus;
				Flight->bGateChanged = true;
				Flight->bOverrideRecorded = true;
				Flight->Feasibility = EPhase4Feasibility::HighRisk;
				Flight->RiskReason =
					TEXT("Locked-horizon change; remote bus route confirmed");
				++State.GateChangeCount;
				State.Incident.bOverrideAcknowledged = true;
				State.Incident.bWarningShown = true;
				State.Incident.Lifecycle =
					EPhase4IncidentLifecycle::Warned;
				State.Incident.PlayerDecision =
					TEXT("Confirmed remote gate change inside the locked horizon.");
				Emit(
					EPhase4EventType::GateChangeWarningIssued,
					Command.Id,
					Flight->Id.Value,
					CurrentGameMilliseconds,
					TEXT("High-risk locked-horizon gate change confirmed."));
				Emit(
					EPhase4EventType::GateChanged,
					Command.Id,
					Flight->Id.Value,
					CurrentGameMilliseconds,
					TEXT("Flight reassigned to remote Stand R1 with bus boarding."));
				Emit(
					EPhase4EventType::IncidentWarningIssued,
					Command.Id,
					State.Incident.Id.Value,
					CurrentGameMilliseconds,
					State.Incident.Warning);
			}
			break;
		}
		case EPhase4CommandType::HoldAffectedDepartures:
			State.Incident.bDeparturesHeld = true;
			State.Incident.Mitigations =
				TEXT("Departures held; affected runway protected.");
			break;
		case EPhase4CommandType::DivertAffectedArrivals:
			State.Incident.bArrivalsDiverted = true;
			State.Incident.Mitigations +=
				TEXT(" Arrivals diverted from Runway 27.");
			break;
		case EPhase4CommandType::DeployTowTeam:
			State.Incident.bTowDispatched = true;
			State.Incident.Lifecycle =
				EPhase4IncidentLifecycle::ResourcesDispatched;
			Emit(
				EPhase4EventType::IncidentLifecycleChanged,
				Command.Id,
				State.Incident.Id.Value,
				CurrentGameMilliseconds,
				TEXT("Tow and airport operations teams dispatched."));
			break;
		case EPhase4CommandType::ProtectIncidentArea:
			State.Incident.bAreaProtected = true;
			State.Incident.Lifecycle =
				EPhase4IncidentLifecycle::AreaProtected;
			State.Incident.ProtectedAtGameMilliseconds =
				CurrentGameMilliseconds;
			Emit(
				EPhase4EventType::IncidentLifecycleChanged,
				Command.Id,
				State.Incident.Id.Value,
				CurrentGameMilliseconds,
				TEXT("Area protected. No injuries; other operations continue."));
			break;
		case EPhase4CommandType::ReviewIncidentCause:
			State.Incident.Lifecycle =
				EPhase4IncidentLifecycle::Reported;
			State.Incident.bReportReviewed = true;
			State.Incident.Report =
				TEXT("A weather-delayed aircraft became disabled near Runway 27 after a disclosed locked-horizon gate override. Response access remained clear and the area was protected in four game minutes.");
			State.Incident.Prevention =
				TEXT("Keep a remote-gate buffer during low visibility and preserve the emergency route.");
			Emit(
				EPhase4EventType::IncidentReportCreated,
				Command.Id,
				State.Incident.Id.Value,
				CurrentGameMilliseconds,
				TEXT("Cause report ready with warning, decision, response, and prevention."));
			break;
		case EPhase4CommandType::ApplyRecoveryPlan:
		{
			State.Incident.bRecoveryApplied = true;
			State.Incident.bRunwayClosed = false;
			State.Incident.Lifecycle =
				EPhase4IncidentLifecycle::Recovered;
			State.Incident.Outcome = TEXT("No injuries");
			State.bRecoverable = true;
			if (FPhase4FlightRecord* Flight =
				FindFlight(State, State.Incident.FlightId))
			{
				if (!Flight->bCompleted)
				{
					Flight->bCompleted = true;
					Flight->State = EPhase4FlightState::Completed;
					++State.CompletedFlightCount;
					State.TotalRegionalRevenueCredits +=
						GetPhase4Fixture().
							CompletedFlightRewardCredits;
					Phase1.ApplyExternalEconomyChange(
						TEXT("Phase4RegionalFlightRevenue"),
						GetPhase4Fixture().
							CompletedFlightRewardCredits,
						0,
						TEXT("Recovered regional flight completed."),
						CurrentGameMilliseconds);
				}
			}
			Phase1.ApplyExternalEconomyChange(
				TEXT("Phase4RecoveryGrant"),
				GetPhase4Fixture().RecoveryGrantCredits,
				0,
				TEXT("Regional recovery grant restored continuity."),
				CurrentGameMilliseconds);
			Emit(
				EPhase4EventType::RecoveryApplied,
				Command.Id,
				State.Incident.Id.Value,
				CurrentGameMilliseconds,
				TEXT("Runway reopened after tow, inspection, and recovery support."));
			break;
		}
		case EPhase4CommandType::AcceptTenantRenewal:
			for (FPhase4RenewalRecord& Renewal : State.Renewals)
			{
				if (Renewal.State == EPhase4RenewalState::Offered)
				{
					Renewal.State = EPhase4RenewalState::Accepted;
					++State.RenewalAcceptedCount;
					Emit(
						EPhase4EventType::TenantRenewed,
						Command.Id,
						Renewal.TenantId.Value,
						CurrentGameMilliseconds,
						TEXT("Recurring tenant renewal accepted."));
					break;
				}
			}
			break;
		default:
			break;
		}
	}

	void FRegionalScheduledSimulation::InitializeFixture(
		const int64 CurrentGameMilliseconds)
	{
		const uint64 MasterSeed = State.MasterSeed;
		const uint64 RandomState = State.RandomStreamState;
		State = {};
		State.bInitialized = true;
		State.MasterSeed = MasterSeed;
		State.RandomStreamState = RandomState;
		State.NextDomainId = Phase4IdBase;
		State.InitializedAtGameMilliseconds = CurrentGameMilliseconds;
		State.LastUpdatedGameMilliseconds = CurrentGameMilliseconds;

		auto AddContract =
			[this](
				const TCHAR* ContentId,
				const TCHAR* OperatorId,
				const TCHAR* OperatorName,
				const TCHAR* AircraftId,
				const TCHAR* AircraftName,
				const int32 Capacity,
				const bool bInternational)
			{
				FPhase4ContractRecord Contract;
				Contract.Id = {AllocateId()};
				Contract.ContentId = ContentId;
				Contract.OperatorId = OperatorId;
				Contract.OperatorDisplayName = OperatorName;
				Contract.AircraftId = AircraftId;
				Contract.AircraftDisplayName = AircraftName;
				Contract.FrequencyPerWeek = 7;
				Contract.PassengerCapacity = Capacity;
				Contract.RewardPerCompletedFlightCredits =
					GetPhase4Fixture().CompletedFlightRewardCredits;
				Contract.bInternational = bInternational;
				State.Contracts.Add(MoveTemp(Contract));
			};
		AddContract(
			TEXT("Contract.Phase4.RiverbendConnect"),
			TEXT("Operator.RiverbendConnect"),
			TEXT("Riverbend Connect"),
			TEXT("Aircraft.RegionalTurboprop.Phase4"),
			TEXT("Trailwing 42"),
			42,
			false);
		AddContract(
			TEXT("Contract.Phase4.NorthstarRegional"),
			TEXT("Operator.NorthstarRegional"),
			TEXT("Northstar Regional"),
			TEXT("Aircraft.RegionalJet.Phase4"),
			TEXT("Skylark 72"),
			72,
			false);
		AddContract(
			TEXT("Contract.Phase4.CoastalAirways"),
			TEXT("Operator.CoastalAirways"),
			TEXT("Coastal Airways"),
			TEXT("Aircraft.NarrowBody.Phase4"),
			TEXT("Harborliner 126"),
			126,
			true);

		State.Gates = {
			{TEXT("A1"), TEXT("Gate A1"), EPhase4BoardingMode::Contact,
				TEXT("Regional"), false, false, true},
			{TEXT("A2"), TEXT("Gate A2"), EPhase4BoardingMode::Contact,
				TEXT("NarrowBody"), false, true, true},
			{TEXT("R1"), TEXT("Remote Stand R1"), EPhase4BoardingMode::RemoteBus,
				TEXT("Regional"), true, false, true}};

		for (int32 Day = 1; Day <= GetPhase4Fixture().TimetableDays; ++Day)
		{
			for (int32 OperatorIndex = 0;
				OperatorIndex < State.Contracts.Num();
				++OperatorIndex)
			{
				const FPhase4ContractRecord& Contract =
					State.Contracts[OperatorIndex];
				FPhase4FlightRecord Flight;
				Flight.Id = {AllocateId()};
				Flight.ContractId = Contract.Id;
				Flight.OperatorId = Contract.OperatorId;
				Flight.OperatorDisplayName =
					Contract.OperatorDisplayName;
				Flight.AircraftId = Contract.AircraftId;
				Flight.AircraftDisplayName =
					Contract.AircraftDisplayName;
				Flight.DayIndex = Day;
				Flight.PlannedArrivalMinute =
					OperatorIndex == 0 ? 480 :
					OperatorIndex == 1 ? 690 : 960;
				Flight.PlannedDepartureMinute =
					Flight.PlannedArrivalMinute +
					(OperatorIndex == 0 ? 50 :
						OperatorIndex == 1 ? 65 : 85);
				Flight.PlannedGateId =
					OperatorIndex == 0 ? TEXT("A1") :
					OperatorIndex == 1 ? TEXT("A2") : TEXT("A2");
				Flight.AssignedGateId = Flight.PlannedGateId;
				Flight.BoardingMode = EPhase4BoardingMode::Contact;
				Flight.PassengerCount =
					Contract.PassengerCapacity -
					((Day + OperatorIndex) % 5);
				Flight.bInternational =
					OperatorIndex == 2 && Day == 4;
				Flight.OriginRegion =
					OperatorIndex == 2
						? TEXT("Lakeshore")
						: TEXT("North Valley");
				Flight.DestinationRegion =
					OperatorIndex == 0
						? TEXT("River City")
						: OperatorIndex == 1
							? TEXT("Pine Coast")
							: TEXT("Harbor Republic");
				const TCHAR* FlightPrefix =
					OperatorIndex == 0 ? TEXT("RB") :
					OperatorIndex == 1 ? TEXT("NS") : TEXT("CA");
				Flight.FlightCode = *FString::Printf(
					TEXT("%s %d"),
					FlightPrefix,
					(OperatorIndex == 0 ? 400 :
						OperatorIndex == 1 ? 220 : 700) + Day);
				if (Day == 2 && OperatorIndex == 0)
				{
					Flight.ActualArrivalOffsetMinutes = -10;
				}
				if (Day == 3 && OperatorIndex == 1)
				{
					Flight.ActualArrivalOffsetMinutes = 45;
					Flight.ActualDepartureOffsetMinutes = 50;
					Flight.bWeatherRestricted = true;
					Flight.Feasibility = EPhase4Feasibility::HighRisk;
					Flight.RiskReason =
						TEXT("Low visibility raises the approach minimum and overlaps Gate A2.");
				}
				if (Day == 5 && OperatorIndex == 0)
				{
					Flight.RiskReason =
						TEXT("Wet runway and locked-horizon gate change reduce recovery margin.");
				}
				State.Flights.Add(MoveTemp(Flight));
			}
		}

		FPhase4FlightRecord& Day3Inbound = State.Flights[7];
		FPhase4FlightRecord& Day3Outbound = State.Flights[8];
		Day3Inbound.TransferPassengerCount =
			GetPhase4Fixture().ConnectingPassengerCount;
		Day3Inbound.TransferBagCount =
			GetPhase4Fixture().TransferBagCount;
		FPhase4ConnectionRecord Connection;
		Connection.Id = {AllocateId()};
		Connection.InboundFlightId = Day3Inbound.Id;
		Connection.OutboundFlightId = Day3Outbound.Id;
		Connection.PassengerCount =
			GetPhase4Fixture().ConnectingPassengerCount;
		Connection.BagCount = GetPhase4Fixture().TransferBagCount;
		Connection.MinimumConnectionMinutes = 30;
		Connection.AvailableConnectionMinutes = 18;
		Connection.Cause =
			TEXT("Low visibility delayed the inbound regional jet.");
		Connection.Remedy =
			TEXT("Protect the connection, then rebook the missed party and bags.");
		State.Connections.Add(Connection);
		for (int32 Index = 0;
			Index < GetPhase4Fixture().TransferBagCount;
			++Index)
		{
			FPhase4TransferBagRecord Bag;
			Bag.Id = {AllocateId()};
			Bag.ConnectionId = Connection.Id;
			Bag.InboundFlightId = Connection.InboundFlightId;
			Bag.OutboundFlightId = Connection.OutboundFlightId;
			State.TransferBags.Add(Bag);
		}

		const FPhase4FlightRecord& InternationalFlight =
			State.Flights[11];
		FPhase4BorderRecord Border;
		Border.FlightId = InternationalFlight.Id;
		Border.PassengerCount =
			GetPhase4Fixture().InternationalPassengerCount;
		Border.State = EPhase4BorderState::AwaitingImmigration;
		Border.bControlledRouteValid = true;
		State.BorderProcesses.Add(Border);

		State.Transports = {
			{EPhase4TransportMode::RentalCar,
				TEXT("Facility.Landside.RentalCar"),
				TEXT("Tenant.RiverbendRentals"),
				80,
				10,
				GetPhase4Fixture().RentalCarPassengerCount,
				true,
				true},
			{EPhase4TransportMode::Rail,
				TEXT("Facility.Landside.RailStation"),
				TEXT("Tenant.ValleyRail"),
				160,
				15,
				GetPhase4Fixture().RailPassengerCount,
				true,
				true}};

		for (int32 Day = 1; Day <= 7; ++Day)
		{
			FPhase4ForecastRecord Forecast;
			Forecast.DayIndex = Day;
			Forecast.HourOffset = 0;
			Forecast.Category =
				Day == 3 ? EPhase4WeatherCategory::LowVisibility :
				Day == 5 ? EPhase4WeatherCategory::Rain :
				Day == 6 ? EPhase4WeatherCategory::StrongWind :
				EPhase4WeatherCategory::Clear;
			Forecast.WindDirectionDegrees =
				Day == 6 ? 190 : 260;
			Forecast.WindSpeedKnots =
				Day == 6 ? 19 : Day == 5 ? 15 : 8;
			Forecast.VisibilityMeters =
				Day == 3 ? 2400 : Day == 5 ? 4200 : 10000;
			Forecast.ConfidencePercent =
				Day <= 2 ? 94 : FMath::Max(58, 92 - Day * 5);
			Forecast.RunwaySurface =
				Day == 5 ? TEXT("Wet") : TEXT("Dry");
			Forecast.RecommendedRunway = TEXT("27");
			State.Forecast.Add(Forecast);
		}

		State.ApproachLimits = {
			{TEXT("Aircraft.RegionalTurboprop.Phase4"), 1800, 22,
				TEXT("Visual/PAPI")},
			{TEXT("Aircraft.RegionalJet.Phase4"), 2600, 24,
				TEXT("Non-precision")},
			{TEXT("Aircraft.NarrowBody.Phase4"), 3000, 26,
				TEXT("Non-precision")}};

		for (const FPhase4ContractRecord& Contract : State.Contracts)
		{
			FPhase4RenewalRecord Renewal;
			Renewal.TenantId = {AllocateId()};
			Renewal.OperatorId = Contract.OperatorId;
			Renewal.OperatorDisplayName =
				Contract.OperatorDisplayName;
			Renewal.SatisfactionPercent = 78;
			Renewal.Evidence =
				TEXT("Completion, controllable delay, connections, and safety communication.");
			Renewal.RecoveryOffer =
				TEXT("Reduced frequency remains available if performance slips.");
			State.Renewals.Add(MoveTemp(Renewal));
		}

		State.Incident.Id = {AllocateId()};
		State.Incident.FlightId = State.Flights[12].Id;
		State.Incident.AffectedRunway = TEXT("27");
		State.Incident.OutcomeSeed =
			FDeterministicStream::SeedNamedStream(
				State.MasterSeed,
				TEXT("Phase4.IncidentOutcome"));
		State.Incident.RiskFactor =
			TEXT("Wet runway, reduced recovery margin, and acknowledged locked-horizon override.");
		State.Incident.Warning =
			TEXT("A delayed remote-gate operation during wet conditions can block emergency access near Runway 27.");
		State.Incident.Remedy =
			TEXT("Keep the response route clear, hold departures, and divert arrivals if the aircraft becomes disabled.");
		State.Incident.CurrentConditions =
			TEXT("Rain; wind 260 at 15 kt; visibility 4.2 km; runway wet.");
	}

	void FRegionalScheduledSimulation::AdvanceOperatingDay(
		const int32 DayIndex,
		const int64 CurrentGameMilliseconds,
		FStarterAirfieldSimulation& Phase1)
	{
		for (FPhase4FlightRecord& Flight : State.Flights)
		{
			if (Flight.DayIndex != DayIndex || Flight.bCompleted)
			{
				continue;
			}
			if (DayIndex == 5 &&
				Flight.Id == State.Incident.FlightId &&
				State.Incident.bWarningShown &&
				State.Incident.bOverrideAcknowledged)
			{
				Flight.State = EPhase4FlightState::AtGate;
				State.Incident.Lifecycle =
					EPhase4IncidentLifecycle::Alerted;
				State.Incident.bRunwayClosed = true;
				State.Incident.AlertedAtGameMilliseconds =
					CurrentGameMilliseconds;
				Emit(
					EPhase4EventType::IncidentLifecycleChanged,
					{},
					State.Incident.Id.Value,
					CurrentGameMilliseconds,
					TEXT("Aircraft disabled near Runway 27; response required."));
				continue;
			}
			if (Flight.ActualArrivalOffsetMinutes < 0)
			{
				Flight.State = EPhase4FlightState::Early;
				++State.EarlyFlightCount;
			}
			else if (Flight.ActualArrivalOffsetMinutes > 5)
			{
				Flight.State = EPhase4FlightState::Late;
				++State.LateFlightCount;
			}
			else
			{
				Flight.State = EPhase4FlightState::OnTime;
			}
			if (Flight.bWeatherRestricted)
			{
				++State.WeatherRestrictedFlightCount;
				Emit(
					EPhase4EventType::WeatherRestrictionIssued,
					{},
					Flight.Id.Value,
					CurrentGameMilliseconds,
					TEXT("Low visibility applied the aircraft-specific approach minimum."));
			}
			Flight.bCompleted = true;
			Flight.State = EPhase4FlightState::Completed;
			++State.CompletedFlightCount;
			State.TotalRegionalRevenueCredits +=
				GetPhase4Fixture().CompletedFlightRewardCredits;
			Phase1.ApplyExternalEconomyChange(
				TEXT("Phase4RegionalFlightRevenue"),
				GetPhase4Fixture().CompletedFlightRewardCredits,
				0,
				TEXT("Regional scheduled flight completed."),
				CurrentGameMilliseconds);
			if (FPhase4ContractRecord* Contract =
				State.Contracts.FindByPredicate(
					[&Flight](const FPhase4ContractRecord& Candidate)
						{
							return Candidate.Id ==
								Flight.ContractId;
						}))
			{
				++Contract->CompletedFlights;
				Contract->ControllableDelayMinutes +=
					FMath::Max(0, Flight.ActualDepartureOffsetMinutes);
			}
			Emit(
				EPhase4EventType::FlightActualized,
				{},
				Flight.Id.Value,
				CurrentGameMilliseconds,
				FString::Printf(
					TEXT("%s completed from %s to %s."),
					*Flight.FlightCode.ToString(),
					*MinuteDisplay(
						Flight.PlannedArrivalMinute +
						Flight.ActualArrivalOffsetMinutes),
					*MinuteDisplay(
						Flight.PlannedDepartureMinute +
						Flight.ActualDepartureOffsetMinutes)));
		}

		if (DayIndex == 3)
		{
			FPhase4ConnectionRecord& Connection =
				State.Connections[0];
			Connection.State = EPhase4ConnectionState::Rebooked;
			State.MissedConnectionPassengerCount =
				GetPhase4Fixture().MissedConnectionPassengerCount;
			State.RebookedPassengerCount =
				State.MissedConnectionPassengerCount;
			State.ConnectedPassengerCount =
				Connection.PassengerCount -
				State.MissedConnectionPassengerCount;
			for (int32 Index = 0;
				Index < State.TransferBags.Num();
				++Index)
			{
				State.TransferBags[Index].State =
					Index < State.MissedConnectionPassengerCount
						? EPhase4TransferBagState::Rebooked
						: EPhase4TransferBagState::Loaded;
			}
			Emit(
				EPhase4EventType::ConnectionUpdated,
				{},
				Connection.Id.Value,
				CurrentGameMilliseconds,
				TEXT("18 passengers connected; 6 were rebooked with their bags."));
		}
		if (DayIndex == 4)
		{
			for (FPhase4TransferBagRecord& Bag : State.TransferBags)
			{
				Bag.State = EPhase4TransferBagState::Completed;
				Bag.bReconciled = true;
			}
			State.CompletedTransferBagCount =
				State.TransferBags.Num();
			FPhase4BorderRecord& Border =
				State.BorderProcesses[0];
			Border.ImmigrationProcessed = Border.PassengerCount;
			Border.CustomsProcessed = Border.PassengerCount;
			Border.SecondaryCount = 4;
			Border.State = EPhase4BorderState::CustomsComplete;
			State.InternationalPassengerCount =
				Border.PassengerCount;
			State.BorderProcessedPassengerCount =
				Border.PassengerCount;
			State.RentalCarPassengerCount =
				GetPhase4Fixture().RentalCarPassengerCount;
			State.RailPassengerCount =
				GetPhase4Fixture().RailPassengerCount;
			Emit(
				EPhase4EventType::BorderProcessingUpdated,
				{},
				Border.FlightId.Value,
				CurrentGameMilliseconds,
				TEXT("International passengers completed immigration and customs."));
		}
		Emit(
			EPhase4EventType::OperatingDayCompleted,
			{},
			static_cast<uint64>(DayIndex),
			CurrentGameMilliseconds,
			FString::Printf(
				TEXT("Regional operating day %d completed."),
				DayIndex));
	}

	void FRegionalScheduledSimulation::UpdateIncident(
		const int64 CurrentGameMilliseconds)
	{
		if (State.Incident.Lifecycle ==
				EPhase4IncidentLifecycle::ResourcesDispatched &&
			State.Incident.bAreaProtected)
		{
			State.Incident.Lifecycle =
				EPhase4IncidentLifecycle::AreaProtected;
			State.Incident.ProtectedAtGameMilliseconds =
				CurrentGameMilliseconds;
		}
	}

	FPhase4QuerySnapshot FRegionalScheduledSimulation::CreateQuerySnapshot(
		const uint64 Revision,
		const int64 CurrentGameMilliseconds,
		const FPhase1State& Phase1State,
		const FPhase2State& Phase2State,
		const FPhase3State& Phase3State) const
	{
		FPhase4QuerySnapshot Query;
		Query.Revision = Revision;
		Query.GameTimeMilliseconds = CurrentGameMilliseconds;
		Query.bUnlocked =
			Phase1State.bAirportOpen &&
			Phase2State.bInitialized &&
			Phase3State.bTerminalOpen &&
			Phase3State.Flight.State == EPhase3FlightState::Completed;
		Query.bInitialized = State.bInitialized;
		Query.bTimetablePublished = State.bTimetablePublished;
		Query.bFixtureCompleted = State.bFixtureCompleted;
		Query.bRecoverable = State.bRecoverable;
		Query.CurrentOperatingDay = State.CurrentOperatingDay;
		Query.ContractCount = State.Contracts.Num();
		Query.AcceptedContractCount = Algo::CountIf(
			State.Contracts,
			[](const FPhase4ContractRecord& Contract)
				{
					return Contract.bAccepted;
				});
		Query.FlightCount = State.Flights.Num();
		Query.CompletedFlightCount = State.CompletedFlightCount;
		Query.EarlyFlightCount = State.EarlyFlightCount;
		Query.LateFlightCount = State.LateFlightCount;
		Query.GateChangeCount = State.GateChangeCount;
		Query.WeatherRestrictedFlightCount =
			State.WeatherRestrictedFlightCount;
		Query.ConnectedPassengerCount =
			State.ConnectedPassengerCount;
		Query.MissedConnectionPassengerCount =
			State.MissedConnectionPassengerCount;
		Query.RebookedPassengerCount =
			State.RebookedPassengerCount;
		Query.TransferBagCount = State.TransferBags.Num();
		Query.CompletedTransferBagCount =
			State.CompletedTransferBagCount;
		Query.InternationalPassengerCount =
			State.InternationalPassengerCount;
		Query.BorderProcessedPassengerCount =
			State.BorderProcessedPassengerCount;
		Query.RentalCarPassengerCount =
			State.RentalCarPassengerCount;
		Query.RailPassengerCount = State.RailPassengerCount;
		Query.RenewalAcceptedCount = State.RenewalAcceptedCount;
		Query.IncidentLifecycle = State.Incident.Lifecycle;
		Query.bRunwayClosed = State.Incident.bRunwayClosed;
		Query.bOtherOperationsContinue =
			State.Incident.bOtherOperationsContinue;

		if (!Query.bUnlocked)
		{
			Query.PrimaryStatus =
				TEXT("Complete the domestic terminal service to unlock regional planning.");
		}
		else if (!State.bInitialized)
		{
			Query.PrimaryStatus =
				TEXT("Regional planning is ready to open.");
		}
		else if (!State.bTimetablePublished)
		{
			Query.PrimaryStatus = FString::Printf(
				TEXT("%d/%d recurring contracts accepted"),
				Query.AcceptedContractCount,
				Query.ContractCount);
		}
		else if (State.bFixtureCompleted)
		{
			Query.PrimaryStatus =
				TEXT("Seven-day regional week complete and recoverable.");
		}
		else
		{
			Query.PrimaryStatus = FString::Printf(
				TEXT("DAY %d OF 7 · %d/%d FLIGHTS COMPLETE"),
				FMath::Max(1, State.CurrentOperatingDay),
				State.CompletedFlightCount,
				State.Flights.Num());
		}

		Query.TimetableSummary = FString::Printf(
			TEXT("MON–SUN · A1 / A2 / R1 · %d EXACT SLOTS"),
			State.Flights.Num());
		const FPhase4FlightRecord* Selected = nullptr;
		if (State.Incident.FlightId.IsValid() &&
			State.Incident.Lifecycle >=
				EPhase4IncidentLifecycle::Alerted)
		{
			Selected = FindFlight(State, State.Incident.FlightId);
		}
		if (!Selected)
		{
			Selected = State.Flights.FindByPredicate(
				[](const FPhase4FlightRecord& Flight)
					{
						return Flight.DayIndex == 3 &&
							Flight.bWeatherRestricted;
					});
		}
		if (!Selected && !State.Flights.IsEmpty())
		{
			Selected = &State.Flights[0];
		}
		if (Selected)
		{
			Query.SelectedFlight = FString::Printf(
				TEXT("%s · %s"),
				*Selected->FlightCode.ToString(),
				*Selected->OperatorDisplayName.ToUpper());
			Query.SelectedFlightDetail = FString::Printf(
				TEXT("DAY %d · %s–%s · %s · %s"),
				Selected->DayIndex,
				*MinuteDisplay(Selected->PlannedArrivalMinute),
				*MinuteDisplay(Selected->PlannedDepartureMinute),
				*Selected->AssignedGateId.ToString(),
				*Selected->AircraftDisplayName);
			Query.FeasibilitySummary = FString::Printf(
				TEXT("RWY 27 ✓ · %s ✓ · SERVICES ✓ · %s"),
				*Selected->AssignedGateId.ToString(),
				Selected->RiskReason.IsEmpty()
					? TEXT("RESILIENT")
					: TEXT("HIGH RISK"));
		}
		Query.ConnectionSummary = FString::Printf(
			TEXT("%d CONNECTED · %d MISSED / %d REBOOKED · %d/%d BAGS"),
			State.ConnectedPassengerCount,
			State.MissedConnectionPassengerCount,
			State.RebookedPassengerCount,
			State.CompletedTransferBagCount,
			State.TransferBags.Num());
		Query.BorderSummary = FString::Printf(
			TEXT("INTERNATIONAL · %d/%d IMMIGRATION + CUSTOMS"),
			State.BorderProcessedPassengerCount,
			State.InternationalPassengerCount);
		Query.TransportSummary = FString::Printf(
			TEXT("RENTAL %d · RAIL %d · ROUTES CONNECTED"),
			State.RentalCarPassengerCount,
			State.RailPassengerCount);

		const FPhase4ForecastRecord* CurrentForecast =
			State.Forecast.FindByPredicate(
				[&Query](const FPhase4ForecastRecord& Forecast)
					{
						return Forecast.DayIndex ==
							FMath::Max(1, Query.CurrentOperatingDay);
					});
		if (!CurrentForecast && !State.Forecast.IsEmpty())
		{
			CurrentForecast = &State.Forecast[0];
		}
		if (CurrentForecast)
		{
			const TCHAR* Category =
				CurrentForecast->Category ==
						EPhase4WeatherCategory::LowVisibility
					? TEXT("LOW VISIBILITY")
					: CurrentForecast->Category ==
								EPhase4WeatherCategory::Rain
						? TEXT("RAIN")
						: CurrentForecast->Category ==
									EPhase4WeatherCategory::StrongWind
							? TEXT("STRONG WIND")
							: TEXT("CLEAR");
			Query.WeatherSummary = FString::Printf(
				TEXT("%s · %03d/%d KT · %.1f KM · %s"),
				Category,
				CurrentForecast->WindDirectionDegrees,
				CurrentForecast->WindSpeedKnots,
				static_cast<double>(
					CurrentForecast->VisibilityMeters) / 1000.0,
				*CurrentForecast->RunwaySurface.ToString().ToUpper());
			Query.ForecastSummary = FString::Printf(
				TEXT("6-HOUR FORECAST · %d%% CONFIDENCE · RUNWAY %s"),
				CurrentForecast->ConfidencePercent,
				*CurrentForecast->RecommendedRunway.ToString());
		}

		Query.IncidentHeadline =
			State.Incident.Lifecycle ==
					EPhase4IncidentLifecycle::None
				? TEXT("NO ACTIVE INCIDENT")
				: FString::Printf(
					TEXT("RWY %s · %s"),
					*State.Incident.AffectedRunway.ToString(),
					*Phase4IncidentDisplayName(
						State.Incident.Lifecycle).ToUpper());
		Query.IncidentCause = State.Incident.RiskFactor;
		Query.IncidentConsequence =
			State.Incident.bRunwayClosed
				? TEXT("RWY 27 CLOSED · OTHER OPS CONTINUE")
				: TEXT("RWY 27 OPEN AFTER INSPECTION");
		Query.IncidentRemedy = State.Incident.Remedy;
		Query.IncidentLifecycleSummary =
			State.Incident.Lifecycle ==
					EPhase4IncidentLifecycle::None
				? TEXT("ALERTED ○  DISPATCHED ○  PROTECTED ○  REPORT ○  RECOVERED ○")
				: FString::Printf(
					TEXT("ALERTED ✓  DISPATCHED %s  PROTECTED %s  REPORT %s  RECOVERED %s"),
					State.Incident.bTowDispatched ? TEXT("✓") : TEXT("○"),
					State.Incident.bAreaProtected ? TEXT("✓") : TEXT("○"),
					State.Incident.bReportReviewed ? TEXT("✓") : TEXT("○"),
					State.Incident.bRecoveryApplied ? TEXT("✓") : TEXT("○"));
		Query.Caption =
			State.Incident.Lifecycle >=
					EPhase4IncidentLifecycle::Alerted &&
				State.Incident.Lifecycle <
					EPhase4IncidentLifecycle::Recovered
				? TEXT("AIRPORT 1: Rescue vehicles entering Runway 27.")
				: TEXT("REGIONAL OPS: Timetable, weather, and connections monitored.");
		Query.RenewalSummary = State.Renewals.ContainsByPredicate(
			[](const FPhase4RenewalRecord& Renewal)
				{
					return Renewal.State == EPhase4RenewalState::Offered;
				})
			? TEXT("RIVERBEND CONNECT · RENEWAL READY")
			: State.RenewalAcceptedCount > 0
				? TEXT("RIVERBEND CONNECT · RENEWED")
				: TEXT("TENANT PERFORMANCE · MONITORING");
		Query.StateChecksum = CalculateChecksum();
		return Query;
	}

	bool FRegionalScheduledSimulation::RestoreState(
		const FPhase4State& Candidate,
		const FPhase1State& Phase1State,
		const FPhase2State& Phase2State,
		const FPhase3State& Phase3State)
	{
		if (!ValidateState(
			Candidate,
			Phase1State,
			Phase2State,
			Phase3State))
		{
			return false;
		}
		State = Candidate;
		PendingCommands.Reset();
		return true;
	}

	bool FRegionalScheduledSimulation::ValidateState(
		const FPhase4State& Candidate,
		const FPhase1State& Phase1State,
		const FPhase2State& Phase2State,
		const FPhase3State& Phase3State) const
	{
		if (!Candidate.bInitialized)
		{
			return Candidate.Contracts.IsEmpty() &&
				Candidate.Flights.IsEmpty() &&
				Candidate.Connections.IsEmpty() &&
				Candidate.TransferBags.IsEmpty();
		}
		if (!Phase1State.bAirportOpen ||
			!Phase2State.bInitialized ||
			!Phase3State.bTerminalOpen ||
			Phase3State.Flight.State != EPhase3FlightState::Completed ||
			Candidate.MasterSeed == 0 ||
			Candidate.NextDomainId < Phase4IdBase ||
			Candidate.CurrentOperatingDay < 0 ||
			Candidate.CurrentOperatingDay > 7)
		{
			return false;
		}

		TSet<uint64> ContractIds;
		for (const FPhase4ContractRecord& Contract : Candidate.Contracts)
		{
			if (!Contract.Id.IsValid() ||
				ContractIds.Contains(Contract.Id.Value) ||
				Contract.OperatorId.IsNone() ||
				Contract.AircraftId.IsNone())
			{
				return false;
			}
			ContractIds.Add(Contract.Id.Value);
		}
		TSet<FName> GateIds;
		for (const FPhase4GateRecord& Gate : Candidate.Gates)
		{
			if (Gate.GateId.IsNone() ||
				GateIds.Contains(Gate.GateId) ||
				(Gate.BoardingMode ==
						EPhase4BoardingMode::RemoteBus &&
					!Gate.bBusAvailable))
			{
				return false;
			}
			GateIds.Add(Gate.GateId);
		}
		TSet<uint64> FlightIds;
		for (const FPhase4FlightRecord& Flight : Candidate.Flights)
		{
			if (!Flight.Id.IsValid() ||
				FlightIds.Contains(Flight.Id.Value) ||
				!ContractIds.Contains(Flight.ContractId.Value) ||
				!GateIds.Contains(Flight.AssignedGateId) ||
				Flight.DayIndex < 1 ||
				Flight.DayIndex > 7 ||
				Flight.PlannedArrivalMinute % 5 != 0 ||
				Flight.PlannedDepartureMinute % 5 != 0 ||
				Flight.PlannedDepartureMinute <=
					Flight.PlannedArrivalMinute)
			{
				return false;
			}
			const FPhase4GateRecord* Gate =
				Candidate.Gates.FindByPredicate(
					[&Flight](const FPhase4GateRecord& CandidateGate)
						{
							return CandidateGate.GateId ==
								Flight.AssignedGateId;
						});
			if (!Gate ||
				(Flight.BoardingMode ==
						EPhase4BoardingMode::RemoteBus &&
					!Gate->bBusAvailable))
			{
				return false;
			}
			FlightIds.Add(Flight.Id.Value);
		}
		for (int32 LeftIndex = 0;
			LeftIndex < Candidate.Flights.Num();
			++LeftIndex)
		{
			for (int32 RightIndex = LeftIndex + 1;
				RightIndex < Candidate.Flights.Num();
				++RightIndex)
			{
				const FPhase4FlightRecord& Left =
					Candidate.Flights[LeftIndex];
				const FPhase4FlightRecord& Right =
					Candidate.Flights[RightIndex];
				if (Left.DayIndex == Right.DayIndex &&
					Left.AssignedGateId == Right.AssignedGateId &&
					Left.PlannedArrivalMinute <
						Right.PlannedDepartureMinute + 5 &&
					Right.PlannedArrivalMinute <
						Left.PlannedDepartureMinute + 5)
				{
					return false;
				}
			}
		}

		TSet<uint64> ConnectionIds;
		for (const FPhase4ConnectionRecord& Connection :
			Candidate.Connections)
		{
			if (!Connection.Id.IsValid() ||
				ConnectionIds.Contains(Connection.Id.Value) ||
				!FlightIds.Contains(
					Connection.InboundFlightId.Value) ||
				!FlightIds.Contains(
					Connection.OutboundFlightId.Value) ||
				Connection.PassengerCount < 0 ||
				Connection.BagCount < 0)
			{
				return false;
			}
			ConnectionIds.Add(Connection.Id.Value);
		}
		TSet<uint64> BagIds;
		for (const FPhase4TransferBagRecord& Bag :
			Candidate.TransferBags)
		{
			if (!Bag.Id.IsValid() ||
				BagIds.Contains(Bag.Id.Value) ||
				!ConnectionIds.Contains(Bag.ConnectionId.Value) ||
				!FlightIds.Contains(Bag.InboundFlightId.Value) ||
				!FlightIds.Contains(Bag.OutboundFlightId.Value))
			{
				return false;
			}
			BagIds.Add(Bag.Id.Value);
		}
		for (const FPhase4BorderRecord& Border :
			Candidate.BorderProcesses)
		{
			const FPhase4FlightRecord* Flight =
				FindFlight(Candidate, Border.FlightId);
			if (!Flight || !Flight->bInternational ||
				Border.ImmigrationProcessed >
					Border.PassengerCount ||
				Border.CustomsProcessed > Border.PassengerCount ||
				(Border.State ==
						EPhase4BorderState::CustomsComplete &&
					(!Border.bControlledRouteValid ||
					 Border.ImmigrationProcessed !=
						Border.PassengerCount ||
					 Border.CustomsProcessed !=
						Border.PassengerCount)))
			{
				return false;
			}
		}
		if (Candidate.Incident.Lifecycle >=
				EPhase4IncidentLifecycle::Alerted &&
			(!Candidate.Incident.bWarningShown ||
			 !Candidate.Incident.bOverrideAcknowledged ||
			 Candidate.Incident.OutcomeSeed == 0 ||
			 Candidate.Incident.RiskFactor.IsEmpty() ||
			 Candidate.Incident.Warning.IsEmpty() ||
			 Candidate.Incident.Remedy.IsEmpty()))
		{
			return false;
		}
		if (Candidate.bFixtureCompleted &&
			(Candidate.CompletedFlightCount !=
					Candidate.Flights.Num() ||
			 Candidate.CompletedTransferBagCount !=
					Candidate.TransferBags.Num() ||
			 Candidate.BorderProcessedPassengerCount !=
					Candidate.InternationalPassengerCount ||
			 Candidate.RebookedPassengerCount !=
					Candidate.MissedConnectionPassengerCount ||
			 Candidate.Incident.Lifecycle !=
					EPhase4IncidentLifecycle::Recovered ||
			 Candidate.RenewalAcceptedCount <= 0))
		{
			return false;
		}
		return true;
	}

	uint64 FRegionalScheduledSimulation::CalculateChecksum() const
	{
		uint64 Hash = 1469598103934665603ull;
		HashValue(Hash, State.bInitialized);
		HashValue(Hash, State.bTimetablePublished);
		HashValue(Hash, State.bFixtureCompleted);
		HashValue(Hash, State.bRecoverable);
		HashValue(Hash, State.MasterSeed);
		HashValue(Hash, State.RandomStreamState);
		HashValue(Hash, State.NextDomainId);
		HashValue(Hash, State.NextEventSequence);
		HashValue(Hash, State.InitializedAtGameMilliseconds);
		HashValue(Hash, State.PublishedAtGameMilliseconds);
		HashValue(Hash, State.LastUpdatedGameMilliseconds);
		HashValue(Hash, State.CurrentOperatingDay);
		HashValue(Hash, State.ProcessedDayMask);
		for (const FPhase4ContractRecord& Contract : State.Contracts)
		{
			HashValue(Hash, Contract.Id.Value);
			HashName(Hash, Contract.ContentId);
			HashName(Hash, Contract.OperatorId);
			HashString(Hash, Contract.OperatorDisplayName);
			HashName(Hash, Contract.AircraftId);
			HashValue(Hash, Contract.bAccepted);
			HashValue(Hash, Contract.CompletedFlights);
			HashValue(Hash, Contract.SatisfactionPercent);
		}
		for (const FPhase4GateRecord& Gate : State.Gates)
		{
			HashName(Hash, Gate.GateId);
			HashValue(Hash, Gate.BoardingMode);
			HashValue(Hash, Gate.bBusAvailable);
			HashValue(Hash, Gate.bOperational);
		}
		for (const FPhase4FlightRecord& Flight : State.Flights)
		{
			HashValue(Hash, Flight.Id.Value);
			HashValue(Hash, Flight.ContractId.Value);
			HashName(Hash, Flight.FlightCode);
			HashValue(Hash, Flight.DayIndex);
			HashValue(Hash, Flight.PlannedArrivalMinute);
			HashValue(Hash, Flight.PlannedDepartureMinute);
			HashValue(Hash, Flight.ActualArrivalOffsetMinutes);
			HashValue(Hash, Flight.ActualDepartureOffsetMinutes);
			HashName(Hash, Flight.AssignedGateId);
			HashValue(Hash, Flight.BoardingMode);
			HashValue(Hash, Flight.Feasibility);
			HashValue(Hash, Flight.State);
			HashValue(Hash, Flight.bWeatherRestricted);
			HashValue(Hash, Flight.bGateChanged);
			HashValue(Hash, Flight.bOverrideRecorded);
			HashValue(Hash, Flight.bCompleted);
		}
		for (const FPhase4ConnectionRecord& Connection :
			State.Connections)
		{
			HashValue(Hash, Connection.Id.Value);
			HashValue(Hash, Connection.InboundFlightId.Value);
			HashValue(Hash, Connection.OutboundFlightId.Value);
			HashValue(Hash, Connection.PassengerCount);
			HashValue(Hash, Connection.BagCount);
			HashValue(Hash, Connection.State);
		}
		for (const FPhase4TransferBagRecord& Bag : State.TransferBags)
		{
			HashValue(Hash, Bag.Id.Value);
			HashValue(Hash, Bag.ConnectionId.Value);
			HashValue(Hash, Bag.State);
			HashValue(Hash, Bag.bReconciled);
		}
		for (const FPhase4BorderRecord& Border :
			State.BorderProcesses)
		{
			HashValue(Hash, Border.FlightId.Value);
			HashValue(Hash, Border.PassengerCount);
			HashValue(Hash, Border.ImmigrationProcessed);
			HashValue(Hash, Border.CustomsProcessed);
			HashValue(Hash, Border.State);
		}
		for (const FPhase4ForecastRecord& Forecast : State.Forecast)
		{
			HashValue(Hash, Forecast.DayIndex);
			HashValue(Hash, Forecast.Category);
			HashValue(Hash, Forecast.WindDirectionDegrees);
			HashValue(Hash, Forecast.WindSpeedKnots);
			HashValue(Hash, Forecast.VisibilityMeters);
			HashValue(Hash, Forecast.ConfidencePercent);
		}
		HashValue(Hash, State.Incident.Id.Value);
		HashValue(Hash, State.Incident.Lifecycle);
		HashValue(Hash, State.Incident.FlightId.Value);
		HashValue(Hash, State.Incident.OutcomeSeed);
		HashString(Hash, State.Incident.PlayerDecision);
		HashString(Hash, State.Incident.Mitigations);
		HashString(Hash, State.Incident.Outcome);
		HashString(Hash, State.Incident.Report);
		HashValue(Hash, State.Incident.bWarningShown);
		HashValue(Hash, State.Incident.bOverrideAcknowledged);
		HashValue(Hash, State.Incident.bDeparturesHeld);
		HashValue(Hash, State.Incident.bArrivalsDiverted);
		HashValue(Hash, State.Incident.bAreaProtected);
		HashValue(Hash, State.Incident.bTowDispatched);
		HashValue(Hash, State.Incident.bRunwayClosed);
		HashValue(Hash, State.Incident.bReportReviewed);
		HashValue(Hash, State.Incident.bRecoveryApplied);
		for (const FPhase4RenewalRecord& Renewal : State.Renewals)
		{
			HashValue(Hash, Renewal.TenantId.Value);
			HashName(Hash, Renewal.OperatorId);
			HashValue(Hash, Renewal.State);
			HashValue(Hash, Renewal.SatisfactionPercent);
		}
		for (const FPhase4Event& Event : State.Events)
		{
			HashValue(Hash, Event.Sequence);
			HashValue(Hash, Event.GameTimeMilliseconds);
			HashValue(Hash, Event.Type);
			HashValue(Hash, Event.Cause.Value);
			HashValue(Hash, Event.SubjectId);
			HashString(Hash, Event.Message);
		}
		HashValue(Hash, State.CompletedFlightCount);
		HashValue(Hash, State.EarlyFlightCount);
		HashValue(Hash, State.LateFlightCount);
		HashValue(Hash, State.GateChangeCount);
		HashValue(Hash, State.WeatherRestrictedFlightCount);
		HashValue(Hash, State.ConnectedPassengerCount);
		HashValue(Hash, State.MissedConnectionPassengerCount);
		HashValue(Hash, State.RebookedPassengerCount);
		HashValue(Hash, State.CompletedTransferBagCount);
		HashValue(Hash, State.InternationalPassengerCount);
		HashValue(Hash, State.BorderProcessedPassengerCount);
		HashValue(Hash, State.RenewalAcceptedCount);
		HashValue(Hash, State.TotalRegionalRevenueCredits);
		return Hash;
	}

	uint64 FRegionalScheduledSimulation::AllocateId()
	{
		return State.NextDomainId++;
	}

	void FRegionalScheduledSimulation::Emit(
		const EPhase4EventType Type,
		const FCommandId& Cause,
		const uint64 SubjectId,
		const int64 CurrentGameMilliseconds,
		const FString& Message)
	{
		State.Events.Add({
			State.NextEventSequence++,
			CurrentGameMilliseconds,
			Type,
			Cause,
			SubjectId,
			Message});
	}
}
