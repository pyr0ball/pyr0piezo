#!/bin/bash
git log ${1}...${2} --pretty=format:'<li> <a href="https://github.com/pyr0ball/pyr0piezo/commit/%H">view commit &bull;</a> %s</li> ' --reverse | grep "#changelog"
