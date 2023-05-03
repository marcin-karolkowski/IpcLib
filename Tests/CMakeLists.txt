project("TestRunner")
add_executable(${PROJECT_NAME} main.cpp)

add_test(NAME Test0_Arg_0 COMMAND ${PROJECT_NAME} 0 )

add_test(NAME Test0_Arg_helloWorld COMMAND ${PROJECT_NAME} helloWorld)
set_property(TEST Test0_Arg_helloWorld PROPERTY WILL_FAIL TRUE)

add_test(NAME Test0_Arg_1 COMMAND ${PROJECT_NAME} 1)
set_property(TEST Test0_Arg_1 PROPERTY WILL_FAIL TRUE)

add_test(NAME Test0_Arg_3 COMMAND ${PROJECT_NAME} 3)
set_property(TEST Test0_Arg_3 PROPERTY WILL_FAIL TRUE)