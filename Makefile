NAME := particle
CC := g++
LDFLAGS := -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
CPPFLAGS := -Wall -Wextra -Werror -MMD --std c++17 -DNEBUG

OBJDIR := obj
SRCDIR := src

SRCS := main.cpp \
		ParticleSystemApplication.cpp
# 		Shader.cpp \
# 		Camera.cpp \
# 		FPSCounter.cpp \
# 		Engine/AEngine.cpp \
# 		Engine/EngineStatic.cpp \
# 		Engine/EngineGen.cpp \
# 		glad/gl.cpp


OBJS := $(patsubst %.cpp,$(OBJDIR)/%.o, $(strip $(SRCS)))
DEPS := $(patsubst %.cpp,$(OBJDIR)/%.d, $(strip $(SRCS)))


INCS := ./include/

_GREY		= \033[30m
_RED		= \033[31m
_GREEN		= \033[32m
_YELLOW		= \033[33m
_BLUE		= \033[34m
_PURPLE		= \033[35m
_CYAN		= \033[36m
_WHITE		= \033[37m
_NO_COLOR	= \033[0m


all : $(NAME)

debug: CPPFLAGS += -g -UNDEBUG
debug: $(NAME)

$(NAME): $(OBJS) Makefile
	$(CC) $(CPPFLAGS) -o $(NAME) $(OBJS) $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@if [ ! -d $(dir $@) ]; then \
		mkdir -p $(dir $@); \
	fi
	$(CC) $(CPPFLAGS) -o $@ -c $< -I$(INCS)


-include $(DEPS)

clean:
	rm -rfd $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all


.PHONY: re fclean clean all debug
