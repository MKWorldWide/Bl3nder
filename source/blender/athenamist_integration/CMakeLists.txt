# AthenaMist AI Agent Integration for Blender

install(DIRECTORY . 
    DESTINATION ${CMAKE_INSTALL_DATADIR}/blender/athenamist_integration
    FILES_MATCHING PATTERN "*.py" PATTERN "*.png" PATTERN "*.md" PATTERN "requirements.txt" PATTERN "core/*" PATTERN "ui/*" PATTERN "interface/*" PATTERN "models/*" PATTERN "tools/*"
) 