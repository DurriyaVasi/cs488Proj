
rootnode = gr.node('root')
rootnode:scale( 0.5, 0.5, 0.5 )
rootnode:translate(0.0, 0.0, -1.0)

background = gr.background("./Assets/starfield_rt.tga", "./Assets/starfield_lf.tga", "./Assets/starfield_up.tga", "./Assets/starfield_dn.tga", "./Assets/starfield_bk.tga", "./Assets/starfield_ft.tga", 1);

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10, 1.0)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10, 0.5)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10, 1.0)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10, 1.0)

noTexture = gr.texture(0, '', 0, '')
brickBumpsTexture = gr.texture(1, './Assets/brickwall.jpg', 1, './Assets/brickwall_normal.jpg')
brickTexture = gr.texture(1, './Assets/brickwall.jpg', 0, '')

box = gr.mesh('cube', 'box')
rootnode:add_child(box)
box:rotate('y', 45)
box:translate(2.0, 0, 0.0)
box:set_material(red)
box:set_texture(brickBumpsTexture)

box2 = gr.mesh('cube', 'box2')
rootnode:add_child(box2)
box2:rotate('y', 45)
box2:translate(-2.0, 0, 0.0)
box2:set_material(red)
box2:set_texture(brickTexture)

ball = gr.mesh('sphere', 'ball')
rootnode:add_child(ball)
ball:translate(0, 0, 0.0)
ball:set_material(red)
ball:set_texture(noTexture)

ball2 = gr.mesh('sphere', 'ball2')
rootnode:add_child(ball2)
ball2:translate(0, -1.0, -2.0)
ball2:set_material(blue)
ball2:set_texture(noTexture)

ballRoot = gr.mesh('sphere', 'ballRoot')
ballRoot:scale( 0.1, 0.1, 0.1)
ballRoot:set_texture(noTexture)
ballRoot:set_material(red)

paddleRoot = gr.mesh('cube', 'paddleRoot')
paddleRoot:scale(0.3, 0.1, 0.1)
paddleRoot:set_texture(noTexture)
paddleRoot:set_material(blue)


return {r=rootnode, b=background, q=ballRoot, p=paddleRoot}
