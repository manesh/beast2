beast2.ai

single-page dark cerulean landing site for Beast2

⸻

overview

beast2.ai is a minimal, local-first landing page.

no frameworks
no build step
just index.html

⸻

structure

beast2-ai-site/
  index.html
  README.md


⸻

local preview

open directly:

open index.html

or with a server:

python3 -m http.server 8080

then visit:

http://localhost:8080


⸻

deploy (cloudflare pages)

first time:

npx wrangler pages deploy . --project-name beast2-ai

update:

git add .
git commit -m "update"
git push
npx wrangler pages deploy . --project-name beast2-ai


⸻

custom domain

in cloudflare dashboard:
	•	go to pages → beast2-ai
	•	add domain: beast2.ai
	•	follow dns instructions

⸻

donate (ko-fi)
	1.	create account: https://ko-fi.com
	2.	connect paypal or stripe
	3.	copy your link:

https://ko-fi.com/YOURNAME

	4.	replace in index.html:

href="#"
→
href="https://ko-fi.com/YOURNAME"

default options:
	•	$6.66
	•	$66.60
	•	$666

⸻

subscribe

v1 options:
	•	leave placeholder form
	•	or replace with:
	•	buttondown
	•	convertkit
	•	simple google form

future:
	•	self-hosted endpoint

⸻

design rules
	•	dark theme
	•	cerulean accents
	•	viridian for donate
	•	single page
	•	no scroll clutter

⸻

editing

edit:

index.html

no build required

⸻

philosophy

simple > complex
one page > many pages
ship > polish


⸻

beast note

overkill is acceptable

missing one bit is worse than storing one million extra
:::

⸻



