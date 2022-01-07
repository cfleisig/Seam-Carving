from PIL import ImageTk, Image

def write_image(image, filename):
    height = image.height
    width = image.width

    f = open(filename, "wb")

    f.write(height.to_bytes(2, byteorder='big'))
    f.write(width.to_bytes(2, byteorder='big'))
    img_raster = []
    for i in range(height):
        for j in range(width):
            img_raster.extend(image.getpixel((j, i)))

    f.write(bytearray(img_raster))
    f.close()

def read_2bytes(f):
    bytes = f.read(2) # [int(f.read(1)), int(f.read(1))]
    return int.from_bytes(bytes, byteorder = 'big')


def read_image(filename):
    f = open(filename, "rb")
    height = read_2bytes(f)
    width = read_2bytes(f)
    image = Image.new("RGB", (width, height))
    bytes = f.read()
    for i in range(height):
        for j in range(width):
            image.putpixel((j, i), (bytes[3*(i*width + j)+0],
                                    bytes[3*(i*width + j)+1],
                                    bytes[3*(i*width + j)+2]))

    return image

#Write a .png, .jpeg or image files to .bin file
image = Image.open("meric_gertler.jpeg")
write_image(image, "meric_gertler.bin")

# Read image from a bin file, save it to png
image = read_image("carved_meric_gertler.bin")
image.save("carved_meric_gertler.png")

# Write img(#).bin -> img(#).png, for 0 <= (#) < 200
# Use this is you want to carve out a seam that is 200 px wide 
# for i in range(200):
#   image = read_image("img%d.bin" % i)
#   image.save("img%d.png" % i)
