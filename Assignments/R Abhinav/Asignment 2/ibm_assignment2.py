import random
temp=random.randrange(0,100,1)
print("Temperature:",temp)
humidity=random.randrange(0,100,1)
print("Humidity:",humidity)
if(temp>60 and humidity<30):
    print(" Fire Hazard Detected")
