#!/bin/bash
# file name: firstcondor.sh

source /opt/sphenix/core/bin/sphenix_setup.sh -n
source /opt/sphenix/core/bin/setup_local.sh "/sphenix/user/jocl/projects/testinstall"
export TESTINSTALL="/sphenix/user/jocl/projects/testinstall"
export HOME=/sphenix/u/jocl
bash finishcondor.sh jetsample 2952