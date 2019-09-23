
BLENDER=blender

all : ../dist/robot.pnct ../dist/robot.scene ../dist/garden.pnct ../dist/garden.scene ../dist/city.pnct ../dist/city.scene #hexapod.pnct hexapod.scene

../dist/city.pnct : city.blend export-meshes.py
	$(BLENDER) --background --python export-meshes.py -- city.blend:Parts '$@'

../dist/city.scene : city.blend export-scene.py
	$(BLENDER) --background --python export-scene.py -- city.blend:Scene '$@'
../dist/robot.pnct : robot.blend export-meshes.py
	$(BLENDER) --background --python export-meshes.py -- robot.blend '$@'
../dist/robot.scene : robot.blend export-scene.py
	$(BLENDER) --background --python export-scene.py -- robot.blend '$@'

../dist/garden.pnct : garden.blend export-meshes.py
	$(BLENDER) --background --python export-meshes.py -- garden.blend '$@'
../dist/garden.scene : garden.blend export-scene.py
	$(BLENDER) --background --python export-scene.py -- garden.blend '$@'

hexapod.pnct : hexapod.blend export-meshes.py
	$(BLENDER) --background --python export-meshes.py -- hexapod.blend:Main '$@'

hexapod.scene : hexapod.blend export-scene.py
	$(BLENDER) --background --python export-scene.py -- hexapod.blend:Main '$@'
