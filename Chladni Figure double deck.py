from visual import*
from PIL import Image
import time
import os

# import imageio

print("    --Notifications-- \n1. Remember not to put this code into folders named with space.\n   Example: \"C:\\somthing good\\chladi.py\" is not OK \n2. Evertime u excute this code the old photos will be deleted, \n   so make sure u have made a backup.\n3. Plz compose those Photos into video urself cuz we dont know how to do that. \n4. It usually takes quite lot time to generate photos. \n5. This program is still under developing ...maybe\n")
os.system("pause")
os.system("cls")


now_position = os.getcwd()

os.system("mkdir Photos")
photo_cleaner = open(now_position + '\\Photos\\!_nuclear_bomb_!.bat','w')
photo_cleaner.write("@echo off \nerase " + now_position + "\\Photos\\*.png\necho NUCLEAR LAUNCH DETECTED \necho SILO DESTROYED \necho  --Old Photos Deleted--")
photo_cleaner.close()
os.system( now_position+'\\Photos\\!_nuclear_bomb_!.bat' )
print("\n --Start generating new photos now--")


output_mode = "scene_False"  # "scene_True" or "scene_False"
color_mode = "color_height"  # "color_height" or "color_nodal_line"
frequency = 2		# Hz
plate_length = 25
test_x, test_y =12, 12

if output_mode == "scene_True" :
	scene = display(autoscale = 0, forward = (1, -0.7, 0.5), width = 900, height = 700)

output_array = asarray(dtype=dtype('uint8'), a=zeros((plate_length, plate_length, 3)) )

def spring_f(i, j, i0, j0, ball_layer1, ball_layer0, same_or_not):
	if same_or_not == 1:
		length0 = 4 * sqrt((i-i0)**2 + (j-j0)**2)
	else:
		length0 = 4 * sqrt(1 + (i-i0)**2 + (j-j0)**2)
	k = 1000
	return k * (abs(ball_layer1[i][j].pos - ball_layer0[i0][j0].pos) - length0) * norm(ball_layer1[i][j].pos - ball_layer0[i0][j0].pos)

class block_unit():
	def __init__(self, pos, radius,m,v,a,f):
		self.pos = pos
		self.radius = radius
		self.m = m
		self.v = v
		self.a = a
		self.f = f
		

balls_1 = []
balls_2 = []
#'''
if output_mode == "scene_False" :
	for j in range(plate_length):														#balls_1
		balls_1.append([   block_unit(pos = vector(i*4.0, 0.0, j*4.0), radius = 0.5, m = 1, v = vector(0, 0, 0), a = vector(0, 0, 0), f = vector(0, 0, 0))   for i in range(plate_length)])													#balls_1
		balls_2.append([   block_unit(pos = vector(i*4.0, 4.0, j*4.0), radius = 0.5, m = 1, v = vector(0, 0, 0), a = vector(0, 0, 0), f = vector(0, 0, 0))   for i in range(plate_length)])
	
elif output_mode == "scene_True" :
	plate = box(pos = ((plate_length-1)*2, 0, (plate_length-1)*2), length = (plate_length-1)*4, width = (plate_length-1)*4, height = 0.3, opacity = 0.2, v = vector(0, 0, 0), a = vector(0, 0, 0), f = vector(0, 0, 0))
	for j in range(plate_length):														#balls_1
		balls_1.append([   sphere(pos = vector(i*4, 0, j*4), radius = 0.5, m = 1, v = vector(0, 0, 0), a = vector(0, 0, 0), f = vector(0, 0, 0))   for i in range(plate_length)])
		balls_2.append([   sphere(pos = vector(i*4, 4, j*4), radius = 0.5, m = 1, v = vector(0, 0, 0), a = vector(0, 0, 0), f = vector(0, 0, 0))   for i in range(plate_length)])
	scene.center = (plate_length*2, 0, plate_length*2)

balls_1 = array(balls_1)
balls_2 = array(balls_2)

balls_1[test_x][test_y].color = color.green
balls_2[test_x][test_y].color = color.green
balls_2[test_x][test_y].pos = vector(4*test_x,4 ,4*test_y)



t = 0
dt = 0.001

n = 1 #for outputing
dn = 1

s = 0 #layer counting
ds = 1

while t <= 30 :
	if output_mode == "scene_True":
		rate(2/dt)
	t += dt
	s = 0
	for ball_layer in [balls_1, balls_2]:
		s += 1
		for i in range(plate_length):
			for j in range(plate_length):
				ball_layer[i][j].f = vector(0, 0, 0)
																		#calculate the same layer
				if i != 0  and  j != plate_length-1:
					ball_layer[i][j].f += spring_f(i-1, j+1, i, j, ball_layer, ball_layer, 1)	
				if j != plate_length-1:
					ball_layer[i][j].f += spring_f(i, j+1, i, j, ball_layer, ball_layer, 1)
				if i != plate_length-1  and  j != plate_length-1:
					ball_layer[i][j].f += spring_f(i+1, j+1, i, j, ball_layer, ball_layer, 1)
				if i != plate_length-1:
					ball_layer[i][j].f += spring_f(i+1, j, i, j, ball_layer, ball_layer, 1)
				if i != plate_length-1  and  j != 0:
					ball_layer[i][j].f += spring_f(i+1, j-1, i, j, ball_layer, ball_layer, 1)
				if j != 0:
					ball_layer[i][j].f += spring_f(i, j-1, i, j, ball_layer, ball_layer, 1)
				if i != 0  and  j != 0:
					ball_layer[i][j].f += spring_f(i-1, j-1, i, j, ball_layer, ball_layer, 1)
				if i != 0:
					ball_layer[i][j].f += spring_f(i-1, j, i, j, ball_layer, ball_layer, 1)


				if s == 1:	#calculate the higher layers
					ball_layer[i][j].f += spring_f(i, j, i, j, balls_2, balls_1, 0)

					if i != 0  and  j != plate_length-1:
						ball_layer[i][j].f += spring_f(i-1, j+1, i, j, balls_2, balls_1, 0)	
					if j != plate_length-1:
						ball_layer[i][j].f += spring_f(i, j+1, i, j, balls_2, balls_1, 0)
					if i != plate_length-1  and  j != plate_length-1:
						ball_layer[i][j].f += spring_f(i+1, j+1, i, j, balls_2, balls_1, 0)
					if i != plate_length-1:
						ball_layer[i][j].f += spring_f(i+1, j, i, j, balls_2, balls_1, 0)
					if i != plate_length-1  and  j != 0:
						ball_layer[i][j].f += spring_f(i+1, j-1, i, j, balls_2, balls_1, 0)
					if j != 0:
						ball_layer[i][j].f += spring_f(i, j-1, i, j, balls_2, balls_1, 0)
					if i != 0  and  j != 0:
						ball_layer[i][j].f += spring_f(i-1, j-1, i, j, balls_2, balls_1, 0)
					if i != 0:
						ball_layer[i][j].f += spring_f(i-1, j, i, j, balls_2, balls_1, 0)


				elif s == 2:	#calculate the lower layers
					ball_layer[i][j].f += spring_f(i, j, i, j, balls_1, balls_2, 0)

					if i != 0  and  j != plate_length-1:
						ball_layer[i][j].f += spring_f(i-1, j+1, i, j, balls_1, balls_2, 0)	
					if j != plate_length-1:
						ball_layer[i][j].f += spring_f(i, j+1, i, j, balls_1, balls_2, 0)
					if i != plate_length-1  and  j != plate_length-1:
						ball_layer[i][j].f += spring_f(i+1, j+1, i, j, balls_1, balls_2, 0)
					if i != plate_length-1:
						ball_layer[i][j].f += spring_f(i+1, j, i, j, balls_1, balls_2, 0)
					if i != plate_length-1  and  j != 0:
						ball_layer[i][j].f += spring_f(i+1, j-1, i, j, balls_1, balls_2, 0)
					if j != 0:
						ball_layer[i][j].f += spring_f(i, j-1, i, j, balls_1, balls_2, 0)
					if i != 0  and  j != 0:
						ball_layer[i][j].f += spring_f(i-1, j-1, i, j, balls_1, balls_2, 0)
					if i != 0:
						ball_layer[i][j].f += spring_f(i-1, j, i, j, balls_1, balls_2, 0)


	balls_1[test_x][test_y].pos = vector(4*test_x,2*sin(t*2*pi*frequency), 4*test_y)
	balls_2[test_x][test_y].pos = vector(4*test_x,2*sin(t*2*pi*frequency)+4, 4*test_y)
	
	for i in range(plate_length):
		for j in range(plate_length):
			for ball_layer in [balls_1, balls_2]:
				ball_layer[i][j].a = ball_layer[i][j].f/ball_layer[i][j].m
				ball_layer[i][j].v += ball_layer[i][j].a*dt
				ball_layer[i][j].pos += ball_layer[i][j].v*dt

			if n == 0.025/dt:
				if color_mode == "color_height" :
					if balls_1[i][j].pos.y >=0:
						output_array[i][j][0] = 0
						output_array[i][j][1] = 0
						if int(abs(balls_1[i][j].pos.y)/3*255) < 255:
							output_array[i][j][2] = int(abs(balls_1[i][j].pos.y)/3*255)
						else:
							output_array[i][j][2] = 255
					else:
						if int(abs(balls_1[i][j].pos.y)/3*255) < 255:
							output_array[i][j][0] = int(abs(balls_1[i][j].pos.y)/3*255)
						else:
							output_array[i][j][0] = 255
						output_array[i][j][1] = 0
						output_array[i][j][2] = 0
				
				elif color_mode == "color_nodal_line" :
					if abs(balls_1[i][j].pos.y) >=0.35:
						output_array[i][j][0] = 0
						output_array[i][j][1] = 0
						output_array[i][j][2] = 0
					else:
						output_array[i][j][0] = 0
						output_array[i][j][1] = 255
						output_array[i][j][2] = 0

	if n < 0.025/dt:
		n += 1
	else:	
		n = 1	
		i = Image.fromarray(output_array, mode='RGB')
		i.save(now_position+'\\Photos\\ch' + str(int(t/dt)) + '.png')