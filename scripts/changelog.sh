#!/bin/bash

# Adapted from https://coderwall.com/p/5cv5lg/generate-your-changelogs-with-git-log
#
# Only commits tagged with #changelog will be added
#
# Usage: pass two tags or commit hashes in when running this script, and a changelog will be generated listing the differences between those two tags.
# If args 3 & 4 are unused, default values will be substituted
# ex:
# ./changelog.sh rev.2.0.1 rev.2.1.1
# ./changelog.sh b1d5817 02cd438 githun-username github project

first_tag="${1}"
last_tag="${2}"
git_user="${3:-pyr0ball}"
git_project="${4:-pyr0piezo}"

git log ${first_tag}...${last_tag} --pretty=format:"%s [view commit](http://github.com/${git_user}/${git_project}/commit/%H) " --reverse | grep "#changelog"
