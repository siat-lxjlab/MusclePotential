import matplotlib.pyplot as plt

x = 0
x_ = list()
y = x**2
y_ = list()
while True:
    x += 1
    if x == 100:
        break
    x_.append(x)
    y_.append(x**2)

plt.figure(1)
plt.plot(x_, y_, '-r')
plt.show()
