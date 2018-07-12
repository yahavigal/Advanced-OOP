#############################################################
### YOU SHOULD EDIT THE BELOW WITH YOUR DETAILS AND FILES ###
#############################################################

set(user1 TamarSardas)
set(user2 AssafAvital)
set(user3 YahavAvigal)
set(id1 203628706)
set(id2 204097588)
set(id3 200921740)

# All source files needed to compile your exe
set (EXE_SOURCE_FILES
	BattleshipManager.cpp
	main.cpp
	BattleshipAnimationUtils.cpp
	IBattleshipGameAlgoCommon.cpp
	BattleShipLayouts.cpp
	BattleShipPlayer.h
	BattleShipLayouts.h
	BattleshipManager.h
	IBattleshipGameAlgo.h
	Types.h
)
# All source files needed to compile your naive algorithm's .dll
set (NAIVE_ALGO_SOURCE_FILES
	Naive.cpp
	IBattleshipGameAlgoCommon.cpp
	IBattleshipGameAlgoCommon.h
	Naive.h
	IBattleshipGameAlgo.h
)
# All source files needed to compile your read-from-file algorithm's .dll
set (FILE_ALGO_SOURCE_FILES
	Ex1Algo.cpp
	IBattleshipGameAlgoCommon.cpp
	IBattleshipGameAlgoCommon.h
	Ex1Algo.h
	IBattleshipGameAlgo.h
)
# All source files needed to compile your smart algorithm's .dll
set (SMART_ALGO_SOURCE_FILES
	Ultimate.cpp
	IBattleshipGameAlgoCommon.cpp
	IBattleshipGameAlgoCommon.h
	Ultimate.h
	IBattleshipGameAlgo.h	
)

add_definitions(-DUNICODE -D_UNICODE)
##################################
### DO NOT CHANGE FROM HERE ON ###
##################################

message("user: ${user1}, id: ${id1}")

# Set exe build parameters
set(EXE_NAME "ex2.${user1}")
add_executable( ${EXE_NAME}	${EXE_SOURCE_FILES})
set_property(TARGET ${EXE_NAME} PROPERTY FOLDER "ex2/${user1}")
set_property(TARGET ${EXE_NAME} PROPERTY OUTPUT_NAME "${user1}")

# Set naive algorithm's .dll build parameters
set(ALG1_NAME "ex2.${user1}.naive")
add_library(${ALG1_NAME} SHARED ${NAIVE_ALGO_SOURCE_FILES})
set_property(TARGET ${ALG1_NAME} PROPERTY FOLDER "ex2/${user1}")
set_property(TARGET ${ALG1_NAME} PROPERTY OUTPUT_NAME "${user1}.naive")
target_compile_definitions(${ALG1_NAME} PRIVATE -DALGO_EXPORTS) # preprocessor flag saying we are the one compiling the header now

# Set read-from-file algorithm's .dll build parameters
set(ALG2_NAME "ex2.${user1}.file")
add_library(${ALG2_NAME} SHARED ${FILE_ALGO_SOURCE_FILES})
set_property(TARGET ${ALG2_NAME} PROPERTY FOLDER "ex2/${user1}")
set_property(TARGET ${ALG2_NAME} PROPERTY OUTPUT_NAME "${user1}.file")
target_compile_definitions(${ALG2_NAME} PRIVATE -DALGO_EXPORTS) # preprocessor flag saying we are the one compiling the header now

# Set smart algorithm's .dll build parameters
set(ALG3_NAME "ex2.${user1}.smart")
add_library(${ALG3_NAME} SHARED ${SMART_ALGO_SOURCE_FILES})
set_property(TARGET ${ALG3_NAME} PROPERTY FOLDER "ex2/${user1}")
set_property(TARGET ${ALG3_NAME} PROPERTY OUTPUT_NAME "${user1}.smart")
target_compile_definitions(${ALG3_NAME} PRIVATE -DALGO_EXPORTS) # preprocessor flag saying we are the one compiling the header now