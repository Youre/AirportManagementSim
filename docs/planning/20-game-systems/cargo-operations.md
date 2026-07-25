# GS-14: Cargo Operations

**Status:** Approved baseline  
**Owner:** Cargo  
**Last updated:** 2026-07-25

## Outcome and scope

Cargo is a complete but intentionally simplified airport specialization. This specification owns freight contracts, shipment units, receiving, storage, build-up/breakdown, screening status, ramp transfer, aircraft loading, and pickup.

## Cargo contract and shipment

Contracts state cargo class, recurring or one-time volume, aircraft requirement, handling facilities, storage needs, service window, reward, and penalty. Initial classes are general freight, mail/express, temperature-sensitive, and oversized. Dangerous-goods detail is not simulated; unsupported or unsafe cargo is simply ineligible.

A shipment is an aggregate unit with stable ID, contract, class, piece count, mass, volume, deadline, security status, storage requirement, and current location. Individual boxes are visual decoration and do not create separate authoritative entities.

## Flow

Outbound cargo follows `Road receipt -> Check/secure -> Storage -> Build-up -> Ramp vehicle -> Aircraft`. Inbound cargo follows `Aircraft -> Ramp vehicle -> Breakdown -> Storage -> Road pickup`. Transfer freight moves between flights through the same secure storage model.

Warehouses define docks, storage zones, processing stations, and airside ports. Capacity is tracked by volume and compatible storage class. Forklifts and internal handling are represented by task progress and optional pooled visuals rather than a detailed driving simulation.

## Operations and specialization

Cargo flights use GS-04 scheduling, GS-06 stands, and GS-07 service dispatch. Night operations are allowed where map and airport policies permit. Cargo ratings emphasize on-time handoff, damage-free abstract handling, capacity, security, and temperature compliance.

Passenger aircraft may carry belly cargo if compatible. Cargo-only progression remains capable of Major status without building a passenger terminal.

## Exceptions and acceptance

Late receipt, full storage, missing equipment, failed security state, incompatible aircraft, missed pickup, or weather delay produces a traceable exception and recovery option. Tests must prove all four cargo classes, inbound/outbound/transfer flow, volume and mass capacity, compatible storage, road-airside continuity, belly cargo, contract performance, and save/load.

