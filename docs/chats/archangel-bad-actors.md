strategies for dealing with spammers / bot accounts







# anti-spam and anti-bot strategies for archangel.my
Archangel should assume spam will arrive early.
Any site about public claims, public figures, evidence, politics, science, testimony, and controversy will attract:
- bots
- trolls
- paid influence campaigns
- low-effort partisans
- link farmers
- fake testimony
- duplicate claim floods
- coordinated brigades
- reputation farming
- synthetic public figures
- AI-generated evidence sludge
The goal is not to make abuse impossible.
The goal is to make abuse expensive, visible, reversible, and less rewarding.
---
# core principle
Do not only ask:
> is this user real?
Ask:
> what power should this account have right now?
A new account can read.
A new account can save claims.
A new account can draft.
But a new account should not immediately be able to flood the public record.
Archangel should separate:
- account creation
- posting
- evidence submission
- public visibility
- moderation power
- reputation power
---
# layered trust model
Use layers instead of one hard gate.
## layer 1: visitor
Can:
- read public claims
- search claims
- view evidence
- view public figure pages
Cannot:
- post
- vote
- award halos
- submit testimony
- edit claims
## layer 2: new angel
Can:
- create a profile
- follow claims
- draft claims privately
- submit limited comments
- propose evidence for review
Limits:
- low daily post cap
- no external link posting at first
- no halo-awarding
- no claim-page edits
- no mass tagging
## layer 3: verified angel
Can:
- submit evidence
- create claims
- create counterclaims
- publicly record stance
- receive halos
Requires some combination of:
- email verification
- phone verification
- account age
- successful review history
- low abuse score
- optional identity proof
## layer 4: trusted angel
Can:
- edit summaries
- classify evidence
- mark duplicate claims
- flag misleading framing
- help review testimony
Requires:
- time
- useful contribution history
- halos from trusted users
- low reversal rate
- no serious abuse history
## layer 5: winged roles
Can:
- moderate
- judge disputes
- review appeals
- resolve evidence classification
- protect high-risk claim pages
Requires:
- public accountability
- visible moderation history
- stricter conduct standard
---
# rate limits
Rate limits are boring and essential.
Limit by:
- account age
- trust level
- IP reputation
- device fingerprint risk
- claim sensitivity
- link count
- duplicate similarity
- report history
- posting velocity
- failed verification attempts
Example limits:
```md
new angel:
- 3 comments per day
- 1 claim proposal per day
- 2 evidence submissions per day
- 0 direct external links until trusted
verified angel:
- 20 comments per day
- 5 claim proposals per day
- 10 evidence submissions per day
trusted angel:
- higher limits
- still throttled during brigades

Important:

Rate limits should be soft when possible.

Instead of only blocking, Archangel can say:

Your contribution has been saved as a draft and will become reviewable after your current limit resets.

This preserves sincere work while stopping floods.

⸻

posting friction

Use friction only where it protects the record.

Examples:

* require a claim to have a counterclaim field before publishing
* require evidence type classification
* require "according to" source labeling
* require testimony to be labeled as testimony
* require external links to include archive links when possible
* require public figure claims to include a source
* require users to choose whether they are making a claim, submitting evidence, or giving testimony

Spam thrives in vague text boxes.

Archangel should make public contribution structured.

⸻

proof-of-work without crypto weirdness

For suspicious accounts, require small human-effort tasks:

* wait time before publishing
* edit review before submission
* choose evidence type
* explain why evidence supports the claim
* link claim to an existing counterclaim
* answer "what would weaken this claim?"
* require preview before publish

This is better than captcha alone because it tests whether the user understands the system.

A bot can write text.

A bad bot struggles with structured epistemic contribution.

⸻

captcha and verification

Use captcha, but do not rely on it.

Possible gates:

* captcha at signup
* captcha after suspicious behavior
* captcha before first public post
* captcha before many rapid submissions
* captcha before posting links
* captcha before creating many claims

Verification options:

* email
* phone
* passkey
* optional government ID for higher roles
* optional video verification for public identity
* optional social account verification
* optional web-of-trust vouching

Do not require heavy identity proof for basic participation.

Do require stronger proof for higher-trust roles.

⸻

link control

Most spam wants links.

New accounts should have restricted link power.

Rules:

new angel:
- no clickable external links
- links saved as plain text or review-only
- links must be attached as evidence, not comments
verified angel:
- limited links per day
- suspicious domains require review
trusted angel:
- normal link privileges

Also:

* block known spam domains
* score URL shorteners as risky
* expand shortened URLs
* require archive copy for important sources
* detect repeated domain promotion
* detect affiliate links
* detect tracking-heavy links
* detect copy-paste link floods

⸻

duplicate and similarity detection

Spam often repeats.

Detect:

* same comment posted across claims
* near-duplicate claims
* repeated evidence links
* repeated testimony patterns
* same text with small mutations
* repeated public figure misattributions
* mass creation of low-quality counterclaims

Instead of deleting immediately:

* collapse duplicates
* mark as possible duplicate
* route to review
* merge into existing claim
* reduce visibility
* preserve audit log

Archangel should avoid silent disappearance.

Better:

This contribution was hidden from public view because it appears to duplicate 14 previous submissions.

⸻

reputation farming protection

Halos should not become farmable likes.

Risks:

* bot rings awarding halos
* friends trading halos
* ideological groups boosting each other
* users posting obvious popular content to farm rank
* low-quality "agree with my side" halo loops

Protections:

* halos attach to specific contributions, not generic profiles
* halo reason required
* halo source visible
* halo weight depends on giver trust
* reciprocal halo rings detected
* halos can be reversed if contribution is later found false or abusive
* controversial claims should not grant huge halo boosts from one faction alone
* cross-side fairness can be rewarded more than applause

Good halo reasons:

* added primary source
* corrected misquote
* added strong counterclaim
* archived disappearing evidence
* labeled uncertainty accurately
* improved neutral summary
* fairly represented opposing side

Bad halo reasons:

* based
* owned them
* my side wins
* ratio
* destroy this person

⸻

public figure protection

Public figure pages will be attacked.

Require stronger sourcing for:

* accusations
* quotes
* stance summaries
* criminal allegations
* medical claims
* sexual claims
* identity claims
* financial misconduct claims
* election claims
* violence-related claims

Rules:

public figure claim:
- must include source
- must distinguish quote, paraphrase, allegation, opinion, and evidence
- must include date
- must allow counter-evidence
- high-risk edits go to review

Never let new accounts freely rewrite public figure pages.

⸻

testimony protection

Testimony is powerful and dangerous.

Every testimony item should be labeled:

type: testimony
source: @username
verification: unverified / partially verified / verified
risk: low / medium / high
visibility: public / limited / sealed pending review

For new accounts:

* testimony can be submitted
* but may appear as pending
* high-risk testimony requires review
* claims about private individuals require stricter handling
* do not let testimony masquerade as confirmed evidence

Core label:

This is testimony, not verified fact.

⸻

AI-generated sludge protection

Archangel should expect AI-generated claim spam.

Signals:

* many long generic claims
* no primary sources
* vague evidence language
* repeated structure
* suspiciously broad coverage
* instant posting across many topics
* citations that do not support text
* fake quotes
* hallucinated public figure positions

Countermeasures:

* require source snippets
* require claim-specific evidence
* require "according to" labels
* require quote verification for quoted public figures
* require a counterclaim
* limit bulk claim creation
* allow AI-assisted drafts but mark them as drafts until reviewed

Possible label:

ai_assisted: yes
human_reviewed: no
public_visibility: limited

⸻

shadow banning vs visible moderation

Avoid pure shadow banning as the default.

It creates distrust.

Prefer visible states:

* draft
* pending review
* limited visibility
* hidden for spam
* removed for abuse
* merged as duplicate
* archived for record
* locked pending review

However, for active bot attacks, temporary shadow throttling may be useful.

Use it carefully.

Log it internally.

For real users, show clear status.

⸻

brigade mode

Some claims will get attacked.

Archangel needs a "storm mode" or "cloud lock" state.

When a claim is under coordinated attack:

* slow new comments
* restrict new accounts
* require verified accounts for edits
* freeze claim summary
* allow evidence proposals but queue them
* temporarily disable halos on that claim
* prioritize judge review
* show public banner

Example:

storm mode active
This claim is receiving unusual activity.
New contributions may be delayed.
Existing evidence remains visible.
Moderation actions will be logged.

This fits the cloud/angel theme without sounding childish.

⸻

account risk scoring

Each account can have an internal risk score.

Signals:

* account age
* verification level
* posting speed
* duplicate rate
* link rate
* report rate
* reversal rate
* halo ring behavior
* IP/device risk
* domain promotion
* public figure attack patterns
* use of banned phrases
* failed captcha attempts
* many accounts from same source

Do not show the raw score.

Do show consequences:

Your account is temporarily limited because of unusual posting behavior.

⸻

domain and source reputation

Evidence links should have source metadata.

Track:

* domain age
* known spam domain
* archive availability
* source type
* correction history
* repeated use by bot accounts
* whether source is primary or secondary
* whether the same source is being spammed across claims

But avoid declaring:

trusted source = true

Better:

source profile:
- primary / secondary / unknown
- archived / not archived
- frequently disputed / rarely disputed
- used in 142 claims
- challenged in 18 claims

Archangel should show source history, not pretend source reputation is simple.

⸻

moderation queues

Create separate queues:

* spam queue
* duplicate claim queue
* evidence review queue
* public figure risk queue
* testimony verification queue
* appeal queue
* bot-ring investigation queue

Do not put everything into one moderation inbox.

That becomes unusable.

⸻

judge tools

Judges need good tools.

A judge should see:

* account age
* recent activity
* duplicate matches
* linked IP/device risk cluster if allowed
* previous reversals
* halo activity
* reports
* affected claims
* evidence links
* edit diffs
* public impact
* suggested action

Judge actions:

* approve
* reject
* request source
* mark disputed
* merge duplicate
* limit account
* remove link
* hide testimony
* lock claim
* escalate
* reverse halo
* ban account
* ban cluster

Every action should create a public or semi-public log entry depending on privacy risk.

⸻

ban strategy

Use escalating consequences:

1. warning
2. temporary posting cooldown
3. link restriction
4. evidence submission restriction
5. halo restriction
6. claim creation restriction
7. temporary suspension
8. permanent ban
9. cluster ban
10. legal/security escalation if needed

For bots, skip escalation.

For humans, allow correction.

⸻

appeal strategy

Appeals matter because Archangel’s whole brand is public fairness.

Appeals should include:

* what action happened
* who made it
* why
* what policy was cited
* how to appeal
* what evidence the user can provide

Appeal outcomes:

* upheld
* reversed
* modified
* needs more evidence
* escalated to higher judge

⸻

anti-spam database tables

Possible tables:

accounts
account_verifications
account_risk_scores
account_limits
sessions
devices
ip_events
claims
claim_edits
evidence_items
evidence_reviews
testimony_items
halo_events
halo_reversals
moderation_actions
moderation_appeals
duplicate_clusters
spam_reports
domain_reputation
storm_mode_events

⸻

minimum viable anti-spam for early archangel

Do this first:

mvp anti-spam:
1. email verification
2. account age limits
3. daily post limits
4. no external links for brand-new accounts
5. evidence must be structured
6. claim creation requires counterclaim field
7. public figure claims require source
8. manual review queue
9. visible moderation log
10. admin/judge ability to lock claim pages
11. duplicate detection by simple text similarity
12. halo events attached to contributions

This is enough for an early private/public alpha.

⸻

recommended early limits

new angel:
- read unlimited
- save unlimited
- draft unlimited
- 3 comments/day
- 1 claim proposal/day
- 2 evidence proposals/day
- no clickable links
- no halos
angel after 7 days + email verified:
- 10 comments/day
- 3 claim proposals/day
- 5 evidence proposals/day
- limited links
- can receive halos
- can give 3 halos/day
trusted angel:
- higher limits
- can edit summaries
- can classify evidence
- can give more halos
judge:
- review queues
- lock claims
- reverse spam
- moderate evidence

⸻

design language

Use Archangel-native terms carefully.

Good:

storm mode
halo review
wing level
angel account
judge review
public record
evidence ledger
claim cloud

Maybe avoid:

smite
sin
heresy
excommunication
holy
divine punishment

The system can be mythic without sounding cultish or punitive.

⸻

short version

Archangel anti-spam strategy:

* let everyone read
* make posting structured
* limit new accounts
* restrict links early
* attach halos to contributions
* detect duplicates
* review public figure edits
* label testimony carefully
* slow down brigades
* make moderation visible
* let humans appeal
* make abuse expensive
* make truth-seeking easier than flooding

A strong product rule for Archangel:
```md id="l2l24b"
A new angel may carry a message.
A trusted angel may help maintain the record.
A judge may protect the process.
No one may simply declare the truth.


