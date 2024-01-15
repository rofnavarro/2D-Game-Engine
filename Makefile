NAME				=				ge_v1

CC					=				g++

CCFLAGS				=				-Werror -Wextra -std=c++17

GEFLAGS				=				-lglfw -lvulkan -ldl -lpthread \
									-lX11 -lXxf86vm -lXrandr -lXi

SRC					=				main.cpp Game.cpp Window.cpp Pipeline.cpp Device.cpp

OBJ					=				$(SRC:.cpp=.o)

%.o:				%.cpp
					$(CC) $(CCFLAGS) -c $(GEFLAGS) $< -o $@

all:				$(NAME)

$(NAME):			$(OBJ)
					$(CC) $(CCFLAGS) $(OBJ) $(GEFLAGS) -o $(NAME)

clean:
					rm -rf $(OBJ)

fclean:				clean
					rm -rf $(NAME)

re:					fclean all

.PHONY:				all clean fclean re