NAME      := ft_ping
CC        := cc
CFLAGS    := -Wall -Wextra -Werror -MMD -MP
INCLUDES  := -Iincludes
SRC_DIR   := srcs
OBJ_DIR   := objs

SRCS      := main.c args.c resolve.c socket.c packet.c checksum.c loop.c stats.c utils.c
OBJS      := $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))
DEPS      := $(OBJS:.o=.d)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ -lm

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re
