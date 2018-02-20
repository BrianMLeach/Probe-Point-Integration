import sys
import shutil
import os
import subprocess
import tempfile
from shutil import copyfile

# Temporary app to work around a problem with build_info.xml created from a
# regression script.
#
# For compilation arguments the directory separators are incorrect.
# E.g. for -IC:\Users\BML we get -IC:\\\\Users\\\\BML
# Also for quoted paths -I"C:\Users\BML" we get "-I\"C:\\\\Users\\\\BML\""
# This app fixes this.
#

def main():
  ret = build_info_fix(sys.argv[1])
  if ret:
    return 0
  return 1

def build_info_fix(i_strSourceInfoFile):

  try:
    InfoFile = open(i_strSourceInfoFile, 'r')
  except IOError:
    print("Error: failed to open " + i_strSourceInfoFile + "\n")
    return False
  
  lines = []
  for line in InfoFile:
    if line.find('compilation_arguments') >= 0:
      line = line.replace(r'"-I\"', r'-I"')
      line = line.replace(r'\""', r'"')
      line = line.replace(os.sep + os.sep + os.sep + os.sep, os.sep)
    lines.append(line)
    
  InfoFile.close()
  
  try:
    InfoFile = open(i_strSourceInfoFile, 'w')
  except IOError:
    print("Error: failed to open " + i_strSourceInfoFile + "\n")
    return False
      
  for line in lines:
    InfoFile.write(line)
    
  InfoFile.close()
  
  return True
    
if __name__ == "__main__":
  main()
