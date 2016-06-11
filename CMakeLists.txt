cmake_minimum_required(VERSION 3.1)

project(ogb2 CXX)

set (CMAKE_CXX_STANDARD 14)

set(CMAKE_BUILD_TYPE Debug)

set(interfaces src/interfaces/menu.cpp src/interfaces/game.cpp)
set(widgets src/widgets/textbox.cpp src/widgets/button.cpp src/widgets/panel.cpp src/widgets/label.cpp)

include_directories(extern)
add_executable(ogb2 src/main.cpp src/renderer.cpp src/vector2.cpp ${interfaces} src/interface.cpp ${widgets} src/widget.cpp src/player.h src/entity.cpp)
target_link_libraries(ogb2 allegro allegro_font allegro_ttf allegro_primitives allegro_image allegro_color sioclient curl)
