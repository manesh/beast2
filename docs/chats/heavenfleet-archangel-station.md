# Archangel Station

Archangel Station is the official desktop application of Heavenfleet.

It is a local-first workstation for:

- Academy
- Courts
- Research
- Engineering
- Archive Preservation

Archangel Station is not a website.

It is not a social media platform.

It is not a chatbot.

It is an officer station connected to the Heavenfleet Archive.

The long-term goal is to provide every member of Heavenfleet with a local copy of civilization's memory and the tools needed to search, preserve, discuss, and expand it.


# Version 0.1

Read-only.

Goals:

- Open local archive bundle
- Search archive
- Browse Academy
- Browse Courts
- Browse Engineering
- View evidence

Non-goals:

- AI
- P2P
- Identity
- User accounts
- Writing


# Version 0.2

Archive Synchronization

Goals:

- Download archive snapshots
- Verify signatures
- Update local archive

Non-goals:

- P2P uploads
- Identity


# Version 0.3

Fleet Preservation

Goals:

- Seed archive bundles
- Seed evidence bundles
- Basic torrent support


# Version 0.4

Archangel Assistant

Goals:

- Local AI
- Archive search assistant
- Engineering assistant
- Research assistant


# Design Philosophy

Archangel Station should remain useful if:

- Heavenfleet.org disappears
- Internet access disappears
- The founder disappears

The archive belongs to humanity.

Not to a server.


# Core Principles

- Local-first.
- Offline-first.
- Archive-first.
- Human-readable data preferred over opaque formats.
- Simplicity preferred over sophistication.
- Read-only before read-write.
- Preservation before AI.
- Search before generation.
- Reproducibility before speculation.
- Open standards preferred over proprietary formats.


# Technology Constraints

Preferred Language:
- Go

Preferred Database:
- SQLite

Preferred Desktop Framework:
- Wails

Preferred Networking:
- HTTP
- Torrent support later

Preferred Archive Formats:
- SQLite
- JSON
- Markdown
- Media files

Avoid:
- Electron
- Cloud dependencies
- Vendor lock-in
- Mandatory accounts


# Core Objects

User
- name
- description

Court
- title
- claims

Claim
- title
- description
- testimonies
- evidence
- definitions

Definition
- title
- body

Evidence
- title
- caption
- image or video or document

Testimony
- title
- body

Academy Lecture
- title
- author
- content

Inventor
- name
- biography

Invention
- inventor
- description

Build Guide
- invention
- instructions

Replication
- invention
- result

Belief
- user
- claim
- believe/idk/reject


# Non-Goals

Archangel Station is not:

- A social network
- A cryptocurrency
- A blockchain
- A DAO
- A replacement operating system
- A general purpose IDE
- AGI
- A fully autonomous civilization simulator

The goal is to preserve, search, and expand the Heavenfleet Archive.


# Success Criteria

Archangel Station is successful if:

- A user can browse the Academy offline.
- A user can browse Courts offline.
- A user can browse Engineering offline.
- A user can search the archive quickly.
- The archive survives the loss of Heavenfleet.org.
- The archive survives the loss of its founder.

Everything else is secondary.


# Relationship To Heavenfleet.org

Archangel Station is not a separate product.

It is a desktop companion to Heavenfleet.org.

Whenever possible:

- Reuse Heavenfleet.org APIs.
- Reuse Heavenfleet.org data models.
- Reuse Heavenfleet.org visual design.
- Reuse Heavenfleet.org terminology.

The website remains the canonical source of truth.

Archangel Station is a local copy and workstation.


# Existing Codebase

The Heavenfleet.org source code will be made available.

Before creating new systems:

- Study existing models.
- Study existing APIs.
- Study existing UI patterns.
- Reuse existing code where practical.

Prefer compatibility over reinvention.


# User Experience

A new user should be able to:

1. Install Archangel Station.
2. Open an archive bundle.
3. Search for information.
4. Browse content.

within five minutes.

Configuration should be minimal.

Technical knowledge should not be required.


# The Valuable Monolith

Archangel Station should begin as a single application.

Avoid premature microservices.

Avoid unnecessary abstraction.

Avoid distributed complexity until justified by real usage.

A simple system that works is preferable to a sophisticated system that does not.


# Data Ownership

Users own their data.

Users should be able to:

- Export data.
- Backup data.
- Copy data.
- Mirror data.

No critical information should be trapped inside proprietary formats.



# Forward Compatibility

Version 0.1 should not prevent:

- Archive synchronization.
- Torrent seeding.
- Local AI integration.
- Identity systems.

Future features should be possible without requiring a complete rewrite.


# Development Priorities

Priority 1:
Archive browsing

Priority 2:
Archive search

Priority 3:
Evidence viewing

Priority 4:
Synchronization

Priority 5:
Preservation

Priority 6:
AI

When in doubt, work on the highest priority unfinished item.


# Architectural Test

Before implementing a feature, ask:

Does this improve:

- Preservation?
- Search?
- Understanding?
- Reproducibility?

If not, the feature is probably out of scope.


# First Task For Claude

Begin by creating the smallest working Archangel Station 0.1 prototype.

The first working prototype should:

1. Launch as a desktop app.
2. Open a local archive bundle.
3. Display a home screen.
4. Browse Academy records.
5. Browse Courts records.
6. Browse Engineering records.
7. Search records by title/content.
8. Open local evidence files when available.

Do not begin Version 0.2 until Version 0.1 works.


# Archive Bundle Format

Version 0.1 archive bundles should be simple.

Preferred structure:

archive/
- archive.sqlite
- manifest.json
- media/
  - images/
  - videos/
  - documents/

The app should be able to open the archive folder locally.

Do not require network access for Version 0.1.


# UI Direction

The visual design should resemble Heavenfleet.org.

Use:

- dark mode
- light mode
- black / white / gold palette
- readable typography
- simple navigation
- mobile-inspired spacing where appropriate

Prioritize clarity over decoration.


# Agent Instructions

When working autonomously:

1. Read this document first.
2. Inspect the existing Heavenfleet.org codebase.
3. Identify reusable models, API shapes, templates, and styles.
4. Create a short implementation plan.
5. Implement one small working milestone at a time.
6. Run builds/tests after each milestone.
7. Keep a changelog.
8. Do not expand scope without updating this document.


# Do Not Invent New Doctrine

Do not rename Heavenfleet concepts unless explicitly instructed.

Use existing terminology:

- Heavenfleet
- Archangel Station
- Academy
- Courts
- Research
- Engineering
- Evidence
- Claims
- Replications
- Archive

Preserve terminology consistency with Heavenfleet.org.


# Build And UX Testing Requirements

Claude should regularly build, run, and manually test the application.

After each meaningful UI milestone:

1. Build the app.
2. Run the app.
3. Open the UI.
4. Click through the main navigation.
5. Test search.
6. Test opening records.
7. Test evidence viewing.
8. Note any confusing or broken UX.
9. Fix obvious issues before moving on.

Do not assume the UI works because the code compiles.

The app must be tested by interacting with it.


# Required User Flows

The following flows must work in Version 0.1:

## Flow 1: Open Archive

- Launch app.
- Select local archive folder.
- Load archive.
- Show archive summary.

## Flow 2: Browse Academy

- Click Academy.
- View lecture list.
- Open lecture.
- Return to list.

## Flow 3: Browse Courts

- Click Courts.
- View court list.
- Open court.
- Open claim.
- View evidence.

## Flow 4: Search

- Enter search query.
- See results.
- Open result.
- Return to search.

## Flow 5: Evidence

- Open evidence record.
- View image/video/document if local file exists.
- Show clear missing-file message if unavailable.


# Connected Station Mode

Archangel Station should eventually support connected use with Heavenfleet.org.

In connected mode, a user should be able to:

- Log in to Heavenfleet.org through Archangel Station.
- Browse live Heavenfleet.org content.
- Submit evidence, claims, replications, and updates to the website.
- Download archive content progressively.
- Prefer torrent/P2P downloads when available.
- Fall back to Heavenfleet.org HTTP downloads when torrents are unavailable.
- Continue using already-downloaded archive content offline.

Archangel Station should be usable as a primary interface for serious Heavenfleet users.

However, Version 0.1 remains read-only and offline-first.


# Progressive Downloading

Archangel Station should not require the full Heavenfleet archive before becoming useful.

The app should support progressive archive loading:

- Metadata first.
- Text records next.
- Thumbnails next.
- Evidence media on demand.
- Full evidence bundles optionally.

The user should be able to search and browse as soon as metadata/text is available.


# Download Priority

When downloading archive content or evidence files, Archangel Station should prefer sources in this order:

1. Local cache.
2. Torrent / P2P peers.
3. Heavenfleet.org HTTP fallback.

The user should never need to understand which source was used unless they open advanced details.


# Home Screen

The default screen should provide immediate access to the archive.

Display:

- Search
- Courts
- Academy
- Research
- Engineering

Also display archive status:

- Archive Version
- Last Sync
- Archive Size
- Local Files
- Connected / Offline status

If updates are available:

- Show Update Available
- Show Download Latest button

The home screen should feel like reporting to a workstation.


# Synchronization UI

Version 0.2 should add:

- Check For Updates
- Download Latest
- Pause Download
- Resume Download
- View Download Progress
- View Sync Status

Users should not need to understand manifests, deltas, or torrents.

Synchronization complexity should remain hidden behind a simple interface.


# Archive Status Panel

Display:

Archive Version:
42

Last Sync:
2026-06-11 21:35 UTC

Archive Size:
12.4 GB

Downloaded Files:
18,442

Status:
Up To Date


# First Run Experience

On first launch:

1. Welcome user.
2. Ask whether to:
   - Open Existing Archive
   - Download Latest Archive

3. Select archive location.

4. Enter main application.

The user should be able to reach useful content within minutes.


# Build Outputs And Installers

Claude should produce runnable builds for major desktop platforms where possible.

Version 0.1 release targets:

- Windows
- macOS
- Linux

Build targets should include:

Windows:
- amd64

macOS:
- arm64
- amd64
- universal when practical

Linux:
- amd64

Use standard Wails build commands appropriate for the platform.

## Release Artifacts

Release files should be placed in:

dist/

Each release should include:

* executable or installer
* version number
* platform name
* SHA256 hash
* release notes

Example:

dist/
  archangel-station-0.1.0-windows-amd64.exe
  archangel-station-0.1.0-macos-universal.dmg
  archangel-station-0.1.0-linux-amd64.AppImage
  SHA256SUMS.txt
  RELEASE_NOTES.md
  
## Packaging Rule

If installer packaging becomes difficult, prioritize portable executables first.

A working portable build is better than a broken installer.


# Version Control

Use Git throughout development.

Before making changes:

- Check current git status.
- Understand existing uncommitted changes.
- Do not overwrite user changes.

After each meaningful milestone:

- Run build/tests.
- Review git diff.
- Commit working changes with a clear message.

Commit small, coherent milestones.

Do not make one giant commit at the end.


# Stop Condition

Version 0.1 is complete when:

- The application builds successfully.
- The application launches successfully.
- A local archive can be opened.
- Academy content can be browsed.
- Court content can be browsed.
- Engineering content can be browsed.
- Search works.
- Evidence files can be opened.
- The application is pleasant to use.

Do not continue expanding scope after Version 0.1 is complete.

Instead, create a list of recommended Version 0.2 improvements.

