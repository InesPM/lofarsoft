## This file should be placed in the root directory of your project.
## Then modify the CMakeLists.txt file in the root directory of your
## project to incorporate the testing dashboard.

set(CTEST_PROJECT_NAME "pybdsm")
set(CTEST_NIGHTLY_START_TIME "00:00:00 EST")

set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE "svn.astron.nl")
set(CTEST_DROP_LOCATION "/CDash/submit.php?project=pybdsm")
set(CTEST_DROP_SITE_CDASH TRUE)
