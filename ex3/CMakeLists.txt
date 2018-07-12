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
	BattleshipCompetitionManager.cpp
	BattleshipCompetitionManager.h
	BattleshipManager.cpp
	main.cpp
	LoggerUtils.h
	BattleShipLayouts.cpp
	BattleShipPlayer.h
	BattleShipLayouts.h
	BattleshipManager.h
	BattleshipLogger.h
	BattleshipLogger.cpp
	IBattleshipGameAlgo.h
	Types.h
)

# All source files needed to compile your smart algorithm's .dll
set (SMART_ALGO_SOURCE_FILES
	MotherOfAllAlgos.cpp
	MotherOfAllAlgos.h
	CoordinateSTL.h
	Types.h
	IBattleshipGameAlgo.h	
)

add_definitions(-DUNICODE -D_UNICODE)
##################################
### DO NOT CHANGE FROM HERE ON ###
##################################

message("user: ${user1}, id: ${id1}")

add_definitions(-DUNICODE -D_UNICODE)

set(EX ex3)

# Set exe build parameters
set(EXE_NAME "${EX}.${user1}")
add_executable( ${EXE_NAME}	${EXE_SOURCE_FILES})
set_property(TARGET ${EXE_NAME} PROPERTY FOLDER "${EX}/${user1}")
set_property(TARGET ${EXE_NAME} PROPERTY OUTPUT_NAME "${user1}")

# Set smart algorithm's .dll build parameters
set(ALG3_NAME "${EX}.${user1}.smart")
add_library(${ALG3_NAME} SHARED ${SMART_ALGO_SOURCE_FILES})
set_property(TARGET ${ALG3_NAME} PROPERTY FOLDER "${EX}/${user1}")
set_property(TARGET ${ALG3_NAME} PROPERTY OUTPUT_NAME "${user1}.smart")
target_compile_definitions(${ALG3_NAME} PRIVATE -DALGO_EXPORTS) # preprocessor flag saying we are the one compiling the header now