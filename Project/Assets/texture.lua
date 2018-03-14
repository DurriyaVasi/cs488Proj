
rootnode = gr.node('root')
rootnode:scale( 0.25, 0.25, 0.25 )
rootnode:translate(0.0, 0.0, -1.0)

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)

noTexture = gr.texture(0, '');
brickTexture = gr.texture(1, './Assets/bricks.jpg');

box = gr.mesh('cube', 'box')
rootnode:add_child(box)
box:set_material(red)
box:set_texture(brickTexture)

return rootnode