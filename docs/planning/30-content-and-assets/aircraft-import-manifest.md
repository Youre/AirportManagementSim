# CT-01 Aircraft Import Manifest

**Status:** Template ready; no assets approved
**Owner:** Visual content
**Last updated:** 2026-07-25

## Purpose

This manifest gates every aircraft file copied from `C:\Users\dave\Documents\Joes_Game\dist\assets` into Unreal. The CSV template is [aircraft-import-manifest-template.csv](aircraft-import-manifest-template.csv).

The template intentionally contains no asset rows. Phase 0 proves the intake contract and validator; it does not approve rights or import aircraft.

## Rules

- Treat the source directory as read-only.
- Use the inventory's canonical filename as `logical_source_name`.
- Record the exact build-hashed deployed file as `physical_source_path`.
- Calculate SHA-256 immediately before review and again before import.
- `reuse_decision` must be explicitly `approved` before copying.
- Creator, license, depicted aircraft, accuracy, cleanup, scale, directional variants, livery treatment, destination ID, reviewer, and review date are mandatory for an approved row.
- Military and unidentified aircraft cannot be approved for core progression.
- The missing Airbus A330-300 SVG cannot receive a row until restored and reviewed.
- A dry run validates rows and proposed destinations without copying or modifying source files.

## Current reconciliation

The replacement deployment exposes 45 PNGs whose dimensions match the 46-record research inventory. The Airbus A330-300 SVG is absent. Hashed filenames are unstable deployment details, so the logical name and exact physical path/checksum are both retained.
