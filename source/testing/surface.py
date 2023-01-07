# %%
import numpy as np


# %%
amplitude = 1
innerTessLevel = 5
r = 1 / innerTessLevel

u = 0.8
v = 0.8

U = np.array([0.5*0.5, 0.5, 1])
V = np.array([0.5*0.5, 0.5, 1])

dU = np.array([1, 1, 0])
dV = np.array([1, 1, 0])

biquadraticMt = np.array([[1, -2, 1],
                          [-2, 2, 0],
                          [1, 1, 0]]) / 2

# %%


def coeff(u, v):
    u %= 1
    v %= 1
    return amplitude * (0.5-abs(u-0.5))*(0.5-abs(v-0.5))


# %%
coeffecients = np.array([[coeff(u - r, v - r), coeff(u, v - r), coeff(u + r, v - r)],
                        [coeff(u - r, v), coeff(u, v), coeff(u + r, v)],
                        [coeff(u - r, v + r), coeff(u, v + r), coeff(u + r, v + r)]])

D = np.dot(biquadraticMt @ U, coeffecients @ biquadraticMt @ V)
x = biquadraticMt @ V
res = np.dot(x, coeffecients[:,1])
y = biquadraticMt @ dU
dUs = np.dot(y, coeffecients @ x)
dVs = np.dot(x, coeffecients @ y)


# %%
