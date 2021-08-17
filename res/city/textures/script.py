import os
listofFiles=os.popen("ls").read().split('\n')
# print(listofFiles)
for file in listofFiles:
    if file.endswith(".tif") or file.endswith(".webp"):
        print(file)
        os.system(f"magick convert \"{file}\" \"{file.split('.')[0] + '.png'}\"")