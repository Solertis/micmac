#
# Christoph Heindl 2010
# Precompiled Headers Demo
# http://cheind.wordpress.com
#

# Instructs the MSVC toolset to use the precompiled header PRECOMPILED_HEADER
# for each source file given in the collection named by SOURCE_VARIABLE_NAME.
function(enable_precompiled_headers PRECOMPILED_HEADER SOURCE_VARIABLE_NAME)
  if(MSVC)
    set(files ${${SOURCE_VARIABLE_NAME}})
    
    # Generate precompiled header translation unit
    get_filename_component(pch_basename ${PRECOMPILED_HEADER} NAME_WE)
    set(pch_abs ${PRECOMPILED_HEADER})
    set(pch_unity ${pch_basename}.cpp)
    FILE(WRITE ${pch_unity} "// Precompiled header unity generated by CMake\n")
    FILE(APPEND ${pch_unity} "#include <${pch_abs}>\n")
    set_source_files_properties(${pch_unity}  PROPERTIES COMPILE_FLAGS "/Yc\"${pch_abs}\"")
    
    # Update properties of source files to use the precompiled header.
    # Additionally, force the inclusion of the precompiled header at beginning of each source file.
    foreach(source_file ${files} )
      set_source_files_properties(
        ${source_file} 
        PROPERTIES COMPILE_FLAGS
        "/Yu\"${pch_abs}\" /FI\"${pch_abs}\""
      )
    endforeach(source_file)
    
    # Finally, update the source file collection to contain the precompiled header translation unit
    set(${SOURCE_VARIABLE_NAME} ${${SOURCE_VARIABLE_NAME}} ${pch_unity} PARENT_SCOPE)
  endif(MSVC)
endfunction(enable_precompiled_headers)