from PIL import Image
import sys

raw_file = open(sys.argv[1], "rb")

width = int.from_bytes(raw_file.read(4), byteorder="little")
height = int.from_bytes(raw_file.read(4), byteorder="little")

output_img = Image.new(mode="RGB", size=(width, height))

for y in range(0, height):
    for x in range(0, width):
        output_img.putpixel((x, y), (int.from_bytes(raw_file.read(1)), int.from_bytes(raw_file.read(1)), int.from_bytes(raw_file.read(1))))

raw_file.close()
output_img.show()