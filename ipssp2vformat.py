#!/usr/bin/env python

"""
Converts IPSSP HFormat
	>id
	SEQUENCE
	HELHELHE
Into PSIPRED-like VFormat
"""

import sys

# psipred: coil, alpha, beta
# ipssp: L=coil, H=alpha, E=beta

translate = {
	'L': (1.0, 0.0, 0.0),
	'H': (0.0, 1.0, 0.0),
	'E': (0.0, 0.0, 1.0)
}

_, seq, struct = list(sys.stdin)
for i, (r, s) in enumerate(zip(seq.strip(), struct.strip())):
	print '%4d %s %s  %6.3f %6.3f %6.3f' % ((i + 1, r, s) + translate[s])
