# Initialisation
 1) Initialise environment: `bash --rcfile environment`.

 2) Prepare makefile and clang compilation dictionary: `./configure`

 3) Run compilation: `make -j8`



# Standard analysis flow

1) Prepare skimmed/distilled ntuples:
 * `./run distill.cc data/fill*/Totem1/`
 * `./run distill.cc data/fill*/Totem1/ -danti`

10) Run distributions with no auxiliary input (bootstrap) [needs input from 1)]:
 * `./run distributions.cc data/fill*/Totem1/ -O "-bootstrap 1 -details 5"`

11) Get distributions for anti-diagonals [needs input from 1)]:
 * `./run distributions_anti.cc data/fill*/Totem1/ -danti`

20) Run alignment determination/check [needs input from 10)]:
 * `./run resolution_fit.cc data/fill*/Totem1/`

21) Run alignment determination/check [needs input from 10)]:
 * `./run alignment.cc data/fill*/Totem1/`
 * `./run alignment_fit.cc data/fill*/Totem1/`
 * `./run alignment_final.cc data/fill*/Totem1/`

22) Determine/check 3-out-of-4 effeciencies [some steps needs input from 10)]:
 * `./run eff3outof4.cc data/fill*/Totem1/`
 * `./run eff3outof4_fit.cc data/fill*/Totem1/`
 * `./run eff3outof4_details.cc data/fill*/Totem1/`

23) Determine/check pileup inefficiency:
 * `./run pileup.cc data/fill*/Totem1/`

24) Determine/check unfolding correction (with different methods):
 * `./run unfolding_cf_mc.cc data/fill*/Totem1/`
 * `./run unfolding_cf_ni.cc data/fill*/Totem1/`

30) Re-run distributions (this time with the auxiliary input from 20, 22, 23 and 24):
 `./run distributions.cc data/fill*/Totem1/`

40) Merge t-distributions form datasets and diagonals
 * `cd data`
 * `./merge`
 * `cd ..`



# Automated test
 * go to the base directory (where `run` file is)
 * execute `test/run_test`
