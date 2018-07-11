set(user1 Tamarsardas)
set(user2 Assafavital)
set(user3 Yahavavigal)
set(id1 203628706)
set(id2 204097588)
set(id3 200921740)
set (SOURCE_FILES
	BattleShip.cpp
	BattleShip.h
	BattleShipLayouts.cpp
	BattleShipLayouts.h
	IBattleshipGameAlgo.h
	Main.cpp
)

### DO NOT CHANGE FROM HERE ON ###

message("user: ${user1}, id: ${id1}")
message("user: ${user2}, id: ${id2}")
message("user: ${user3}, id: ${id3}")

set(EXE_NAME "ex1.${user1}")
add_executable( ${EXE_NAME}
	${SOURCE_FILES}
)
#BUILD_BINARY_OUTPUT_DIRECTORY
set_property(TARGET ${EXE_NAME} PROPERTY FOLDER "ex1/${user1}")
set_property(TARGET ${EXE_NAME} PROPERTY OUTPUT_NAME "${user1}")