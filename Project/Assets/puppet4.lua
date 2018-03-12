-- puppet.lua
-- A simplified puppet without posable joints, but that
-- looks roughly humanoid.

rootnode = gr.node('root')
rootnode:scale( 0.25, 0.25, 0.25 )
rootnode:translate(0.0, 0.0, -1.0)

rootRotateNode = gr.node('rootRotate')
rootnode:add_child(rootRotateNode)
--rootRotateNode:rotate('y', 90)

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

rightUpperArmJointMove = gr.node('rightUpperArmJointMove')
shoulder:add_child(rightUpperArmJointMove)
rightUpperArmJointMove :translate(-0.55, -0.03, 0.0)

rightUpperArmJoint = gr.joint('rightUpperArmJoint', {0, 0, 0}, {-90, 0, 0})
rightUpperArmJointMove:add_child(rightUpperArmJoint)

rightUpperArm = gr.node('rightUpperArm')
rightUpperArmJoint:add_child(rightUpperArm)
rightUpperArm:translate(0.0, -0.3, 0)

rightUpperArmScale = gr.mesh('sphere', 'rightUpperArmScale')
rightUpperArm:add_child(rightUpperArmScale)
rightUpperArmScale:scale(0.13, 0.32, 0.13)
rightUpperArmScale:set_material(blue)

rightForearmJointMove = gr.node('rightForearmJointMove')
rightUpperArm:add_child(rightForearmJointMove)
rightForearmJointMove:translate(0.0, -0.3, 0.0)

rightForearmJoint = gr.joint('rightForearmJoint', {0, 0, 0}, {-90, 0, 0})
rightForearmJointMove:add_child(rightForearmJoint)

rightForearm = gr.node('rightForearm')
rightForearmJoint:add_child(rightForearm)
rightForearm:translate(0.0, -0.29, 0.0)

rightForearmScale = gr.mesh('sphere', 'rightForearmScale')
rightForearm:add_child(rightForearmScale)
rightForearmScale:scale(0.07, 0.27, 0.07)
rightForearmScale:set_material(blue)

rightHandJointMove = gr.node('rightHandJointMove')
rightForearm:add_child(rightHandJointMove)
rightHandJointMove:translate(0.0, -0.29, 0.0)

rightHandJoint = gr.joint('rightHandJoint', {0, 0, 0}, {-90, 0, 0})
rightHandJointMove:add_child(rightHandJoint)

rightHand = gr.node('rightHand')
rightHandJoint:add_child(rightHand)
rightHand:translate(0.0, -0.02, 0.0)

rightHandScale = gr.mesh('sphere', 'rightHandScale')
rightHand:add_child(rightHandScale)
rightHandScale:scale(0.05, 0.05, 0.05)
rightHandScale:set_material(blue)

hip = gr.node('hip')
torso:add_child(hip)
hip:translate(0.0, -0.55, 0.0)

hipScale = gr.mesh('sphere', 'hipScale')
hip:add_child(hipScale)
hipScale:scale(0.35, 0.15, 0.15) 
hipScale:set_material(green)

rightThighJointMove = gr.node('rightThighJointMove')
hip:add_child(rightThighJointMove)
rightThighJointMove:translate(-0.20, -0.03, 0.0)

rightThighJoint = gr.joint('rightThighJoint', {0, 0, 0}, {-45, 0 , 0})
rightThighJointMove:add_child(rightThighJoint)

rightThigh = gr.node('rightThigh')
rightThighJoint:add_child(rightThigh)
rightThigh:translate(0.0, -0.40, 0.0)

rightThighScale = gr.mesh('sphere', 'rightThighScale')
rightThigh:add_child(rightThighScale)
rightThighScale:scale(0.13, 0.40, 0.13)
rightThighScale:set_material(blue)

rightCalfJointMove = gr.node('rightCalfJointMove')
rightThigh:add_child(rightCalfJointMove)
rightCalfJointMove:translate(0.0, -0.40, 0.0)

rightCalfJoint = gr.joint('rightCalfJoint', {0, 0, 0}, {-45, 0 , 45})
rightCalfJointMove:add_child(rightCalfJoint)

rightCalf = gr.node('rightCalf')
rightCalfJoint:add_child(rightCalf)
rightCalf:translate(0.0, -0.34, 0.0)

rightCalfScale = gr.mesh('sphere', 'rightCalfScale')
rightCalf:add_child(rightCalfScale)
rightCalfScale:scale(0.13, 0.34, 0.13)
rightCalfScale:set_material(blue)

rightFootJointMove = gr.node('rightFootJointMove')
rightCalf:add_child(rightFootJointMove)
rightFootJointMove:translate(0.0, -0.34, 0.0)

rightFootJoint = gr.joint('rightFootJoint', {0, 0, 0}, {-135, -90, -90})
rightFootJointMove:add_child(rightFootJoint)

rightFoot = gr.node('rightFoot')
rightFootJoint:add_child(rightFoot)
rightFoot:translate(0.0, -0.20, 0.0)

rightFootScale = gr.mesh('sphere', 'rightFootScale')
rightFoot:add_child(rightFootScale)
rightFootScale:scale(0.05, 0.20, 0.05)
rightFootScale:set_material(green) 

leftUpperArmJointMove = gr.node('leftUpperArmJointMove')
shoulder:add_child(leftUpperArmJointMove)
leftUpperArmJointMove :translate(0.55, -0.03, 0.0)

leftUpperArmJoint = gr.joint('leftUpperArmJoint', {0, 0, 0}, {0, 0, 90})
leftUpperArmJointMove:add_child(leftUpperArmJoint)

leftUpperArm = gr.node('leftUpperArm')
leftUpperArmJoint:add_child(leftUpperArm)
leftUpperArm:translate(0.0, -0.3, 0)

leftUpperArmScale = gr.mesh('sphere', 'leftUpperArmScale')
leftUpperArm:add_child(leftUpperArmScale)
leftUpperArmScale:scale(0.13, 0.32, 0.13)
leftUpperArmScale:set_material(blue)

leftForearmJointMove = gr.node('leftForearmJointMove')
leftUpperArm:add_child(leftForearmJointMove)
leftForearmJointMove:translate(0.0, -0.3, 0.0)

leftForearmJoint = gr.joint('leftForearmJoint', {0, 0, 0}, {0, 0, 90})
leftForearmJointMove:add_child(leftForearmJoint)

leftForearm = gr.node('leftForearm')
leftForearmJoint:add_child(leftForearm)
leftForearm:translate(0.0, -0.29, 0.0)

leftForearmScale = gr.mesh('sphere', 'leftForearmScale')
leftForearm:add_child(leftForearmScale)
leftForearmScale:scale(0.07, 0.27, 0.07)
leftForearmScale:set_material(blue)

leftHandJointMove = gr.node('leftHandJointMove')
leftForearm:add_child(leftHandJointMove)
leftHandJointMove:translate(0.0, -0.29, 0.0)

leftHandJoint = gr.joint('leftHandJoint', {0, 0, 0}, {0, 0, 90})
leftHandJointMove:add_child(leftHandJoint)

leftHand = gr.node('leftHand')
leftHandJoint:add_child(leftHand)
leftHand:translate(0.0, -0.02, 0.0)

leftHandScale = gr.mesh('sphere', 'leftHandScale')
leftHand:add_child(leftHandScale)
leftHandScale:scale(0.05, 0.05, 0.05)
leftHandScale:set_material(blue)

leftThighJointMove = gr.node('leftThighJointMove')
hip:add_child(leftThighJointMove)
leftThighJointMove:translate(0.20, -0.03, 0.0)

leftThighJoint = gr.joint('leftThighJoint', {0, 0, 0}, {0, 0 , 45})
leftThighJointMove:add_child(leftThighJoint)

leftThigh = gr.node('leftThigh')
leftThighJoint:add_child(leftThigh)
leftThigh:translate(0.0, -0.40, 0.0)

leftThighScale = gr.mesh('sphere', 'leftThighScale')
leftThigh:add_child(leftThighScale)
leftThighScale:scale(0.13, 0.40, 0.13)
leftThighScale:set_material(blue)

leftCalfJointMove = gr.node('leftCalfJointMove')
leftThigh:add_child(leftCalfJointMove)
leftCalfJointMove:translate(0.0, -0.40, 0.0)

leftCalfJoint = gr.joint('leftCalfJoint', {0, 0, 0}, {-45, 0 , 45})
leftCalfJointMove:add_child(leftCalfJoint)

leftCalf = gr.node('leftCalf')
leftCalfJoint:add_child(leftCalf)
leftCalf:translate(0.0, -0.34, 0.0)

leftCalfScale = gr.mesh('sphere', 'leftCalfScale')
leftCalf:add_child(leftCalfScale)
leftCalfScale:scale(0.13, 0.34, 0.13)
leftCalfScale:set_material(blue)

leftFootJointMove = gr.node('leftFootJointMove')
leftCalf:add_child(leftFootJointMove)
leftFootJointMove:translate(0.0, -0.34, 0.0)

leftFootJoint = gr.joint('leftFootJoint', {0, 0, 0}, {90, 90, 135})
leftFootJointMove:add_child(leftFootJoint)

leftFoot = gr.node('leftFoot')
leftFootJoint:add_child(leftFoot)
leftFoot:translate(0.0, -0.20, 0.0)

leftFootScale = gr.mesh('sphere', 'leftFootScale')
leftFoot:add_child(leftFootScale)
leftFootScale:scale(0.05, 0.20, 0.05)
leftFootScale:set_material(green)

neckJointMove = gr.node('neckJointMove')
shoulder:add_child(neckJointMove)
neckJointMove:translate(0.0, 0.10, 0.0)

neckJoint = gr.joint('neckJoint', {-45, 0, 90}, {0, 0, 0});
neckJointMove:add_child(neckJoint)

neck = gr.node('neck')
neckJoint:add_child(neck)
neck:translate(0.0, 0.15, 0.0)

neckScale = gr.mesh('sphere', 'neckScale')
neck:add_child(neckScale)
neckScale:scale(0.10, 0.15, 0.10)
neckScale:set_material(blue)

headJointMove = gr.node('headJointMove')
neck:add_child(headJointMove)
headJointMove:translate(0.0, 0.07, 0.0)

headJoint = gr.joint('headJoint', {0, 0, 0}, {-30, 0, 30})
headJointMove:add_child(headJoint)

headBob = gr.joint('headBob', {-15, 0, 15}, {0, 0, 0})
headJoint:add_child(headBob)

head = gr.node('head')
headBob:add_child(head)
head:translate(0.0, 0.3, 0.0)

headScale = gr.mesh('sphere', 'headScale')
head:add_child(headScale)
headScale:scale(0.3, 0.3, 0.3)
headScale:set_material(green)

leftEye = gr.mesh('sphere', 'leftEye')
head:add_child(leftEye)
leftEye:scale(0.05, 0.05, 0.05)
leftEye:translate(0.1, 0.1, 0.0)
leftEye:set_material(red)

rightEye = gr.mesh('sphere', 'rightEye')
head:add_child(rightEye)
rightEye:scale(0.05, 0.05, 0.05)
rightEye:translate(-0.1, 0.1, 0.0) 
rightEye:set_material(red)

return rootnode
