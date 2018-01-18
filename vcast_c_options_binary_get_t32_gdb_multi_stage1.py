# revision history. 
# Nov 2015 : read vcast_c_options.h and create a "command file" with T32 dump binary data commands
# Mar 2016 : accept the --gdb command line option and then the command file will be for gdb
# July 2016 : also look for the function call coverage
# April 2017 : accept the --ghs  and then the command file will be for Multi
import os
import sys
import string

class bit_array ():
  def __init__ (self, name, length, unit, type) :
    self.cl_name = name
    self.cl_length = length
    self.cl_unit = unit 
    self.cl_type = type


def read_vcast_c_options ( bitarraylist ) :        
  try:
    vcast_c_optionsfile = open ("vcast_c_options.h", 'r' )
  except IOError:
    print ("Error : vcast_c_options.h does not exist")
    return 0   
  for line in vcast_c_optionsfile.readlines () :
    linestrip = line.strip() 
    if "{" in linestrip and "," in linestrip and "}" in linestrip : 
# then maybe we found something like {1, (char *)vcast_unit_stmt_bytes_1, 3, (char *)vcast_unit_branch_bytes_1, 1},
      tokens = linestrip.split (',')
      tokenindex = 0
      for token in tokens :
        tokenindex = tokenindex + 1
        stringindex = token.find ( "vcast_unit_stmt_bytes_" )
        if stringindex != -1 :
          arraylengthstr = tokens [tokenindex]  # tokenindex already points to the next array element
          if arraylengthstr [-1:] == "}" :
            arraylengthstr = arraylengthstr [0:-1]
          unitstr = token [ stringindex+22:] 
          print ("Found " + token [ stringindex: ]  + " " + arraylengthstr + " " + unitstr ) 
          newbitarray = bit_array ( token [ stringindex:], int( arraylengthstr ) , int(unitstr), 'S' )
          bitarraylist.append (newbitarray)

        stringindex = token.find ( "vcast_unit_branch_bytes_" )
        if stringindex != -1 :
          arraylengthstr = tokens [tokenindex] 
          if arraylengthstr [-1:] == "}" :
            arraylengthstr = arraylengthstr [0:-1]
          unitstr = token [ stringindex+24:] 
          print ("Found " + token [ stringindex: ]  + " " + arraylengthstr + " " + unitstr ) 
          newbitarray = bit_array ( token [ stringindex:], int( arraylengthstr ) , int(unitstr), 'B' )
          bitarraylist.append (newbitarray)  
          
        stringindex = token.find ( "vcast_unit_fn_call_bytes_" )
        if stringindex != -1 :
          arraylengthstr = tokens [tokenindex] 
          if arraylengthstr [-1:] == "}" :
            arraylengthstr = arraylengthstr [0:-1]
          unitstr = token [ stringindex+25:] 
          print ("Found " + token [ stringindex: ]  + " " + arraylengthstr + " " + unitstr ) 
          newbitarray = bit_array ( token [ stringindex:], int( arraylengthstr ) , int(unitstr), 'C' )
          bitarraylist.append (newbitarray)            
          
                  

  vcast_c_optionsfile.close()
  return 1      
    

def create_cmm_file_to_read_bitarrays (bitarraylist) :
  try:
    cmmfile = open ("vcast_c_options.cmm", 'w' )
  except IOError:
    print ("Error : failed to open vcast_c_options.cmm")
    return 0  
  for bit_array in bitarraylist :

    outputstring = "data.save.binary " + bit_array.cl_name + ".bin" + " y.range(" + bit_array.cl_name + ")\n" 
    cmmfile.write (outputstring)
  cmmfile.close ()
  return 
def create_ghs_file_to_read_bitarrays (bitarraylist) :
  try:
    plabackfile = open ("vcast_c_options.p", 'w' )
  except IOError:
    print ("Error : failed to open vcast_c_options.p")
    return 0  
  plabackfile.write("halt\n")
  for bit_array in bitarraylist :
# memdump raw vcast_unit_stmt_bytes_1.bin vcast_unit_stmt_bytes_1 1
    outputstring = "memdump raw " + bit_array.cl_name + ".bin " + bit_array.cl_name + " " + str(bit_array.cl_length) +"\n" 
    plabackfile.write (outputstring)
  plabackfile.write("c\n")    
  plabackfile.close ()
  return   
def create_gdb_file_to_read_bitarrays (bitarraylist) :
  try:
    gdbinifile = open ("vcast_c_options.ini", 'w' )
  except IOError:
    print ("Error : failed to create gdb command file vcast_c_options.ini")
    return 0  
  gdbinifile.write ("set print elements 0\n")
  gdbinifile.write ("set logging on\n")
  gdbinifile.write ("set print array on\n")
  gdbinifile.write ("echo VCAST Buffer Start\\n\n")
 
  for bit_array in bitarraylist :
    outputstring = "output/u " + bit_array.cl_name + "\n" 
    gdbinifile.write (outputstring)
  gdbinifile.write ("echo VCAST Buffer End\\n\n")     
  gdbinifile.write ("set logging off\n")  
  
  gdbinifile.close ()
  return   
       

# for arg in sys.argv:		# prints the elements of the command line which is a list
#   print arg
# sys.argv[0] is the name of ourselves
def main() :
# originaly assume there are no arguments , later add support for --gdb and --t32
# Also assume there is the implicit vcast_c_options.h input file in the current directory.
  build_t32_command_file = False 
  build_gdb_command_file = False 
  build_ghs_command_file = False 
  for arg in sys.argv :
    if arg [0:2] == "--" :
      if arg [2:5] == "gdb" :
        build_gdb_command_file = True
      if arg [2:5] == "t32" :
        build_t32_command_file = True   
      if arg [2:5] == "ghs" :
        build_ghs_command_file = True         
  if build_t32_command_file == False and build_gdb_command_file == False and build_ghs_command_file == False :
    print ("Need one or more of --gdb or --t32 or --ghs as command line options")
    return    
  bitarraylist = []
  if read_vcast_c_options ( bitarraylist ) == 0 :
    return 
  if build_t32_command_file == True :
    create_cmm_file_to_read_bitarrays ( bitarraylist )
  if build_gdb_command_file == True :
    create_gdb_file_to_read_bitarrays ( bitarraylist )
  if build_ghs_command_file == True :
    create_ghs_file_to_read_bitarrays ( bitarraylist )  
  return

# Every Python module has it's __name__ defined and if this is '__main__', it implies that the module is being run standalone by the user and we can do # corresponding appropriate actions. 
if __name__ == "__main__":
  main()
  sys.exit (0)