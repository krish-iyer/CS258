import numpy as np
from scipy.optimize import minimize
import matplotlib.pyplot as plt

def wait_t(stage, clk):
    return (np.ceil(stage/clk)-(stage/clk))

def obj_best(x):
    if_t = 35
    id_t = 400
    reg_l_t = 200
    exec_t = 50
    reg_s_t = 200

    return (wait_t(if_t, x) + wait_t(id_t, x) + 2*wait_t(reg_l_t, x) + + wait_t(exec_t, x) + wait_t(reg_s_t, x))*x

def obj_worst(x):
    if_t = 35
    id_t = 400
    reg_l_t = 500
    exec_t = 400
    reg_s_t = 200

    return (wait_t(if_t, x) + wait_t(id_t, x) + 2*wait_t(reg_l_t, x) + + wait_t(exec_t, x) + wait_t(reg_s_t, x))*x

x = np.arange(165,1000, 1)
y = obj_worst(x)
x_min = np.min(y)
x_ind = np.argmin(y)
print("min: {} ; index: {} ; cycle time : {}".format(x_min,x_ind,x[x_ind]))
plt.plot(x,y)
plt.xlabel("clock cycle time(ps)")
plt.ylabel("unused wait time(ps)")
plt.show()
