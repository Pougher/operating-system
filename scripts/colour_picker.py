import pygame
import pygame_widgets
from pygame_widgets.slider import Slider

pygame.init()
pygame.font.init()

font = pygame.font.SysFont("JetBrains Mono", 30)

surface = pygame.display.set_mode((640, 480))

running = True

CONVERSION = 255.0 / 63.0

red_slider = Slider(surface, 20, 20, 400, 20, min=0, max=63, step=1,
    colour = (50, 0, 0), handleColour = (200, 0, 0))
green_slider = Slider(surface, 20, 80, 400, 20, min=0, max=63, step=1,
    colour = (0, 50, 0), handleColour = (0, 200, 0))
blue_slider = Slider(surface, 20, 140, 400, 20, min=0, max=63, step=1,
    colour = (0, 0, 50), handleColour = (0, 0, 200))

def draw_text(v, colour, location):
    text = font.render(v, True, colour)
    surface.blit(text, location)

def get_hex_value(slider):
    return "{:02x}".format(slider.getValue())

def draw_colour():
    colour = (
        int(red_slider.getValue() * CONVERSION),
        int(green_slider.getValue() * CONVERSION),
        int(blue_slider.getValue() * CONVERSION)
    )
    pygame.draw.rect(
        surface,
        colour,
        pygame.Rect(20, 400, 80, 80)
    )

    draw_text(str(red_slider.getValue()), (255, 255, 255), (450, 10))
    draw_text(str(green_slider.getValue()), (255, 255, 255), (450, 70))
    draw_text(str(blue_slider.getValue()), (255, 255, 255), (450, 130))

    hex_value = '#' + get_hex_value(red_slider) + get_hex_value(green_slider)+\
        get_hex_value(blue_slider)
    draw_text(hex_value, (255, 255, 255), (120, 420))

while running:
    surface.fill((0, 0, 0))
    events = pygame.event.get()
    for event in events:
        if event.type == pygame.QUIT:
            running = False

    draw_colour()

    pygame_widgets.update(events)
    pygame.display.flip()
