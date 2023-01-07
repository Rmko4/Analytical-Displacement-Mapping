# %%
import numpy as np


# %%
amplitude = 1
innerTessLevel = 3
r = 1 / innerTessLevel

u = 2/3
v = 2/3

U = np.array([0.5*0.5, 0.5, 1])
V = np.array([0.5*0.5, 0.5, 1])

biquadraticMt = np.array([[1, -2, 1],
                          [-2, 2, 0],
                          [1, 1, 0]]) / 2

# %%


def coeff(u, v):
    u %= 1
    v %= 1
    return amplitude * (0.5-abs(u-0.5))*(0.5-abs(v-0.5))


# %%
coeffecients = np.array([[coeff(u - r, v + r), coeff(u, v + r), coeff(u + r, v + r)],
                        [coeff(u - r, v), coeff(u, v), coeff(u + r, v)],
                        [coeff(u - r, v - r), coeff(u, v - r), coeff(u + r, v - r)]])

D = np.dot(biquadraticMt @ U, coeffecients @ biquadraticMt @ V)

# %%
