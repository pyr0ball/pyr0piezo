import subprocess

major_version = "2.2.1"
print("-DPP_VERSION=\\\"" + major_version + "_" + subprocess.check_output(["git", "rev-parse", "HEAD"])[0:6] + "\\\"")
