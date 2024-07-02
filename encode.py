from PIL import Image

image = Image.open("source/test_image.jpg").resize((300, 300)) # for testing purposes, resized to 300 by 300
out_file = open("raw_images/test_image.raw", "wb")

width = image.width
height = image.height

# add width and height to file as uint32_t
out_file.write(width.to_bytes(4, "little"))
out_file.write(height.to_bytes(4, "little"))

# add pixel data to file as 3 uint8_t, in row major format
for y in range(0, height):
    for x in range(0, width):
        pixel = image.getpixel((x, y))
        out_file.write(pixel[0].to_bytes(1, "little")) # red
        out_file.write(pixel[1].to_bytes(1, "little")) # green
        out_file.write(pixel[2].to_bytes(1, "little")) # blue

# close file safely when done, and notify
out_file.close()
print("Finished generating raw file.")