function (add_keyboard name)
  set(KEYBOARD_DIR_${name} "${CMAKE_CURRENT_SOURCE_DIR}" CACHE STRING "Directory for the ${name} keyboard")
endfunction()
