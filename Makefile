# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/08/17 10:12:54 by mle-faou          #+#    #+#              #
#    Updated: 2022/10/28 19:09:16 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

################################################################################
#								  COMPILATION								   #
################################################################################

NAME			=	webserv

SRCS			=	$(SRCS_DIR)/utils/utils.cpp \
					$(SRCS_DIR)/main.cpp \
					$(SRCS_DIR)/Server/Response/Response/Header/Response_Header.cpp \
					$(SRCS_DIR)/Server/Response/Response/Response.cpp \
					$(SRCS_DIR)/Server/Response/Response/Content/Response_Content.cpp \
					$(SRCS_DIR)/Server/Response/AutoIndex/AutoIndex.cpp \
					$(SRCS_DIR)/Server/Response/Methods/Methods.cpp \
					$(SRCS_DIR)/Server/Response/CGI/CGI.cpp \
					$(SRCS_DIR)/Server/Response/HtmlChecker/HTMLChecker.cpp \
					$(SRCS_DIR)/Server/Response/PostMethod/PostMethod.cpp \
					$(SRCS_DIR)/Server/Request/ChunkProcess/ChunkProcess.cpp \
					$(SRCS_DIR)/Server/Request/RequestConfig.cpp \
					$(SRCS_DIR)/Server/Request/Request.cpp \
					$(SRCS_DIR)/Server/Extensions/Extensions.cpp \
					$(SRCS_DIR)/Server/Cluster/Cluster.cpp \
					$(SRCS_DIR)/Server/Server.cpp \
					$(SRCS_DIR)/Server/Locations/NewLocations.cpp \
					$(SRCS_DIR)/Server/Locations/Locations.cpp \
					$(SRCS_DIR)/Server/Locations/LocationsUtils.cpp \
					$(SRCS_DIR)/Server/Locations/NewLocationsUtils.cpp \
					$(SRCS_DIR)/Server/RateLimit/RateLimit.cpp \
					$(SRCS_DIR)/ServersConfig/checkVar.cpp \
					$(SRCS_DIR)/ServersConfig/ServersConfig.cpp


SRCS_DIR		=	./srcs

OBJS			=	$(SRCS:.cpp=.o)
CFLAGS			=	-std=c++98 -g3 -Wall -Wextra -Werror

CXX				=	clang++

PRINT_COMP		=	printf "\r- [$(_BOLD)$(_YELLOW)%s]" $@

%.o : %.cpp
	@printf "\r                                                               "
	@$(PRINT_COMP) Compiling $@
	@$(CXX) $(CFLAGS) -I. -c $< -o ${<:.cpp=.o}

################################################################################
#								 	 COLORS									   #
################################################################################

# This is a minimal set of ANSI/VT100 color codes
_END=$'\033[0m
_BOLD=$'\033[1m
_UNDER=$'\033[4m
_REV=$'\033[7m

# Colors
_GREY=$'\033[30m
_RED=$'\033[31m
_GREEN=$'\033[32m
_YELLOW=$'\033[33m
_BLUE=$'\033[34m
_PURPLE=$'\033[35m
_CYAN=$'\033[36m
_WHITE=$'\033[37m

# Inverted, i.e. colored backgrounds
_IGREY=$'\033[40m
_IRED=$'\033[41m
_IGREEN=$'\033[42m
_IYELLOW=$'\033[43m
_IBLUE=$'\033[44m
_IPURPLE=$'\033[45m
_ICYAN=$'\033[46m
_IWHITE=$'\033[47m

################################################################################
#								       RULES								   #
################################################################################

all:
	@printf "$(_BOLD)$(_GREEN):::       ::: :::::::::: :::::::::   ::::::::  :::::::::: :::::::::  :::     :::\n"
	@printf ":+:       :+: :+:        :+:    :+: :+:    :+: :+:        :+:    :+: :+:     :+:\n"
	@printf "+:+       +:+ +:+        +:+    +:+ +:+        +:+        +:+    +:+ +:+     +:+\n"
	@printf "+#+  +:+  +#+ +#++:++#   +#++:++#+  +#++:++#++ +#++:++#   +#++:++#:  +#+     +:+\n"
	@printf "+#+ +#+#+ +#+ +#+        +#+    +#+        +#+ +#+        +#+    +#+  +#+   +#+ \n"
	@printf " #+#+# #+#+#  #+#        #+#    #+# #+#    #+# #+#        #+#    #+#   #+#+#+#  \n"
	@printf "  ###   ###   ########## #########   ########  ########## ###    ###     ###    $(_END)\n"
	@printf "$(_PURPLE)Compiling $(NAME) . . . $(_END)\n"
	@make $(NAME)

$(NAME): $(OBJS)
	@printf "\r                                                               "
	@printf "\r$(_BOLD)$(_GREEN)Everything has been compiled correctly !\n"
	@$(CXX) $(OBJS) $(CFLAGS) -o $(NAME)
	@printf "$(_BOLD)$(_GREEN)   ___                  __\n"
	@printf "  / _ \___  ___  ___   / /\n"
	@printf " / // / _ \/ _ \/ -_) /_/\n"
	@printf "/____/\___/_//_/\__/ (_)$(_END)\n"

clean:
	@printf "$(_PURPLE)Deleting pre-compilation files for $(NAME) . . . $(_END)\n"
	@/bin/rm -f $(OBJS)

fclean: clean
	@printf "$(_PURPLE)Deleting executable for $(NAME) . . . $(_END)\n"
	@/bin/rm -f Client
	@/bin/rm -f $(NAME)

re: fclean all

setup_default: all
	@touch -f webSite/d_entry/1
	@touch -f webSite/d_entry/delete/2
	@touch -f webSite/d_entry/delete/to_delete.tmp
	@touch -f webSite/d_entry/delete/toto
	@touch -f webSite/d_entry/delete/no_permission && chmod 000 webSite/d_entry/delete/no_permission

default_config: re
	@./$(NAME)

siege: all
	@x-terminal-emulator -x "./webserv ./tests/config_simple_linux.wscfg"
	@sleep 1
	@siege -b 127.0.0.1:8081

check_leaks: all
	@x-terminal-emulator -x "sleep 2 && timeout 10 siege -b 127.0.0.1:8081"
	@valgrind ./webserv ./tests/config_simple_linux.wscfg

stress_test_availability: all
	@x-terminal-emulator -x "sleep 3 && timeout 15 curl -F \""data=@tests/Upload/image_2K.jpg\"" 127.0.0.1:8081/post"
	@x-terminal-emulator -x "sleep 3 && timeout 15 curl -F \""data=@tests/Upload/image_2K.jpg\"" 127.0.0.1:8081/post"
	@x-terminal-emulator -x "sleep 3 && timeout 15 curl -F \""data=@tests/Upload/image_2K.jpg\"" 127.0.0.1:8081/post"
	@x-terminal-emulator -x "sleep 3 && timeout 15 curl -F \""data=@tests/Upload/image_2K.jpg\"" 127.0.0.1:8081/post"
	@x-terminal-emulator -x "sleep 3 && timeout 15 curl -F \""data=@tests/Upload/image_2K.jpg\"" 127.0.0.1:8081/post"
	@x-terminal-emulator -x "sleep 3 && timeout 15 curl -F \""data=@tests/Upload/image_2K.jpg\"" 127.0.0.1:8081/post"
	@x-terminal-emulator -x "sleep 3 && timeout 15 curl -F \""data=@tests/Upload/image_2K.jpg\"" 127.0.0.1:8081/post"
	@x-terminal-emulator -x "./webserv ./tests/config_simple_linux.wscfg"
	@sleep 2 && timeout 20 siege -b 127.0.0.1:8081

client:
	@$(CXX) $(CFLAGS) tests/WebServClient.cpp -o Client

.PHONY: all clean fclean re default_config client siege check_leaks stress_test_availability
