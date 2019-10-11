#!/bin/bash

# Adapted from https://coderwall.com/p/5cv5lg/generate-your-changelogs-with-git-log
#
# Commits are separated by category, requiring that the user add tags to commits based on their content
# Adding #bugfix to a commit will add it to the bugfix category
# Adding #featureadd to a commit will add it to the added features category
# Adding #docs to a commit will add it to the documentation category
#
# Usage: pass two tags or commit hashes in when running this script, and a changelog will be generated listing the differences between those two tags.
# If args 3 & 4 are unused, default values will be substituted
# ex:
# ./changelog.sh rev.2.0.1 rev.2.1.1
# ./changelog.sh b1d5817 02cd438 githun-username github-project
#
# The script then generates a markdown file to be used on the documentation site

# Get referential location of the script (needed for automatically adding changelog to nav later)
scriptlocation="$(dirname "$0")"

# Set variables from cli args, get a filename-safe timestamp
date_pretty=`date +%Y-%m-%d`
timestamp=`date +%Y-%m-%d_%H-%M-%S`
first_tag="${1}"
last_tag="${2}"
git_user="${3:-pyr0ball}"
git_project="${4:-pyr0piezo}"

# Gather full changelog
changelog=$(git log ${first_tag}...${last_tag} --pretty=format:"- %s [view commit](http://github.com/${git_user}/${git_project}/commit/%H) \n" --reverse | sed 's/#bugfix//g' | sed 's/#featureadd//g' | sed 's/#changelog//g' | sed 's/#docs//g')

# Gather bugfixes
bugfixes=$(git log ${first_tag}...${last_tag} --pretty=format:"- %s [view commit](http://github.com/${git_user}/${git_project}/commit/%H) \n" --reverse | grep "#bugfix" | sed 's/#bugfix//g' )

# Gather added features
featureadd=$(git log ${first_tag}...${last_tag} --pretty=format:"- %s [view commit](http://github.com/${git_user}/${git_project}/commit/%H) \n" --reverse | grep "#featureadd" | sed 's/#featureadd//g' )

# Gather added features
docs=$(git log ${first_tag}...${last_tag} --pretty=format:"- %s [view commit](http://github.com/${git_user}/${git_project}/commit/%H) \n" --reverse | grep "#docs" | sed 's/#docs//g' )

# Build a new nav bar entry for the changelog
navadd="        - Changelog $first_tag to $last_tag: /'changelogs/changelog_${date_pretty}_${first_tag}_${last_tag}.md/'"

# Build a markdown output for the static changelog page
read -r -d '' mdoutput << EOF
###Pyr0-Piezo Changelog $first_tag to $last_tag

#### Added Features

$featureadd

------------------------------------

#### Bugfixes

$bugfixes

------------------------------------

#### Documentation Updates

$docs

------------------------------------

#### Full Changelog

$changelog

EOF

# Build a markdown output for the full changelog page
read -r -d '' changelogmdadd << EOF

### $first_tag to $last_tag

Changelog updated $timestamp

 *Added Features*

$featureadd

------------------------------------

 *Bugfixes*

$bugfixes

------------------------------------

 *Documentation Updates*

$docs

------------------------------------

 *Full Changelog*

$changelog

------------------------------------
EOF

# Change directory to script location to prepare for editing nav content
pushd $scriptlocation

# Create a new markdown file for the changelog
echo -e "$mdoutput" > ../docs/changelogs/changelog_${date_pretty}_${first_tag}_${last_tag}.md

# Add the latest changelog to the nav (Latest at top, under "Logs"
sed -i "/Logs/a \ \ \ \ \ \ \ \ - '${first_tag}-${last_tag}': 'changelogs/changelog_${date_pretty}_${first_tag}_${last_tag}.md'" ../mkdocs.yml

# Append the changelog content to the full list page
echo -e "$changelogmdadd" >> ../docs/changelog.md
