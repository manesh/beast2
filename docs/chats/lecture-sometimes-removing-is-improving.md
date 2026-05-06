lecture: sometimes removing is improving

many people think improvement means adding

more buttons
more features
more abstractions
more layers

sometimes this is true

but many systems do not fail because they are missing something

they fail because:

they accumulated too much

too many concepts
too many paths
too many exceptions

every added thing has cost

memory cost
ui cost
maintenance cost
understanding cost

sometimes the best improvement is not adding

it is removing

i recently realized beast2 did not need crumb files when the database lives in the same folder as the artifacts

the system already had truth:

-beast2.db exists
→ local mode

the crumb file became redundant

so removing it improved the system

fewer files
less ambiguity
less code
less explanation

this pattern appears everywhere

a good engineer does not just ask:

what can i add?

they also ask:

what can i remove
without losing capability?

sometimes the answer is surprising

sometimes removing one concept makes five others clearer

sometimes deleting code is progress

sometimes deleting a file is progress

sometimes deleting a feature is progress

because systems do not become beautiful merely by becoming larger

they become beautiful when:

each remaining part justifies its existence

this is one reason simplicity is difficult

removing the wrong thing breaks systems

removing the right thing clarifies them

there is also a psychological trap

people often associate:

more
with
better

but in engineering:

unnecessary complexity is debt

the ideal system is not:

the system with the most parts

the ideal system is:

the simplest system
that still accomplishes the goal

sometimes improving a system means building something new

sometimes improving a system means removing something old

wisdom is knowing the difference