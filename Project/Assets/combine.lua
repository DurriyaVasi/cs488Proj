
rootnode = gr.node('root')
rootnode:scale( 0.5, 0.5, 0.5 )
rootnode:translate(0.0, 0.0, -1.0)

background = gr.background("./Assets/starfield_bk.tga", "./Assets/starfield_dn.tga", "./Assets/starfield_ft.tga", "./Assets/starfield_lf.tga", "./Assets/starfield_rt.tga", "./Assets/starfield_up.tga", 1);

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)

noTexture = gr.texture(0, '', 0, '')
brickBumpsTexture = gr.texture(1, './Assets/brickwall.jpg', 1, './Assets/brickwall_normal.jpg')
brickTexture = gr.texture(1, './Assets/brickwall.jpg', 0, '')

box = gr.mesh('cube', 'box')
rootnode:add_child(box)
box:rotate('y', 45)
box:translate(1.0, 0, -0.5)
box:set_material(red)
box:set_texture(brickBumpsTexture)

box2 = gr.mesh('cube', 'box2')
rootnode:add_child(box2)
box2:rotate('y', 45)
box2:translate(-1.0, 0, -0.5)
box2:set_material(red)
box2:set_texture(brickTexture)

ball = gr.mesh('sphere', 'ball')
rootnode:add_child(ball)
ball:translate(0, 0, -5.0)
ball:set_material(red)
ball:set_texture(noTexture)

return {r=rootnode, b=background}
