import pygame
import pygame_widgets
import random
import copy

SCREEN_X = 1024
SCREEN_Y = 800
BORDER_SIZE = 100

# converts a given RGB hex code into an RGB tuple
def rgb(hex):
    return ((hex >> 16) & 0xff, (hex >> 8) & 0xff, hex & 0xff)

WHITE           = (255, 255, 255)
LIGHT_BLUE      = rgb(0x6c5eb5)
DARK_BLUE       = rgb(0x352879)

pygame.init()
pygame.font.init()

font = pygame.font.SysFont("JetBrains Mono", 19)
surface = pygame.display.set_mode((SCREEN_X, SCREEN_Y))
clock = pygame.time.Clock()
running = True

# classes
class CharacterGrid:
    def __init__(self, x, y, scale):
        self.grid = [[0 for i in range(8)] for i in range(8)]
        for i in range(8):
            for j in range(8):
                self.grid[j][i] = random.choice([0, 1])

        self.x = x
        self.y = y
        self.scale = scale

        self.cursor_x = 0
        self.cursor_y = 0

    def draw(self):
        # draw the grid first
        for i in range(9):
            pygame.draw.line(
                surface,
                LIGHT_BLUE,
                (i * 8 * self.scale + self.x, self.y),
                (i * 8 * self.scale + self.x, self.y + 64 * self.scale + 1),
                2
            )
            pygame.draw.line(
                surface,
                LIGHT_BLUE,
                (self.x, self.y + i * 8 * self.scale),
                (self.x + 64 * self.scale + 1, self.y + i * 8 * self.scale),
                2
            )

        # draw the cells that are lit up in the grid
        width = 8 * self.scale
        for i in range(8):
            for j in range(8):
                ix = i * 8 * self.scale + self.x
                iy = j * 8 * self.scale + self.y
                if self.grid[j][i]:
                    pygame.draw.rect(
                        surface,
                        LIGHT_BLUE,
                        pygame.Rect(ix, iy, width, width)
                    )

        pygame.draw.line(
            surface,
            WHITE,
            [
                (self.cursor_x * 8 + 2) * self.scale + self.x,
                (self.cursor_y * 8 + 2) * self.scale + self.y
            ],
            [
                (self.cursor_x * 8 + 6) * self.scale + self.x,
                (self.cursor_y * 8 + 6) * self.scale + self.y
            ],
            3
        )

        pygame.draw.line(
            surface,
            WHITE,
            [
                (self.cursor_x * 8 + 2) * self.scale + self.x,
                (self.cursor_y * 8 + 6) * self.scale + self.y
            ],
            [
                (self.cursor_x * 8 + 6) * self.scale + self.x,
                (self.cursor_y * 8 + 2) * self.scale + self.y
            ],
            3
        )

    def move_cursor(self, x, y):
        if (self.cursor_x + x >= 0 and self.cursor_x + x < 8) and \
            (self.cursor_y + y >= 0 and self.cursor_y + y < 8):
            self.cursor_x += x
            self.cursor_y += y

    def flip_colour(self):
        self.grid[self.cursor_y][self.cursor_x] = \
            not self.grid[self.cursor_y][self.cursor_x]

class CharacterHUD:
    def __init__(self, x, y, scale):
        self.x = x
        self.y = y
        self.scale = scale

        self.image = pygame.Surface((8 * 8 * scale, 16 * 8 * scale))

    def draw_character(self, char, x, y):
        for i in range(8):
            for j in range(8):
                pygame.draw.rect(
                    self.image,
                    LIGHT_BLUE if char[j][i] else DARK_BLUE,
                    pygame.Rect(
                        x + i * self.scale,
                        y + j * self.scale,
                        self.scale,
                        self.scale
                    )
                )

    def update_image(self, chars):
        for i in range(8):
            for j in range(16):
                self.draw_character(
                    chars[i + j * 8],
                    i * 8 * self.scale,
                    j * 8 * self.scale
                )

    def draw(self):
        pygame.draw.rect(
            surface,
            LIGHT_BLUE,
            pygame.Rect(
                self.x - 1,
                self.y - 1,
                self.scale * 64 + 6,
                self.scale * 128 + 6,
                width=1
            )
        )
        surface.blit(self.image, (self.x + 2, self.y + 2))

class CommandLine:
    def __init__(self, x, y):
        self.command = ''
        self.enabled = False

        self.filename = 'font.fnt'

        self.valid_commands = {
            'save'          : self.command_save,
            'goto'          : self.command_goto,
            'load'          : self.command_load,
            'generate-c'    : self.command_generate_c
        }

    def command_generate_c(self, arguments, parent):
        char_bytes = [char_to_bytes(i) for i in parent.characters]

        c_string = 'const unsigned char font8x8_basic[128][8] = {\n'
        for i in char_bytes:
            c_string += '    { '
            c_string += ', '.join(['0x{:02x}'.format(x) for x in i])
            c_string += '},\n'

        c_string += '};\n'
        print(c_string)

    def command_save(self, arguments, parent):
        if len(arguments) > 1:
            print("Invalid Syntax for command 'save'")
            return

        if len(arguments) == 1:
            self.filename = arguments[0]

        char_bytes = []
        for i in parent.characters:
            char_bytes.append(char_to_bytes(i))

        file_contents = bytearray([j for sub in char_bytes for j in sub])
        with open(self.filename, 'wb') as f:
            f.write(file_contents)

    def command_load(self, arguments, parent):
        if len(arguments) > 1:
            print("Invalid Syntax for command 'load'")
            return

        if len(arguments) == 1:
            self.filename = arguments[0]

        loaded_data = bytearray([])
        with open(self.filename, 'rb') as f:
            loaded_data = f.read()

        loaded_data_8subdiv = [loaded_data[x:8 + x] \
            for x in range(0, len(loaded_data), 8)]
        chrs = []

        for i in loaded_data_8subdiv:
            chrs.append(bytes_to_char(i))

        parent.load_char_array(chrs)

    def command_goto(self, arguments, parent):
        if len(arguments) > 1 or len(arguments) != 1:
            print("Invalid Syntax for command 'goto'")
            return

        parent.current_char = int(arguments[0])
        parent.update_editor_char()

    def process_command(self, parent):
        tokens = self.command.split(' ')
        if len(tokens) <= 0:
            return

        if tokens[0] not in self.valid_commands.keys():
            print("Command not found: '" + tokens[0] + "'")
            return

        command = copy.deepcopy(tokens[0])

        del tokens[0]
        self.valid_commands[command](tokens, parent)

    def update(self, keycode, key, parent):
        if key == '': return

        if keycode == pygame.K_BACKSPACE:
            if len(self.command) > 0:
                self.command = self.command[0:len(self.command) - 1]
            return

        if key == '\r':
            self.process_command(parent)
            self.command = ''
            self.enabled = False
            return

        self.command += key

    def draw(self):
        text = font.render(self.command, True, WHITE)
        x = BORDER_SIZE + 32
        y = SCREEN_Y - BORDER_SIZE - 40
        font_height = text.get_rect().height

        surface.blit(text, [ x, y ])

        # draw the help text
        draw_text("WASD      = Move cursor", x, y - font_height * 1, WHITE)
        draw_text("SPACE     = Save character", x, y - font_height * 2, WHITE)
        draw_text("ENTER     = Toggle pixel", x, y - font_height * 3, WHITE)
        draw_text("n         = Next character", x, y - font_height * 4, WHITE)
        draw_text("b         = Last character", x, y - font_height * 5, WHITE)
        draw_text("l         = Load character", x, y - font_height * 6, WHITE)
        draw_text("c         = Enter command", x, y - font_height * 7, WHITE)

        if self.enabled:
            pygame.draw.rect(
                surface,
                WHITE,
                pygame.Rect(
                    x + text.get_rect().width,
                    y,
                    12,
                    font_height
                )
            )

class CharacterEditor:
    def __init__(self):
        self.characters = \
            [[[ 0 for i in range(8)] for i in range(8)] for i in range(128)]
        self.current_char = 0

        self.editor = CharacterGrid(BORDER_SIZE + 32, BORDER_SIZE + 32, 5)
        self.hud = CharacterHUD(
            SCREEN_X - BORDER_SIZE - 32 - 4 * 8 * 8 - 4,
            BORDER_SIZE + 32 - 4,
            4
        )
        self.commandline = CommandLine(BORDER_SIZE + 32, 500)

        self.hud.update_image(self.characters)

    def update_editor_char(self):
        self.editor.grid = copy.deepcopy(
            self.characters[self.current_char]
        )

    def load_char_array(self, arr):
        self.characters = copy.deepcopy(arr)
        self.hud.update_image(self.characters)

        self.current_char = 0
        self.editor.grid = copy.deepcopy(
            self.characters[self.current_char]
        )


    def handle_events(self, events):
        global running

        for event in events:
            if event.type == pygame.QUIT:
                running = False
            if event.type == pygame.KEYDOWN:
                if self.commandline.enabled:
                    self.commandline.update(event.key, event.unicode, self)
                elif event.key == pygame.K_c:
                    self.commandline.enabled = True
                elif event.key == pygame.K_w:
                    self.editor.move_cursor(0, -1)
                elif event.key == pygame.K_a:
                    self.editor.move_cursor(-1, 0)
                elif event.key == pygame.K_s:
                        self.editor.move_cursor(0, 1)
                elif event.key == pygame.K_d:
                    self.editor.move_cursor(1, 0)
                elif event.key == pygame.K_RETURN:
                    self.editor.flip_colour()
                elif event.key == pygame.K_SPACE:
                    self.characters[self.current_char] = copy.deepcopy(
                        self.editor.grid
                    )
                    self.hud.update_image(self.characters)
                elif event.key == pygame.K_l:
                    # load the editor with the character indexed by the current
                    # character
                    self.editor.grid = copy.deepcopy(
                        self.characters[self.current_char]
                    )
                elif event.key == pygame.K_n:
                    if self.current_char < 127:
                        self.current_char += 1
                        self.update_editor_char()
                elif event.key == pygame.K_b:
                    if self.current_char > 0:
                        self.current_char -= 1
                        self.update_editor_char()

    def draw(self):
        self.editor.draw()
        self.hud.draw()
        self.commandline.draw()

        character_x = (self.current_char % 8) * 8 * self.hud.scale
        character_y = (self.current_char // 8) * 8 * self.hud.scale

        pygame.draw.rect(
            surface,
            WHITE,
            pygame.Rect(
                self.hud.x + character_x,
                self.hud.y + character_y,
                9 * self.hud.scale,
                9 * self.hud.scale
            ),
            width = 2
        )


# functions

# draws a C64-like border on the surface for aesthetic reasons (it doesnt serve
# a purpose), along with a helper variable that can be reused
border_rect = pygame.Rect(
    BORDER_SIZE,
    BORDER_SIZE,
    (SCREEN_X - BORDER_SIZE * 2),
    (SCREEN_Y - BORDER_SIZE * 2)
)
def draw_screen_border():
    surface.fill(LIGHT_BLUE)
    pygame.draw.rect(surface, DARK_BLUE, border_rect)

# converts a character into a byte array
def char_to_bytes(char):
    ret = []

    for i in char:
        binary = ''.join([str(int(x)) for x in i])
        ret.append(int(binary, 2))
    return ret

# converts the binary representation of a character to its array representation
def bytes_to_char(char):
    c = []
    for i in char:
        c.append([int(x) for x in '{:08b}'.format(i)])
    return c

def draw_text(string, x, y, col):
    text = font.render(string, True, col)
    surface.blit(text, (x, y))

editor = CharacterEditor()

while running:
    draw_screen_border()

    events = pygame.event.get()
    editor.handle_events(events)

    editor.draw()
    pygame.display.flip()

    clock.tick(60)
