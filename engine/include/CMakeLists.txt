# Create headers list
list(APPEND JC_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/Attack.h
    ${CMAKE_CURRENT_LIST_DIR}/Board.h
    ${CMAKE_CURRENT_LIST_DIR}/FindMagic.h
    ${CMAKE_CURRENT_LIST_DIR}/Magic.h
    ${CMAKE_CURRENT_LIST_DIR}/PieceTable.h
    ${CMAKE_CURRENT_LIST_DIR}/types.h
    ${CMAKE_CURRENT_LIST_DIR}/BBoard.h
    ${CMAKE_CURRENT_LIST_DIR}/Game.h
    ${CMAKE_CURRENT_LIST_DIR}/Move.h
    ${CMAKE_CURRENT_LIST_DIR}/Transposition.h
    ${CMAKE_CURRENT_LIST_DIR}/Zobrist.h)

# Return the lists to the parent scope
set(JC_HEADERS ${JC_HEADERS} PARENT_SCOPE)
