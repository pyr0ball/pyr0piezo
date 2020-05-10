import subprocess

major_version = "2.3.0"

git_head = str(subprocess.check_output(["git", "rev-parse", "HEAD"])[0:6])

print("-DPP_VERSION=\\\"" + major_version + "_" + git_head + "\\\"")