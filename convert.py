from PIL import Image

filename = 'image'

im = Image.open(f"{filename}.ppm")
im.save(f"{filename}.png")
