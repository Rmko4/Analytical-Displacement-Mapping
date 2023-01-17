# %%
import numpy as np


# %%
amplitude = 1
innerTessLevel = 5
r = 1 / innerTessLevel

u = 0.4
v = 0.4

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


def subpatch_transform(t):
    return (tileSize * t - 0.5) % 1

# %%
def subpatch_transform(t):
    return (tileSize * t - 0.5) % 1

tess_amplitude = 2

def coeff(u, v):
    u %= 1
    v %= 1

    if (v <= u and 1. - u < v):
        tmp = u
        u = v
        v = 1. - tmp
    elif (v > u and 1. - u <= v):
        u = 1. - u
        v = 1. - v
    elif (v >= u and 1. - u > v):
        tmp = u
        u = 1. - v
        v = tmp
    if (u > 0.5):
        u = 1. - u
    if (v > 0.45):
        return 2 * tess_amplitude
    return min(1.0, v * 10.0) * tess_amplitude - tess_amplitude

vertU1 = 0.2499
vertU2 = 0.2501
vertV = 0.5

tileSize = 6
r = 1 / tileSize

u1 = subpatch_transform(vertU1)
u2 = subpatch_transform(vertU2)
v = subpatch_transform(vertV)

uC1 = vertU1 + r * (0.5 - u1)
uC2 = vertU2 + r * (0.5 - u2)
vC = vertV + r * (0.5 - v)

res = coeff(uC1, vC)

coeffecients1 = np.array([[coeff(uC1 - r, vC - r), coeff(uC1, vC - r), coeff(uC1 + r, vC - r)],
                        [coeff(uC1 - r, vC), coeff(uC1, vC), coeff(uC1 + r, vC)],
                        [coeff(uC1 - r, vC + r), coeff(uC1, vC + r), coeff(uC1 + r, vC + r)]])

coeffecients2 = np.array([[coeff(uC2 - r, vC - r), coeff(uC2, vC - r), coeff(uC2 + r, vC - r)],
                        [coeff(uC2 - r, vC), coeff(uC2, vC), coeff(uC2 + r, vC)],
                        [coeff(uC2 - r, vC + r), coeff(uC2, vC + r), coeff(uC2 + r, vC + r)]])


print(coeffecients1)
print(coeffecients2)

# u2 = 0.2
# v = 0.5
U1 = np.array([u1*u1, u1, 1])
U2 = np.array([u2*u2, u2, 1])
V = np.array([v*v, v, 1])

dU1 = np.array([2*u1, 1, 0])
dU2 = np.array([2*u2, 1, 0])
dV = np.array([2*v, 1, 0])

biquadraticMt = np.array([[1, -2, 1],
                          [-2, 2, 0],
                          [1, 1, 0]]) / 2

biquadraticM = np.array([[1, -2, 1],
                          [-2, 2, 1],
                          [1, 0, 0]]) / 2

D1 = np.dot(biquadraticM.T @ U1, coeffecients1.T @ biquadraticM.T @ V)
D2 = np.dot(biquadraticM.T @ U2, coeffecients2.T @ biquadraticM.T @ V)

x = biquadraticM @ V
res1 = np.dot(x, coeffecients2[:,2])
res2 = coeffecients2 @ x
y = biquadraticM @ U2
res3 = np.dot(y, coeffecients2[1])
dUs = np.dot(y, coeffecients2 @ x)
dVs = np.dot(x, coeffecients2 @ y)


dDdu1 = np.dot(biquadraticM @ V, coeffecients1 @ biquadraticM @ dU1)
dDdv1 = np.dot(biquadraticM @ dV, coeffecients1 @ biquadraticM @ U1)

dDdu2 = np.dot(biquadraticM @ V, coeffecients2 @ biquadraticM @ dU2)
dDdv2 = np.dot(biquadraticM @ dV, coeffecients2 @ biquadraticM @ U2)

# uc = u + (0.5 - uHat1) / tileSize
# %%
