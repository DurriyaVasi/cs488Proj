-- puppet.lua
-- A simplified puppet without posable joints, but that
-- looks roughly humanoid.

rootnode = gr.node('root')
rootnode:scale( 0.25, 0.25, 0.25 )
rootnode:translate(0.0, 0.0, -1.0)

rootRotateNode = gr.node('rootRotate')
rootnode:add_child(rootRotateNode)

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)

torso = gr.node('torso')
rootRotateNode:add_child(torso)

torsoScale = gr.mesh('sphere', 'torso')
torso:add_child(torsoScale)
torsoScale:scale(0.5, 0.8, 0.5)
torsoScale:scale(0.7, 0.7, 0.7)
torsoScale:set_material(white)

shoulder = gr.node('shoulder')
torso:add_child(shoulder)
shoulder:translate(0.0, 0.5, 0.0)

shoulderScale = gr.mesh('sphere', 'shoulderScale')
shoulder:add_child(shoulderScale)
shoulderScale:scale(0.6, 0.13, 0.13)
shoulderScale:set_material(red)

rightUpperArm = gr.node('rightUpperArm')
shoulder:add_child(rightUpperArm)
rightUpperArm:translate(-0.55, -0.3, 0)

rightUpperArmJointMove = gr.node('rightUpperArmJointMove')
rightUpperArm:add_child(rightUpperArmJointMove)
rightUpperArmJointMove :translate(0.5, 0.0, 0.5)

rightUpperArmJoint = gr.joint('rightUpperArmJoint', {-45, 0, 0}, {-45, 0, 0})
rightUpperArmJointMove:add_child(rightUpperArmJoint)

rightUpperArmJointUnmove = gr.node('rightUpperArmJointUnmove')
rightUpperArmJoint:add_child(rightUpperArmJointUnmove)
rightUpperArmJointUnmove:translate(-0.5, 0.0, -0.5)

rightUpperArmScale = gr.mesh('sphere', 'rightUpperArmScale')
rightUpperArmJointUnmove:add_child(rightUpperArmScale)
rightUpperArmScale:scale(0.13, 0.32, 0.13)
rightUpperArmScale:set_material(blue)

--rightForearmJointMove = gr.node('rightForearmJointMove')
--rightUpperArm:add_child(rightForearmJointMove)
--rightForearmJointMove:translate(0.0, -0.3, 0.0)

rightForearm = gr.node('rightForearm')
rightUpperArmJointUnmove:add_child(rightForearm)
rightForearm:translate(0.0, -0.59, 0.0)



rightForearmJoint = gr.joint('rightForearmJoint', {-45, 0, 0}, {-45, 0, 0})
rightForearm:add_child(rightForearmJoint)

--rightForearmJointUnmove = gr.node('rightForearmJointUnmove')
--rightForearmJoint:add_child(rightForearmJointUnmove)
--rightForearmJointUnmove:translate(0.0, 0.3, 0.0)

rightForearmScale = gr.mesh('sphere', 'rightForearmScale')
rightForearmJoint:add_child(rightForearmScale)
rightForearmScale:scale(0.07, 0.27, 0.07)
rightForearmScale:set_material(blue)

--rightHandJointMove = gr.node('rightHandJointMove')
--rightForearm:add_child(rightHandJointMove)
--rightHandJointMove:translate(-0.5, 0.0, 0.0)

rightHand = gr.node('rightHand')
rightForearmJoint:add_child(rightHand)
rightHand:translate(0.0, -0.31, 0.0)

rightHandJoint = gr.joint('rightHandJoint', {-45, 0, 0}, {-45, 0, 0})
rightHand:add_child(rightHandJoint)

--rightHandJointUnmove = gr.node('rightHandJointUnmove')
--rightHandJoint:add_child(rightHandJointUnmove)
--rightHandJointUnmove:translate(0.5, 0.0, 0.0)

rightHandScale = gr.mesh('sphere', 'rightHandScale')
rightHandJoint:add_child(rightHandScale)
rightHandScale:scale(0.05, 0.05, 0.05)
rightHandScale:set_material(blue)

return rootnode
