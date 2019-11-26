import matplotlib.pyplot as plt
import pandas as pd

local  = pd.read_csv("local.csv")
remote = pd.read_csv("remote.csv")

fig, ax = plt.subplots(1, 2)

fig.suptitle("Ping Pong")

ax[0].plot(local['length'], local['rate'])
ax[0].set_label("Local")
ax[0].set_xlabel("length")
ax[0].set_ylabel("rate")

ax[1].plot(remote['length'], remote['rate'])
ax[1].set_label("Remote")
ax[1].set_xlabel("length")
ax[1].set_ylabel("rate")

plt.show()