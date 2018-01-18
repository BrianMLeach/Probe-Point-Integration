import sys
import shutil
import os
import subprocess
import tempfile
from shutil import copyfile

# N.B. At present source_dir is needed, but in the future if omitted it means 
# dest_dir blanket replaces the path.
def main():
  source_bat = ''
  dest_bat = ''
  source_env = ''
  dest_env = ''
  p_source_dir = False
  source_dir = ''
  p_dest_dir = False
  dest_dir = ''
  store_index = 0

  for line in sys.argv[1:]:
    if p_source_dir:
      source_dir = line
      p_source_dir = False
    elif p_dest_dir:
      dest_dir = line
      p_dest_dir = False
     
    else:
      
      if (line.find('-s') == 0) or (line.find('/s') == 0):
        line = line[2:]
        if line != '':
          source_dir = line
        else:
          p_source_dir = True
          
      elif (line.find('-d') == 0) or (line.find('/d') == 0):
        line = line[2:]
        if line != '':
          dest_dir = line
        else:
          p_dest_dir = True
          
      else:
        if store_index == 0:
          source_bat = line
        elif store_index == 1:
          dest_bat = line
        elif store_index == 2:
          source_env = line
        elif store_index == 3:
          dest_env = line
        store_index = store_index + 1

  ret = 0;
  if (dest_dir == '') or (source_dir == ''):
    usage()
  elif (store_index != 4):
    usage()
  else:
    ret = regression_rebase(source_bat, dest_bat, source_env, dest_env, source_dir, dest_dir)
  if ret:
    return 0
  return 1

def usage():
  print("VectorCAST regression file rebase.")
  print("RelocatePathInScript.py <Src.BAT> <Dst.BAT> <Src_Env> <Dst_Env> [-s Src_Path]")
  print("                        [-d Dst_Path]")
  print("  Src.BAT     The source cover regression script")
  print("  Dst.BAT     The destination cover regression script")
  print("  Src_Env     Points to the working directory of the source environment")
  print("  Dst_Env     Points to the working directory of the destination environment")
  print("  Src_Path    A partial source path encompassing all of the source files")
  print("  Dst_Path    Src_Path is exchanged for Dst_Path in the batch file")
  print("N.B. This script does not copy the source files for you")
  print("     The source files must not be instrumented when running Dst.BAT")
  print("     Not verified for Linux based systems. Not verified for UUT regressions")
 
def regression_rebase(i_strSourceBat, i_strDestBat, i_strSourceEnv, i_strDestEnv, i_strSourceDir, i_strDestDir):

  fs_case_sensitive = is_fs_case_sensitive('.')
    
  i_strSourceDir = strip_os_sep(i_strSourceDir)
  i_strDestDir = strip_os_sep(i_strDestDir)
  i_strSourceEnv = strip_os_sep(i_strSourceEnv)
  i_strDestEnv = strip_os_sep(i_strDestEnv)
   
  try:
      source_bat_file = open(i_strSourceBat, 'r')
  except IOError:
      print("Error: failed to open " + i_strSourceBat + "\n")
      return False
  try:      
      dest_bat_file = open(i_strDestBat, 'w')
  except IOError:
      print ("Error: failed to open " + i_strDestBat + "\n")
      return False
  
  line_column = 0
  case_column = 0
  source_count = 0
  changed_count = 0
  add_pos = 0
  for line in source_bat_file:
 
    add_pos = line.find('cover source add')
    if add_pos >= 0:
      temp_line = line
      source_count = source_count + 1
      line = replace_fs(line, i_strDestDir, i_strSourceDir, fs_case_sensitive)
      if line != temp_line:
        changed_count = changed_count + 1
        
    if (line.find('cover options set_instrumentation_directory') >= 0) or \
       (line.find('cover probe_point add_file') >= 0):
      line = replace_fs(line, i_strDestEnv, i_strSourceEnv, fs_case_sensitive)       

    if (line.find('LIBRARY_INCLUDE_DIR') >= 0) or (line.find('TESTABLE_SOURCE_DIR') >= 0) or \
      (line.find('TYPEHANDLED_SOURCE_DIR') >= 0) or (line.find('cover instrument') >= 0) or \
      (line.find('cover source unit_options') >= 0) or \
      (line.find('cover append_cover_io') >= 0):
      line = replace_fs(line, i_strDestDir, i_strSourceDir, fs_case_sensitive)
  
    dest_bat_file.write(line)

  source_bat_file.close()
  dest_bat_file.close()
                            
  if changed_count == 0:
    print("Error: No source files were rebased")
    return False
  
  if changed_count != source_count:
    print("Warning: Not all the source files were rebased")
       
  return True

# Strip the end separator unless the path is of form 'd:\'
def strip_os_sep(i_strPath):
  nLen = len(i_strPath)
  if nLen > 1:
    if i_strPath[-1:] == os.sep:
      if (nLen != 3) or (i_strPath[1] != ':'):
        i_strPath = i_strPath[:-1]
  return i_strPath
    
def is_fs_case_sensitive(path):
    #
    # Force case with the prefix
    #
  with tempfile.NamedTemporaryFile(prefix='TmP',dir=path) as tmp_file:
    return(not os.path.exists(tmp_file.name.lower()))
    
def replace_fs(line, dest, src, sensitive):
  if sensitive:
    replace_line = line.replace(src, dest) 
  else:
    line_pos = line.upper().find(src.upper())
    replace_line = ''
    while line_pos >= 0:
      replace_line = replace_line + line[0:line_pos] + dest
      line = line[line_pos+len(src):]
      line_pos = line.upper().find(src.upper())
    replace_line = replace_line + line
  return replace_line
    
if __name__ == "__main__":
  main()
