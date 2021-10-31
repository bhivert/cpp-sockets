
.PHONY: all depend clean fclean
.SUFFIXES:

NAME		=	sockets_test

CC			=	gcc
CCFLAGS		=	-Wall -Wextra -I$(INCS_DIR)

CXX			=	g++
CXXFLAGS	=	-Wall -Wextra -std=c++11 -I$(INCS_DIR)

LDFLAGS		=
LDLIBS		=

MKDIR		=	mkdir -p
RM			=	rm -rf

SRCS_DIR	=	srcs
INCS_DIR	=	incs
OBJS_DIR	=	objs
DEPS_DIR	=	deps

CC_SRCS		=	
CXX_SRCS	=	main.cpp

CC_INCS		=	$(shell find $(SRCS_DIR) $(INCS_DIR) -type f -name '*.h')
CXX_INCS	=	$(shell find $(SRCS_DIR) $(INCS_DIR) -type f -name '*.hpp')

OBJS		=	$(CC_SRCS:%.c=$(OBJS_DIR)/%.c.o) \
				$(CXX_SRCS:%.cpp=$(OBJS_DIR)/%.cpp.o)

DEPS		=	$(CC_SRCS:%.c=$(DEPS_DIR)/$(SRCS_DIR)/%.c.d) \
				$(CXX_SRCS:%.cpp=$(DEPS_DIR)/$(SRCS_DIR)/%.cpp.d) \
				$(CC_INCS:%.h=$(DEPS_DIR)/%.h.d) \
				$(CXX_INCS:%.hpp=$(DEPS_DIR)/%.hpp.d)

all	:	$(NAME)

$(NAME)	:	$(DEPS) $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $@

$(DEPS_DIR)/$(SRCS_DIR)/%.c.d	:	$(SRCS_DIR)/%.c
	@$(MKDIR) $(@D)
	$(CC) $(CCFLAGS) -MM -MT $(<:$(SRCS_DIR)/%.c=$(OBJS_DIR)/%.c.o) $< -o $@

$(DEPS_DIR)/$(SRCS_DIR)/%.cpp.d	:	$(SRCS_DIR)/%.cpp
	@$(MKDIR) $(@D)
	$(CXX) $(CXXFLAGS) -MM -MT $(<:$(SRCS_DIR)/%.cpp=$(OBJS_DIR)/%.cpp.o) $< -o $@

$(CC_INCS:%.h=$(DEPS_DIR)/%.h.d)	:
	@$(MKDIR) $(@D)
	$(CC) $(CCFLAGS) -MM -MT $@ $(@:$(DEPS_DIR)/%.h.d=%.h) -o $@

$(CXX_INCS:%.hpp=$(DEPS_DIR)/%.hpp.d)	:
	@$(MKDIR) $(@D)
	$(CXX) $(CXXFLAGS) -MM -MT $@ $(@:$(DEPS_DIR)/%.hpp.d=%.hpp) -o $@

depend	:	$(DEPS)

ifeq ($(filter clean fclean, $(MAKECMDGOALS)), )
include $(DEPS)
endif

$(OBJS_DIR)/%.c.o	:	$(SRCS_DIR)/%.c $(DEPS_DIR)/$(SRCS_DIR)/%.c.d
	@$(MKDIR) $(@D)
	$(CC) $(CCFLAGS) -c $< -o $@

$(OBJS_DIR)/%.cpp.o	:	$(SRCS_DIR)/%.cpp $(DEPS_DIR)/$(SRCS_DIR)/%.cpp.d
	@$(MKDIR) $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean	:
	$(RM) $(OBJS_DIR)
	$(RM) $(DEPS_DIR)

fclean	:	clean
	$(RM) $(NAME)
