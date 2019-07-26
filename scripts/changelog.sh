#!/bin/bash

# Adapted from https://coderwall.com/p/5cv5lg/generate-your-changelogs-with-git-log
#
# Only commits tagged with #changelog will be added
#
# Usage: pass two tags in when running this script, and a changelog will be generated listing the differences between those two tags.
# ex:
# ./changelog.sh rev.2.0.1 rev.2.1.1
git log ${1}...${2} --pretty=format:'<li> <a href="https://github.com/pyr0ball/pyr0piezo/commit/%H">view commit &bull;</a> %s</li> ' --reverse | grep "#changelog"
