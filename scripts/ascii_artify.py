from PIL import Image

ASCII_VALUES = [' ', '.', ':', '|', ',', '=', '#', '@']
THRESHOLD = int(256 / 8)
THRESHOLD_VALUES = [x for x in range(0, 255, THRESHOLD)]

print(THRESHOLD_VALUES)

img = Image.open('guy.png').convert('L')
width, height = img.size

img.show()

pix = img.load()

def calculate_greyscale_ascii(v):
    if v == 255: return ' '
    if v == 200: return '+'
    for x in range(len(THRESHOLD_VALUES)):
        if THRESHOLD_VALUES[x] >= v:
            return ASCII_VALUES[x]
    return '@'

string = ''
for i in range(0, height):
    string += 'printf("'
    for j in range(0, width):
        string += calculate_greyscale_ascii(pix[j, i])
    string += '\\n");\n'

print(string)
print(pix[15, 8])
