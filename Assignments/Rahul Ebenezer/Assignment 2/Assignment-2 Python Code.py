import random
temperature=random.randint(1,50)
print('Temperature=',temperature,'deg C')
humidity=random.randint(1,100)
print('Humidity',humidity,'%')
if 20<temperature<35 and 20<humidity<60:
    print('No Hazard Detected')
else:
    print('Hazard Detected')
