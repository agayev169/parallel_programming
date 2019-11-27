import matplotlib.pyplot as plt
import pandas as pd

local1 = pd.read_csv("local_1.csv")
local2 = pd.read_csv("local_2.csv")
local4 = pd.read_csv("local_4.csv")
local8 = pd.read_csv("local_8.csv")
# remote = pd.read_csv("remote.csv")

fig, ax = plt.subplots(2, 2)

fig.suptitle("Merge Sort - Local")

ax[0][0].plot(local1['time'], local1['arr_size'])
ax[0][0].set_title(f"proc_n = 1")
# ax[0][0].set_title("1")
ax[0][0].set_xlabel("time")
ax[0][0].set_ylabel("arr_size")

ax[0][1].plot(local2['time'], local2['arr_size'])
ax[0][1].set_title(f"proc_n = 2")
ax[0][1].set_xlabel("time")
ax[0][1].set_ylabel("arr_size")

ax[1][0].plot(local4['time'], local4['arr_size'])
ax[1][0].set_title(f"proc_n = 4")
ax[1][0].set_xlabel("time")
ax[1][0].set_ylabel("arr_size")

ax[1][1].plot(local8['time'], local8['arr_size'])
ax[1][1].set_title(f"proc_n = 8")
ax[1][1].set_xlabel("time")
ax[1][1].set_ylabel("arr_size")

plt.show()