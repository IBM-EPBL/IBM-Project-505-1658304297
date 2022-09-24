import random
temperature=random.randint(0,100)
print("Temperature:",temperature," degrees in Celsius")
humidity=random.randint(0,100)
print("Humidity:",humidity,"%")
if(temperature > 55 and humidity < 20):
    print(" Fire Hazard has been detected")
