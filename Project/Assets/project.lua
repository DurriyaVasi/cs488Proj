
rootnode = gr.node('root')
rootnode:scale( 0.5, 0.5, 0.5 )
rootnode:translate(0.0, 0.0, -1.0)

background = gr.background("./Assets/starfield_rt.tga", "./Assets/starfield_lf.tga", "./Assets/starfield_up.tga", "./Assets/starfield_dn.tga", "./Assets/starfield_bk.tga", "./Assets/starfield_ft.tga", 1);

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10, 1.0)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10, 0.5)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10, 1.0)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10, 1.0)
glass = gr.material({0.9333, 0.9137, 0.9137}, {0.99, 0.99, 0.99}, 20, 0.5)
grey = gr.material({0.5, 0.5, 0.5}, {0.99, 0.99, 0.99}, 20, 1.0)

noTexture = gr.texture(0, '', 0, '')
brickBumpsTexture = gr.texture(1, './Assets/brickwall.jpg', 1, './Assets/brickwall_normal.jpg')
brickTexture = gr.texture(1, './Assets/brickwall.jpg', 0, '')
startTexture = gr.texture(1, './Assets/startButton.jpg', 0, '')

alienHead = gr.mesh('sphere', 'alienHead')
rootnode:add_child(alienHead)
alienHead:scale(0.30, 0.45, 0.25)
alienHead:translate(0, 0.8, 0)
alienHead:set_material(green)
alienHead:set_texture(noTexture)

--alientLeftEye = gr.mesh('sphere', 'alientLeftEye')
--rootnode:add_achild(alienLeftEye)
--alientLeftEye:scale(0.1, 0.1, 0.1)
--alienLeftEye:translate(-20.5, 20.0, 15.0)
--alientLeftEye:set_material(

ufoGlass = gr.mesh('sphere', 'ufoGlass')
rootnode:add_child(ufoGlass)
ufoGlass:translate( 0, 0.5, 0.0)
ufoGlass:set_material(glass)
ufoGlass:set_texture(noTexture)

ufoBottom = gr.mesh('sphere', 'ufoBottom')
rootnode:add_child(ufoBottom)
ufoBottom:scale(2.0, 0.5, 2.0)
ufoBottom:set_material(grey)
ufoBottom:set_texture(noTexture)

startButton = gr.mesh('square', 'startButton')
startButton:scale(0.75, 2.0, 0.75)
startButton:rotate('z', 90)
startButton:set_material(red)
startButton:set_texture(startTexture)

ballRoot = gr.mesh('sphere', 'ballRoot')
ballRoot:scale( 0.1, 0.1, 0.1)
ballRoot:set_texture(noTexture)
ballRoot:set_material(red)

paddleRoot = gr.mesh('cube', 'paddleRoot')
paddleRoot:scale(0.5, 0.1, 0.1)
paddleRoot:set_texture(noTexture)
paddleRoot:set_material(blue)


return {r=rootnode, b=background, q=ballRoot, p=paddleRoot, s=startButton}
