NAME := particle-engine
CC := g++
LDFLAGS := -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
CPPFLAGS := -Wall -Wextra -Werror -MMD --std c++17 -DNDEBUG

OBJDIR := obj
SRCDIR := src

SRCS := main.cpp \
		app/ParticleSystemApplication.cpp \
		app/Pipelines/ComputePipeline.cpp \
		app/Pipelines/RenderingPipeline.cpp \
		app/Pipelines/ShadowmapPipeline.cpp \
		app/Objects/Moon.cpp \
		app/Objects/Rings.cpp \
		app/ParticleSystemUtils.cpp \
		app/Swapchain.cpp \
		app/Window.cpp


OBJS := $(patsubst %.cpp,$(OBJDIR)/%.o, $(strip $(SRCS)))
DEPS := $(patsubst %.cpp,$(OBJDIR)/%.d, $(strip $(SRCS)))

SHADERDIR := shaders

SHADERSRCS := triangle/shader.vert \
			  triangle/shader.frag \
			  particle/shader.vert \
			  particle/shader.frag \
			  particle/init.comp \
			  particle/update.comp \
			  shadows/model.vert \
			  shadows/particle.vert \
			  shadows/shader.frag

SHADERSPV := $(patsubst %,$(SHADERDIR)/%.spv, $(strip $(SHADERSRCS)))


INCS := ./include/ 
PATH_TO_STB := /usr/include

_GREY		= \033[30m
_RED		= \033[31m
_GREEN		= \033[32m
_YELLOW		= \033[33m
_BLUE		= \033[34m
_PURPLE		= \033[35m
_CYAN		= \033[36m
_WHITE		= \033[37m
_NO_COLOR	= \033[0m


all : shader $(NAME)

debug: CPPFLAGS += -g -UNDEBUG
debug: shader $(NAME)

$(NAME): $(OBJS) Makefile
	$(CC) $(CPPFLAGS) -o $(NAME) $(OBJS) $(LDFLAGS) -I$(PATH_TO_STB)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@if [ ! -d $(dir $@) ]; then \
		mkdir -p $(dir $@); \
	fi
	$(CC) $(CPPFLAGS) -o $@ -c $< -I$(INCS) -I$(PATH_TO_STB)

$(SHADERDIR)/%.spv: $(SHADERDIR)/%
	glslangValidator -V $< -o $@ -gVS

shader: $(SHADERSPV)

shaderclean:
	rm -f $(SHADERSPV)

clean:
	rm -rfd $(OBJDIR)

fclean: clean shaderclean
	rm -f $(NAME)

re: fclean all

-include $(DEPS)


.PHONY: re fclean clean all debug
