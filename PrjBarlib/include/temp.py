import math
def angle(x1,y1, x2,y2):
	dot = x1*x2 + y1*y2	  # dot product
	det = x1*y2 - y1*x2	  # determinant
	angle = math.atan2(det, dot)  # atan2(y, x) or atan2(sin, cos)
	if (angle < 0):
		angle += 2 * math.pi
	print(angle / math.pi * 180)


def fulAngle(prev, center, new):
	prev = (prev[0] - center[0],prev[1] - center[1])
	new = (new[0] - center[0], new[1] - center[1])
	angle(prev[0], prev[1], new[0], new[1])

fulAngle((179,62), (188,56), (186,61)) #0
fulAngle((179,62), (188,56), (189,55)) #1

fulAngle((188,56), (186,61), (179,62)) #2
fulAngle((188,56), (186,61), (189,55)) #3

print(2.9441971778869629 * 180 / math.pi) # 1
print(5.6808981895446777 * 180 / math.pi) #0

k  = 0
wid = 122
for i in range(wid - 1):
	for j in range(i + 1, wid):
		assert(i != j)
		k += 1

print(k)
print(wid * (wid + 1) / 2 - wid)