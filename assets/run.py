import os
import pathlib

# find current path of file so that the script can run from anywhere
file_path = pathlib.Path(__file__).parent.resolve()

# Blender must be in your PATH!

# run Blender to render sprites
os.system("blender -b {} -a".format(os.path.join(file_path, "punchformer.blend")))

# run Blender to render materials
#os.system("blender -b {} -a".format(os.path.join(file_path, "materials.blend")))
