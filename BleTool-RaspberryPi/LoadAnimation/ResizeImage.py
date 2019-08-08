from PIL import Image

im_name = 'load_1.gif'
im = Image.open(im_name)
out = im.resize((100,100), Image.ANTIALIAS)
out.save('load_a.gif', 'gif')
